/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#pragma once

#include "appstate.h"
#include <SDL3/SDL_video.h>
#include <lua.h>

void fln_system_init(fln_app_state *);

bool fln_shoulderminte();

int fln_luaopen_system(lua_State *L);
