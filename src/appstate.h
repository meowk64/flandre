#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <lua.h>

struct fln_app_state_t
{
    lua_State * L;
    SDL_Window * window;
    SDL_GLContext ogl_context;
};
