/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#include "timer.h"

#include <lauxlib.h>
#include <lua.h>

#include <SDL3/SDL_timer.h>

static int l_milliseconds(lua_State *L) {
	lua_pushinteger(L, (lua_Integer)SDL_GetTicks());
	return 1;
}

static int l_nanoseconds(lua_State *L) {
	lua_pushinteger(L, (lua_Integer)SDL_GetTicksNS());
	return 1;
}

static int l_counter(lua_State *L) {
	lua_pushinteger(L, (lua_Integer)SDL_GetPerformanceCounter());
	return 1;
}

int fln_luaopenimer(lua_State *L) {
	const luaL_Reg funcs[] = {
		{ "milliseconds", l_milliseconds },
		{ "nanoseconds", l_nanoseconds },
		{ "counter", l_counter },
		{ nullptr, nullptr },
	};
	luaL_newlib(L, funcs);
	return 1;
}