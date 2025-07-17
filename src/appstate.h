#pragma once

#include <SDL3/SDL_video.h>
#include <lua.h>

typedef struct fln_app_state_s
{
    lua_State * L;
    SDL_Window * window;
    SDL_GLContext ogl_context;
} fln_app_state_t;
