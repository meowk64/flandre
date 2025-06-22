#include "keyboard.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <uthash.h>
#include "memory.h"
#include <lauxlib.h>


struct key_state_entry_t {
    int key;
    bool state;
    UT_hash_handle hh;
};

static struct key_state_entry_t *key_states = nullptr;

static int l_is_down(lua_State * L)
{
    int key = luaL_checkinteger(L, 1);
    struct key_state_entry_t *entry;
    HASH_FIND_INT(key_states, &key, entry);
    lua_pushboolean(L, entry ? entry->state : false);
    return 1;
}

void fln_receive_keyboard_events(const SDL_Event * event)
{
    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP)
    {
        int key = event->key.scancode;
        bool is_down = (event->type == SDL_EVENT_KEY_DOWN);

        struct key_state_entry_t *entry;
        HASH_FIND_INT(key_states, &key, entry);

        if (entry)
        {
            entry->state = is_down;
        }
        else
        {
            entry = fln_allocate(sizeof(struct key_state_entry_t));
            if (entry)
            {
                entry->key = key;
                entry->state = is_down;
                HASH_ADD_INT(key_states, key, entry);
            }
        }
    }
}

void fln_clear_key_states()
{
    struct key_state_entry_t *entry, *tmp;
    HASH_ITER(hh, key_states, entry, tmp)
    {
        HASH_DEL(key_states, entry);
        fln_free(entry);
    }
}

int fln_luaopen_keyboard(lua_State *L)
{
    const luaL_Reg funcs[] = {
        {"is_down", l_is_down},
        {nullptr, nullptr}
    };
    luaL_newlib(L, funcs);
    return 1;
}
