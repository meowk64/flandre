#include "system.h"

#include <SDL3/SDL_video.h>
#include <lauxlib.h>
#include <lua.h>
#include "log.h"
#include "error.h"

static const char * const window_options[] = {
    "size",
    "title",
    "fullscreen", nullptr};

static SDL_Window * window;

static bool terminate = false;

static int l_window(lua_State * L)
{
    static bool is_fullscreen = false;

    int idx = luaL_checkoption(L, 1, nullptr, window_options);
    int top = lua_gettop(L);
    if (top == 1)
    {
        switch (idx)
        {
        case 0:
        {
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            lua_pushinteger(L, w);
            lua_pushinteger(L, h);
            return 2;
        }
        case 1:
        {
            const char * title = SDL_GetWindowTitle(window);
            lua_pushstring(L, title);
            return 1;
        }
        case 2:
        {
            lua_pushboolean(L, is_fullscreen);
            return 1;
        }
        default:
        {
            return fln_error(L, "it's impossible...");
        }
        }
    }
    else if (top >= 1)
    {
        switch (idx)
        {
        case 0:
        {
            int w = luaL_checkinteger(L, 2);
            int h = luaL_checkinteger(L, 3);
            SDL_SetWindowSize(window, w, h);
            return 0;
        }
        case 1:
        {
            SDL_SetWindowTitle(window, luaL_checkstring(L, 2));
            return 0;
        }
        case 2:
        {
            bool fs = lua_toboolean(L, 2);
            is_fullscreen = fs;
            SDL_SetWindowFullscreen(window, fs);
            return 0;
        }
        default:
        {
            return fln_error(L, "it's impossible...");
        }
        }
    }
    return fln_error(L, "invalid argument number (%d)", top);
}

static int l_terminate(lua_State * L)
{
    if (!terminate)
    {
        terminate = true;
        log_info("`flandre.system.terminate` has been called!");
        log_info("the program will be terminate at the next frame!");
    }
    else
    {
        return fln_error(L, "attempt to terminate the program twice");
    }
    return 0;
}

void fln_set_window_for_system_module(SDL_Window * win)   // WHAAAAT
{
    window = win;
}

bool fln_should_terminte()
{
    return terminate;
}

int fln_luaopen_system(lua_State * L)
{
    const luaL_Reg funcs[] = {
        {"window", l_window},
        {"terminate", l_terminate},
        {nullptr, nullptr}};
    luaL_newlib(L, funcs);
    return 1;
}
