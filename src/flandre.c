#include "flandre.h"
#include <lua.h>
#include "decoder.h"
#include "entity.h"
#include "graphics.h"
#include "keyboard.h"
#include "math.h"

int fln_luaopen(lua_State * L)
{
    lua_settop(L, 0);
    lua_newtable(L);

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

    lua_settop(L, 1);
    return 1;
}
