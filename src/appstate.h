/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#pragma once

#include <SDL3/SDL_video.h>

extern "C" {
#include <lua.h>
}

struct fln_app_state {
	lua_State *L;
	SDL_Window *window;
	SDL_GLContext ogl_context;
};
