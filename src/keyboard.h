#pragma once
#include <SDL3/SDL_events.h>
#include <lua.h>

void fln_receive_keyboard_events(const SDL_Event * event);

void fln_clear_key_states(void);

int fln_luaopen_keyboard(lua_State * L);
