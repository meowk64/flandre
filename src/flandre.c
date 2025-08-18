/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#include "flandre.h"
#include "data.h"
#include "callback.h"
#include "graphics.h"
#include "keyboard.h"
#include "math.h"
#include "mouse.h"
#include "system.h"
#include "timer.h"
#include <lua.h>

int fln_luaopen(lua_State *L) {
	lua_settop(L, 0);
	lua_newtable(L);

	lua_pushcfunction(L, fln_luaopen_system);
	lua_call(L, 0, 1);
	lua_setfield(L, 1, "system");

	lua_pushcfunction(L, fln_luaopen_timer);
	lua_call(L, 0, 1);
	lua_setfield(L, 1, "timer");

	lua_pushcfunction(L, fln_luaopen_data);
	lua_call(L, 0, 1);
	lua_setfield(L, 1, "data");

	lua_pushcfunction(L, fln_luaopen_callback);
	lua_call(L, 0, 1);
	lua_setfield(L, 1, "callback");

	lua_pushcfunction(L, fln_luaopen_math);
	lua_call(L, 0, 1);
	lua_setfield(L, 1, "math");

	lua_pushcfunction(L, fln_luaopen_graphics);
	lua_call(L, 0, 1);
	lua_setfield(L, 1, "graphics");

	lua_pushcfunction(L, fln_luaopen_keyboard);
	lua_call(L, 0, 1);
	lua_setfield(L, 1, "keyboard");

	lua_pushcfunction(L, fln_luaopen_mouse);
	lua_call(L, 0, 1);
	lua_setfield(L, 1, "mouse");

	lua_settop(L, 1);
	return 1;
}
