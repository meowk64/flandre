#pragma once

#include <SDL3/SDL_video.h>
#include <lua.h>

bool fln_iterate_entites(lua_State * L);

bool fln_draw_entities(lua_State * L);

int fln_luaopen_entity(lua_State * L);
