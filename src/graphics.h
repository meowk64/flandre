#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <lua.h>

#include "appstate.h"
#include "opengl/glad.h"

int fln_luaopen_graphics(lua_State * L);

SDL_WindowFlags fln_gfx_sdl_configure(struct fln_app_state_t * appstate);

bool fln_gfx_init_resource(struct fln_app_state_t * appstate);

void fln_gfx_begin_drawing(struct fln_app_state_t * appstate);

void fln_gfx_end_drawing(struct fln_app_state_t * appstate);

void fln_gfx_destroy_resource(struct fln_app_state_t * appstate);

void fln_gfx_receive_window_events(struct fln_app_state_t * appstate, const SDL_Event * event);