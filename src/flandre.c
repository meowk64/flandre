#include "flandre.h"
#include <lua.h>
#include "mouse.h"
#include "timer.h"
#include "decoder.h"
#include "entity.h"
#include "graphics.h"
#include "keyboard.h"
#include "math.h"
#include "system.h"

int fln_luaopen(lua_State * L)
{
    lua_settop(L, 0);
    lua_newtable(L);

    lua_pushcfunction(L, fln_luaopen_system);
    lua_call(L, 0, 1);
    lua_setfield(L, 1, "system");

    lua_pushcfunction(L, fln_luaopen_timer);
    lua_call(L, 0, 1);
    lua_setfield(L, 1, "timer");

    lua_pushcfunction(L, fln_luaopen_decoder);
    lua_call(L, 0, 1);
    lua_setfield(L, 1, "decoder");

    lua_pushcfunction(L, fln_luaopen_entity);
    lua_call(L, 0, 1);
    lua_setfield(L, 1, "entity");

    lua_pushcfunction(L, fln_luaopen_math);
    lua_call(L, 0, 1);
    lua_setfield(L, 1, "math");

    lua_pushcfunction(L, fln_luaopen_graphics);
    lua_call(L, 0, 1);
    lua_setfield(L, 1, "graphics");

    lua_pushcfunction(L, fln_luaopen_keyboard);
    lua_call(L, 0, 1);
    lua_setfield(L, 1, "keyboard");

    lua_pushcfunction(L, fln_luaopen_mouse);
    lua_call(L, 0, 1);
    lua_setfield(L, 1, "mouse");

    lua_settop(L, 1);
    return 1;
}
