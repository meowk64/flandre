#pragma once

#include <lua.h>
#include <SDL3/SDL_video.h>

void fln_set_window_for_system_module(SDL_Window *);

bool fln_should_terminte();

int fln_luaopen_system(lua_State * L);
