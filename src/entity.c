/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#include "entity.h"

#include "error.h"
#include <lauxlib.h>
#include <lua.h>

#include "memory.h"

typedef struct entity_node_s {
	int id;
	int layer;
	struct entity_node_s *next;
	struct entity_node_s *prev;
} entity_node_t;

typedef struct entities_layer_s {
	entity_node_t *first;
	entity_node_t *last;
} entities_layer_t;

// 只是用来作为键的静态值
static int KEY = 0x0d000721;

#define ENTITIES_LAYERS_SIZE 16

static entities_layer_t entities_layers[ENTITIES_LAYERS_SIZE] = { 0 };

#define get_entities_table(L) lua_rawgetp(L, LUA_REGISTRYINDEX, &KEY)

static void iterate_layer(lua_State *L, entities_layer_t layer, const char *func_name) {
	lua_settop(L, 0);
	get_entities_table(L);
	entity_node_t *current = layer.first;
	while (current) {
		lua_settop(L, 1);
		lua_rawgeti(L, 1, current->id);
		// kill() 函数可能会在处理回调时被调用，
		// 所以提前更新 current 防止 kill 掉以后变成空指针
		current = current->next;

		if (lua_getfield(L, 2, "act")) {
			if (!lua_toboolean(L, 3)) {
				continue;
			}
		}
		lua_settop(L, 2);

		if (lua_getfield(L, 2, func_name) == LUA_TFUNCTION) {
			lua_pushvalue(L, 2);
			int code = lua_pcall(L, 1, 0, 0);
			if (code != LUA_OK) {
				// 如果出现问题，将该对象的 `act` 改为不活跃，以避免重复执行出错的函数
				lua_pushboolean(L, false);
				lua_setfield(L, 2, "act");
				printf("(in runtime) (%s|error code: %d) %s\n", func_name, code, lua_tostring(L, -1));
			}
		}
		lua_settop(L, 2);
	}
}

static int l_new(lua_State *L) {
	int layer = luaL_optinteger(L, 1, 0);
	if (layer > 16) {
		fln_warning("attempt to create an entity at layer %d (>16), it has been limited to 16", layer);
		layer = 16;
	} else if (layer < 0) {
		fln_warning("attempt to create an entity at layer %d (<0), it has been limited to 0", layer);
		layer = 0;
	}
	lua_settop(L, 0);
	get_entities_table(L);
	lua_newtable(L);
	int ref = luaL_ref(L, -2);

	entity_node_t *new_node = (entity_node_t *)fln_alloc(sizeof(entity_node_t));
	new_node->id = ref;
	new_node->layer = 0;
	new_node->next = nullptr;
	new_node->prev = nullptr;

	if (new_node == nullptr) {
		return fln_error(L, "failed to create new entity node");
	}
	if (entities_layers[layer].first == nullptr) {
		entities_layers[layer].first = new_node;
		entities_layers[layer].last = new_node;
	} else {
		entities_layers[layer].last->next = new_node;
		new_node->prev = entities_layers[layer].last;
		entities_layers[layer].last = new_node;
	}

	lua_rawgeti(L, -1, ref);

	entities_layers[layer].last->layer = layer;
	lua_pushlightuserdata(L, (void *)entities_layers[layer].last);
	lua_rawseti(L, -2, 0);
	lua_pushboolean(L, 1);
	lua_setfield(L, -2, "act");
	return 1;
}

static int l_kill(lua_State *L) {
	lua_settop(L, 1);
	if (lua_rawgeti(L, 1, 0) != LUA_TLIGHTUSERDATA) {
		return 0;
	}
	entity_node_t *entity = (entity_node_t *)lua_topointer(L, -1);
	size_t layer = entity->layer;
	lua_pushnil(L);
	lua_rawseti(L, 1, 0);

	if (entities_layers[layer].first == entities_layers[layer].last) {
		entities_layers[layer].first = nullptr;
		entities_layers[layer].last = nullptr;
	} else if (entity == entities_layers[layer].first) {
		entities_layers[layer].first = entity->next;
		entity->next->prev = nullptr;
	} else if (entity == entities_layers[layer].last) {
		entities_layers[layer].last = entity->prev;
		entity->prev->next = nullptr;
	} else {
		entity->prev->next = entity->next;
		entity->next->prev = entity->prev;
	}
	get_entities_table(L);
	luaL_unref(L, -1, entity->id);
	fln_free((void *)entity);
	return 0;
}

static int pri_l_iterate(lua_State *L) {
	for (size_t i = 0; i < ENTITIES_LAYERS_SIZE; i++) {
		if (entities_layers[i].first) {
			iterate_layer(L, entities_layers[i], "itr");
		}
	}
	return 0;
}

static int pri_l_draw(lua_State *L) {
	for (size_t i = 0; i < ENTITIES_LAYERS_SIZE; i++) {
		if (entities_layers[i].first) {
			iterate_layer(L, entities_layers[i], "drw");
		}
	}
	return 0;
}

bool fln_iterate_entites(lua_State *L) {
	lua_settop(L, 0);
	lua_pushcfunction(L, pri_l_iterate);
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		printf("(unexpected) lua: %s\n", lua_tostring(L, -1));
		return false;
	}
	return true;
}

bool fln_draw_entities(lua_State *L) {
	lua_settop(L, 0);
	lua_pushcfunction(L, pri_l_draw);
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		printf("(unexpected) lua: %s\n", lua_tostring(L, -1));
		return false;
	}
	return true;
}

int fln_luaopen_entity(lua_State *L) {
	lua_newtable(L);
	lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY);
	const luaL_Reg funcs[] = { { "new", l_new }, { "kill", l_kill }, { nullptr, nullptr } };
	luaL_newlib(L, funcs);
	return 1;
}
