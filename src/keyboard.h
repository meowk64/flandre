#pragma once
#include <lua.h>
#include <SDL3/SDL_events.h>

void fln_receive_keyboard_events(const SDL_Event * event);

void fln_clear_key_states(void);

int fln_luaopen_keyboard(lua_State * L);
