#include "mouse.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "appstate.h"

#include "entity.h"
#include "flandre.h"
#include "graphics.h"
#include "keyboard.h"
#include "log.h"
#include "memory.h"
#include "opengl/glad.h"
#include "system.h"

int SDL_AppInit(void ** appstate_, int argc, char * argv[])
{
    log_info("Flandre game framework | dev-0.1.0");
    log_info("SDL version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MINOR_VERSION);
    log_info("Lua version: %s.%s.%s", LUA_VERSION_MAJOR, LUA_VERSION_MINOR, LUA_VERSION_RELEASE);
    log_info("initalizing basic resource...");
    if (!SDL_SetAppMetadata("Flandre Game Framework", "0.1.0 dev", "flandre"))
    {
        return SDL_APP_FAILURE;
    }
    *appstate_ = fln_alloc(sizeof(struct fln_app_state_t));
    struct fln_app_state_t * appstate = (struct fln_app_state_t *)*appstate_;
    if (!appstate)
    {
        log_error("cannot allocate memory for fln_app_state_t");
    }
    memset(appstate, 0, sizeof(struct fln_app_state_t));
    log_info("initializing Lua...");
    appstate->L = luaL_newstate();
    if (!appstate)
    {
        log_error("cannot allocate memory for lua_State");
    }
    log_info("opening Lua libraries...");
    luaL_openlibs(appstate->L);
    luaL_requiref(appstate->L, "flandre", fln_luaopen, true);

    log_info("initializing SDL subsystem (SDL_INIT_VIDEO | SDL_INIT_EVENTS)...");
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        log_error("failed to call SDL_InitSubSystem()");
        return SDL_APP_FAILURE;
    }
    SDL_WindowFlags window_flags = fln_gfx_sdl_configure(appstate);
    appstate->window = SDL_CreateWindow("Untitled", 1366, 768, window_flags | SDL_WINDOW_RESIZABLE);
    if (!appstate->window)
    {
        log_error("failed to call SDL_CreateWindow()");
        return SDL_APP_FAILURE;
    }
    fln_set_window_for_system_module(appstate->window);
    log_info("initializing graphics device...");
    if (!fln_gfx_init_resource(appstate))
    {
        return SDL_APP_FAILURE;
    }
    SDL_ShowWindow(appstate->window);
    log_info("loading program entry...");
    if (luaL_dofile(appstate->L, "main.lua"))
    {
        log_error("(in entry) %s", lua_tostring(appstate->L, -1));
    }
    log_info("done!");
    return SDL_APP_CONTINUE;
}

int SDL_AppIterate(void * appstate_)
{
    struct fln_app_state_t * appstate = (struct fln_app_state_t *)appstate_;
    if (fln_should_terminte())
    {
        return SDL_APP_SUCCESS;
    }
    // uint64_t frame_start = SDL_GetTicks();
    if (!fln_iterate_entites(appstate->L))
    {
        return SDL_APP_FAILURE;
    }
    fln_gfx_begin_drawing(appstate);
    if (!fln_draw_entities(appstate->L))
    {
        return SDL_APP_FAILURE;
    }
    fln_gfx_end_drawing(appstate);
    /*
    uint64_t frame_time = SDL_GetTicks() - frame_start;
    if (frame_time <= 15)
    {
        SDL_Delay(15 - frame_time);
    }*/
    return SDL_APP_CONTINUE;
}

int SDL_AppEvent(void * appstate_, const SDL_Event * event)
{
    struct fln_app_state_t * appstate = (struct fln_app_state_t *)appstate_;
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }
    else
    {
        fln_receive_keyboard_events(event);
        fln_receive_mouse_events(event);
        fln_gfx_receive_window_events(appstate, event);
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void * appstate_)
{
    struct fln_app_state_t * appstate = (struct fln_app_state_t *)appstate_;
    lua_close(appstate->L);
    // lua虚拟机一定要最先关闭，否则一些资源会丢失上下文（例如OpenGL资源会在上下文已经释放过后再释放）
    fln_gfx_destroy_resource(appstate);
    fln_clear_key_states();
    SDL_DestroyWindow(appstate->window);
    fln_free(appstate);
}
