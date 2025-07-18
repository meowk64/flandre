#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <lua.h>

#include "appstate.h"

#define FLN_USERTYPE_PIPELINE "fln.pipeline"
#define FLN_USERTYPE_MESH "fln.mesh"
#define FLN_USERTYPE_TEXTURE2D "fln.texture2d"

struct fln_gfx_backend_t
{
    SDL_WindowFlags (*sdl_configure)(struct fln_app_state_t * appstate);
    bool (*init_resource)(struct fln_app_state_t * appstate);
    bool (*begin_drawing)(struct fln_app_state_t * appstate);
    bool (*end_drawing)(struct fln_app_state_t * appstate);
    bool (*destroy_resource)(struct fln_app_state_t * appstate);
    void (*receive_window_events)(struct fln_app_state_t * appstate, const SDL_Event * event);
    lua_CFunction l_pipeline;
    lua_CFunction l_pipeline_release;
    lua_CFunction l_pipeline_uniform;
    //lua_CFunction l_pipeline_texture;
    lua_CFunction l_pipeline_submit;
    lua_CFunction l_mesh;
    lua_CFunction l_mesh_release;
    lua_CFunction l_texture2d;
    lua_CFunction l_texture2d_release;
};
