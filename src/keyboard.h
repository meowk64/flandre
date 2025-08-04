/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#pragma once
#include <SDL3/SDL_events.h>
#include <lua.h>

void fln_receive_keyboard_events(const SDL_Event *event);

void fln_clear_key_states(void);

int fln_luaopen_keyboard(lua_State *L);
