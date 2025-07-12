#include "mouse.h"
#include <lauxlib.h>
#include <SDL3/SDL_mouse.h>
#include <lua.h>

static int l_position(lua_State * L)
{
    float x, y;
    SDL_GetMouseState(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

static int l_button(lua_State * L)
{
    SDL_MouseButtonFlags flag = SDL_GetMouseState(nullptr, nullptr);
    lua_pushboolean(L, flag & SDL_BUTTON_LMASK);
    lua_pushboolean(L, flag & SDL_BUTTON_RMASK);
    lua_pushboolean(L, flag & SDL_BUTTON_MMASK);
    return 3;
}

void fln_receive_mouse_events(const SDL_Event * event)
{
}

int fln_luaopen_mouse(lua_State * L)
{
    const luaL_Reg funcs[] = {
        {"position", l_position},
        {"button", l_button},
        {nullptr, nullptr},
    };
    luaL_newlib(L, funcs);
    return 1;
}