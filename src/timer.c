#include "timer.h"

#include <lua.h>
#include <lauxlib.h>

#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_time.h>

int l_milliseconds(lua_State * L)
{
    lua_pushinteger(L, (lua_Integer)SDL_GetTicks());
    return 1;
}

int l_nanoseconds(lua_State * L)
{
    lua_pushinteger(L, (lua_Integer)SDL_GetTicksNS());
    return 1;
}

int l_counter(lua_State * L)
{
    lua_pushinteger(L, (lua_Integer)SDL_GetPerformanceCounter());
    return 1;
}

int fln_luaopen_timer(lua_State *L)
{
    const luaL_Reg funcs[] = {
        {"milliseconds", l_milliseconds},
        {"nanoseconds", l_nanoseconds},
        {"counter", l_counter},
        {nullptr, nullptr},
    };
    luaL_newlib(L, funcs);
    return 1;
}