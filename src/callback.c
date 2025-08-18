#include "callback.h"
#include <lauxlib.h>
#include <lua.h>

static int KEY_ITERATE_FUNC = 0x0d000721;
static int KEY_DRAW_FUNC = 0x0009961;
//static int KEY_EVENT_FUNC = 0x0004545;

static int l_placeholder(lua_State *L) {
	return 0;
}

static int l_iterate(lua_State *L) {
	lua_settop(L, 1);
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY_ITERATE_FUNC);
	return 0;
}

static int l_draw(lua_State *L) {
	lua_settop(L, 1);
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY_DRAW_FUNC);
	return 0;
}

/*
static int l_event(lua_State *L) {
	lua_settop(L, 1);
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY_EVENT_FUNC);
	return 0;
}
*/

void fln_iterate(lua_State *L) {
	lua_rawgetp(L, LUA_REGISTRYINDEX, &KEY_ITERATE_FUNC);
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		printf("(in iterate callback) lua: %s\n", lua_tostring(L, -1));
		lua_pushcfunction(L, l_placeholder);
		lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY_ITERATE_FUNC);
	}
}

void fln_draw(lua_State *L) {
	lua_rawgetp(L, LUA_REGISTRYINDEX, &KEY_DRAW_FUNC);
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		printf("(in draw callback) lua: %s\n", lua_tostring(L, -1));
		lua_pushcfunction(L, l_placeholder);
		lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY_DRAW_FUNC);
	}
}

int fln_luaopen_callback(lua_State *L) {
    lua_pushcfunction(L, l_placeholder);
    lua_pushvalue(L, -1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY_ITERATE_FUNC);
    lua_rawsetp(L, LUA_REGISTRYINDEX, &KEY_DRAW_FUNC);
	const luaL_Reg funcs[] = {
		{ "iterate", l_iterate },
		{ "draw", l_draw },
		{ nullptr, nullptr }
	};
	luaL_newlib(L, funcs);
	return 1;
}
