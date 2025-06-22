#include "graphics.h"

#include <SDL3/SDL_video.h>
#include <lua.h>
#include <lauxlib.h>

#include "gfx_backend_ogl.h"
#include "gfx_interface.h"
#include "log.h"
#include "opengl/glad.h"

static struct fln_gfx_backend_t backend;

SDL_WindowFlags fln_gfx_sdl_configure(struct fln_app_state_t * appstate)
{
    if (backend.sdl_configure)
    {
        return backend.sdl_configure(appstate);
    }
    else
    {
        log_error("graphics backend invalid");
        return 0;
    }
}

bool fln_gfx_init_resource(struct fln_app_state_t * appstate)
{
    if (backend.init_resource)
    {
        return backend.init_resource(appstate);
    }
    else
    {
        log_error("graphics backend invalid");
        return false;
    }
}

void fln_gfx_begin_drawing(struct fln_app_state_t * appstate)
{
    if (backend.begin_drawing)
    {
        backend.begin_drawing(appstate);
    }
    else
    {
        log_error("graphics backend invalid");
    }
}

void fln_gfx_end_drawing(struct fln_app_state_t * appstate)
{
    if (backend.end_drawing)
    {
        backend.end_drawing(appstate);
    }
    else
    {
        log_error("graphics backend invalid");
    }
}

void fln_gfx_destroy_resource(struct fln_app_state_t * appstate)
{
    if (backend.destroy_resource)
    {
        backend.destroy_resource(appstate);
    }
    else
    {
        log_error("graphics backend invalid");
    }
}

void fln_gfx_receive_window_events(struct fln_app_state_t * appstate, const SDL_Event * event)
{
    if (backend.receive_window_events)
    {
        backend.receive_window_events(appstate, event);
    }
    else
    {
        log_error("graphics backend invalid");
    }
}

int fln_luaopen_graphics(lua_State * L)
{
    log_warn("TODO: add other graphical backends");
    log_warn("current graphical backend: OpenGL");
    backend = fln_gfx_init_backend_ogl();
    const luaL_Reg funcs[] = {{"pipeline", backend.l_pipeline},
                              {"mesh", backend.l_mesh},
                              {"texture2d", backend.l_texture2d},
                              {nullptr, nullptr}};
    const luaL_Reg meths_pipeline[] = {{"uniform", backend.l_pipeline_uniform},
                                       {"submit", backend.l_pipeline_submit},
                                       {"release", backend.l_pipeline_release},
                                       {"texture", backend.l_pipeline_texture},
                                       {"__gc", backend.l_pipeline_release},
                                       {nullptr, nullptr}};
    const luaL_Reg meths_mesh[] = {
        {"release", backend.l_mesh_release}, {"__gc", backend.l_mesh_release}, {nullptr, nullptr}};
    const luaL_Reg meths_texture[] = {
        {"release", backend.l_texture_release}, {"__gc", backend.l_texture_release}, {nullptr, nullptr}};

    luaL_newmetatable(L, FLN_USERTYPE_PIPELINE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, meths_pipeline, 0);

    luaL_newmetatable(L, FLN_USERTYPE_TEXTURE2D);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, meths_texture, 0);

    luaL_newmetatable(L, FLN_USERTYPE_MESH);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, meths_mesh, 0);

    luaL_newlib(L, funcs);
    return 1;
}
