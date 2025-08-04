/*
	This file is part of Flandre
	Copyright (C) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.
*/
#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <lua.h>

#include "appstate.h"
#include "opengl/glad.h"

int fln_luaopen_graphics(lua_State *L);

SDL_WindowFlags fln_gfx_sdl_configure(fln_app_state_t *appstate);

bool fln_gfx_init_resource(fln_app_state_t *appstate);

void fln_gfx_begin_drawing(fln_app_state_t *appstate);

void fln_gfx_end_drawing(fln_app_state_t *appstate);

void fln_gfx_destroy_resource(fln_app_state_t *appstate);

void fln_gfx_receive_window_events(fln_app_state_t *appstate, const SDL_Event *event);