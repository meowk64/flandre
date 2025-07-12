#pragma once

#include <lua.h>
#include <SDL3/SDL_events.h>

void fln_receive_mouse_events(const SDL_Event *);

int fln_luaopen_mouse(lua_State *);