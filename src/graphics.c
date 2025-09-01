/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#include "graphics.h"

#include <SDL3/SDL_video.h>
#include <lauxlib.h>
#include <lua.h>

#include "gfx_backend_ogl.h"
#include "gfx_interface.h"
#include "opengl/glad.h"

static fln_gfx_backend backend;

SDL_WindowFlags fln_gfx_sdl_configure(fln_app_state *appstate) {
	if (backend.sdl_configure) {
		return backend.sdl_configure(appstate);
	} else {
		printf("graphics backend invalid\n");
		return 0;
	}
}

bool fln_gfx_init(fln_app_state *appstate) {
	if (backend.init) {
		return backend.init(appstate);
	} else {
		printf("graphics backend invalid\n");
		return false;
	}
}

void fln_gfx_begin_drawing(fln_app_state *appstate) {
	if (backend.begin_drawing) {
		backend.begin_drawing(appstate);
	} else {
		printf("graphics backend invalid\n");
	}
}

void fln_gfx_end_drawing(fln_app_state *appstate) {
	if (backend.end_drawing) {
		backend.end_drawing(appstate);
	} else {
		printf("graphics backend invalid\n");
	}
}

void fln_gfx_destroy_resource(fln_app_state *appstate) {
	if (backend.destroy_resource) {
		backend.destroy_resource(appstate);
	} else {
		printf("graphics backend invalid\n");
	}
}

void fln_gfx_receive_window_events(fln_app_state *appstate, const SDL_Event *event) {
	if (backend.receive_window_events) {
		backend.receive_window_events(appstate, event);
	} else {
		printf("graphics backend invalid\n");
	}
}

int fln_luaopen_graphics(lua_State *L) {
	backend = fln_gfx_init_backend_ogl();
	const luaL_Reg funcs[] = { { "pipeline", backend.l_pipeline },
		{ "mesh", backend.l_mesh },
		{ "texture2d", backend.l_texture2d },
		{ nullptr, nullptr } };
	const luaL_Reg meths_pipeline[] = { { "uniform", backend.l_pipeline_uniform },
		{ "submit", backend.l_pipeline_submit },
		{ "submit_instanced", backend.l_pipeline_submit_instanced },
		{ "release", backend.l_pipeline_release },
		//{"texture", backend.l_pipelineexture},
		{ "__gc", backend.l_pipeline_release },
		{ nullptr, nullptr } };
	const luaL_Reg meths_mesh[] = {
		{ "release", backend.l_mesh_release },
		{ "__gc", backend.l_mesh_release },
		{ nullptr, nullptr }
	};
	const luaL_Reg methsexture[] = {
		{ "size", backend.l_texture2d_size },
		{ "release", backend.l_texture2d_release },
		{ "__gc", backend.l_texture2d_release },
		{ nullptr, nullptr }
	};

	luaL_newmetatable(L, FLN_USERTYPE_PIPELINE);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, meths_pipeline, 0);

	luaL_newmetatable(L, FLN_USERTYPE_TEXTURE2D);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, methsexture, 0);

	luaL_newmetatable(L, FLN_USERTYPE_MESH);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, meths_mesh, 0);

	luaL_newlib(L, funcs);
	return 1;
}
