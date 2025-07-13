#include "mouse.h"
#include <SDL3/SDL_events.h>
#include <lauxlib.h>
#include <SDL3/SDL_mouse.h>
#include <lua.h>

static float mouse_wheel_status = 0; // 

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

static int l_wheel(lua_State * L)
{
    bool reset = lua_toboolean(L, 1);
    lua_pushnumber(L, mouse_wheel_status);
    if(reset)
    {
        mouse_wheel_status = 0;
    }
    return 1;
}

void fln_receive_mouse_events(const SDL_Event * event)
{
    if (event->type == SDL_EVENT_MOUSE_WHEEL)
    {
        mouse_wheel_status += event->wheel.y;
    }
}

int fln_luaopen_mouse(lua_State * L)
{
    const luaL_Reg funcs[] = {
        {"position", l_position},
        {"button", l_button},
        {"wheel", l_wheel},
        {nullptr, nullptr},
    };
    luaL_newlib(L, funcs);
    return 1;
}
