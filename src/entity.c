#include "entity.h"

#include <lauxlib.h>
#include <lua.h>
#include "log.h"

#include "memory.h"

struct entity_node
{
    int id;
    struct entity_node * next;
    struct entity_node * prev;
} entity_node;

struct entities_layer
{
    struct entity_node * first;
    struct entity_node * last;
} entities_layer;

// 只是用来作为键的静态值
static int KEY = 0x0d000721;

#define ENTITIES_LAYERS_SIZE 16

static struct entities_layer entities_layers[ENTITIES_LAYERS_SIZE] = {0};

#define get_entities_table(L) lua_rawgetp(L, LUA_REGISTRYINDEX, &KEY)

static struct entity_node * create_entity_node(int ref)
{
    struct entity_node * new_node = (struct entity_node *)fln_alloc(sizeof(entity_node));
    if (new_node == nullptr)
    {
        log_error("failed to allocate memory for new entity node");
        return nullptr;
    }
    new_node->id = ref;
    new_node->next = nullptr;
    new_node->prev = nullptr;
    return new_node;
}

static void iterate_layer(lua_State * L, struct entities_layer layer, const char * func_name)
{
    lua_settop(L, 0);
    get_entities_table(L);
    struct entity_node * current = layer.first;
    while (current)
    {
        lua_settop(L, 1);
        lua_rawgeti(L, 1, current->id);
        // kill() 函数可能会在处理回调时被调用，
        // 所以提前更新 current 防止 kill 掉以后变成空指针
        current = current->next;

        if (lua_getfield(L, 2, "act"))
        {
            if (!lua_toboolean(L, 3))
            {
                continue;
            }
        }
        lua_settop(L, 2);

        if (lua_getfield(L, 2, func_name) == LUA_TFUNCTION)
        {
            lua_pushvalue(L, 2);
            int code = lua_pcall(L, 1, 0, 0);
            if (code != LUA_OK)
            {
                log_error("(%s|error code: %d) lua: %s", func_name, code, lua_tostring(L, -1));
            }
        }
        lua_settop(L, 2);
    }
}

static int l_new(lua_State * L)
{
    int layer = luaL_optinteger(L, 1, 0);
    if (layer > 16)
    {
        log_warn("attempt to create an entity at layer %d (>16), it has been changed to 16", layer);
        layer = 16;
    }
    else if (layer < 0)
    {
        log_warn("attempt to create an entity at layer %d (<16), it has been changed to 0", layer);
        layer = 0;
    }
    lua_settop(L, 0);
    get_entities_table(L);
    lua_newtable(L);
    int ref = luaL_ref(L, -2);

    struct entity_node * new_node = create_entity_node(ref);
    if (new_node == nullptr)
    {
        return luaL_error(L, "failed to create new entity node");
    }

    if (entities_layers[layer].first == nullptr)
    {
        entities_layers[layer].first = new_node;
        entities_layers[layer].last = new_node;
    }
    else
    {
        entities_layers[layer].last->next = new_node;
        new_node->prev = entities_layers[layer].last;
        entities_layers[layer].last = new_node;
    }

    lua_rawgeti(L, -1, ref);

    lua_pushlightuserdata(L, (void *)entities_layers[layer].last);
    lua_rawseti(L, -2, 0);
    lua_pushinteger(L, layer);
    lua_rawseti(L, -2, -1);
    lua_pushboolean(L, 1);
    lua_setfield(L, -2, "act");
    return 1;
}

static int l_kill(lua_State * L)
{
    lua_settop(L, 1);
    if (lua_rawgeti(L, 1, 0) != LUA_TLIGHTUSERDATA)
    {
        return 0;
    }
    struct entity_node * entity = (struct entity_node *)lua_topointer(L, -1);
    size_t layer = lua_rawgeti(L, 1, -1);
    lua_pushnil(L);
    lua_rawseti(L, 1, 0);

    if (entities_layers[layer].first == entities_layers[layer].last)
    {
        entities_layers[layer].first = nullptr;
        entities_layers[layer].last = nullptr;
    }
    else if (entity == entities_layers[layer].first)
    {
        entities_layers[layer].first = entity->next;
        entity->next->prev = nullptr;
    }
    else if (entity == entities_layers[layer].last)
    {
        entities_layers[layer].last = entity->prev;
        entity->prev->next = nullptr;
    }
    else
    {
        entity->prev->next = entity->next;
        entity->next->prev = entity->prev;
    }
    get_entities_table(L);
    luaL_unref(L, -1, entity->id);
    fln_free((void *)entity);
    return 0;
}

static int pri_l_iterate(lua_State * L)
{
    for (size_t i = 0; i < ENTITIES_LAYERS_SIZE; i++)
    {
        if (entities_layers[i].first)
        {
            iterate_layer(L, entities_layers[i], "itr");
        }
    }
    return 0;
}

static int pri_l_draw(lua_State * L)
{
    for (size_t i = 0; i < ENTITIES_LAYERS_SIZE; i++)
    {
        if (entities_layers[i].first)
        {
            iterate_layer(L, entities_layers[i], "drw");
        }
    }
    return 0;
}

bool fln_iterate_entites(lua_State * L)
{
    lua_settop(L, 0);
    lua_pushcfunction(L, pri_l_iterate);
    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        log_error("(unexpected) lua: %s", lua_tostring(L, -1));
        return false;
    }
    return true;
}

bool fln_draw_entities(lua_State * L)
{
    lua_settop(L, 0);
    lua_pushcfunction(L, pri_l_draw);
    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        log_error("(unexpected) lua: %s", lua_tostring(L, -1));
        return false;
    }
    return true;
}

int fln_luaopen_entity(lua_State * L)
{
    lua_newtable(L);
    lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY);
    const luaL_Reg funcs[] = {{"new", l_new}, {"kill", l_kill}, {nullptr, nullptr}};
    luaL_newlib(L, funcs);
    return 1;
}
