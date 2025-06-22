#include "math.h"
#include <cglm/cglm.h>
#include <cglm/types-struct.h>
#include <lua.h>
#include <lauxlib.h>
#include <cglm/struct.h>
#include "memory.h"

static int l_transform(lua_State * L)
{
    bool identity = lua_toboolean(L, 1);
    mat4s ** transform = lua_newuserdata(L, sizeof(mat4s *));
    *transform = fln_allocate(sizeof(mat4s));
    luaL_setmetatable(L, FLN_USERTYPE_TRANSFORM);

    if (identity)
    {
        **transform = glms_mat4_identity();
    }
    else
    {
        **transform = glms_mat4_zero();
    }

    return 1;
}

static int l_m_transform_set(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float * values = (float *)luaL_checkstring(L, 2);
        size_t len = lua_rawlen(L, 2);
        if (len != sizeof(mat4))
        {
            return luaL_error(L, "invalid matrix size");
        }
        (*transform)->m00 = values[0];
        (*transform)->m01 = values[1];
        (*transform)->m02 = values[2];
        (*transform)->m03 = values[3];
        (*transform)->m10 = values[4];
        (*transform)->m11 = values[5];
        (*transform)->m12 = values[6];
        (*transform)->m13 = values[7];
        (*transform)->m20 = values[8];
        (*transform)->m21 = values[9];
        (*transform)->m22 = values[10];
        (*transform)->m23 = values[11];
        (*transform)->m30 = values[12];
        (*transform)->m31 = values[13];
        (*transform)->m32 = values[14];
        (*transform)->m33 = values[15];
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_single(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        int ix = luaL_checkinteger(L, 2);
        int iy = luaL_checkinteger(L, 3);
        double value = luaL_checknumber(L, 4);
        if (ix < 1 || ix > 4 || iy < 1 || iy > 4)
        {
            return luaL_error(L, "invalid matrix index");
        }
        (*transform)->raw[ix - 1][iy - 1] = (float)value;
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_translate(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        float z = luaL_checknumber(L, 4);
        **transform = glms_translate(**transform, (vec3s){x, y, z});
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_rotate(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float angle = luaL_checknumber(L, 2);
        float x = luaL_checknumber(L, 3);
        float y = luaL_checknumber(L, 4);
        float z = luaL_checknumber(L, 5);
        **transform = glms_rotate(**transform, angle, (vec3s){x, y, z});
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_rotate_at(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float angle = luaL_checknumber(L, 2);
        float at_x = luaL_checknumber(L, 3);
        float at_y = luaL_checknumber(L, 4);
        float at_z = luaL_checknumber(L, 5);
        float x = luaL_checknumber(L, 6);
        float y = luaL_checknumber(L, 7);
        float z = luaL_checknumber(L, 8);
        **transform = glms_rotate_at(**transform, (vec3s){at_x, at_y, at_z}, angle, (vec3s){x, y, z});
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_spin(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float angle = luaL_checknumber(L, 2);
        float x = luaL_checknumber(L, 3);
        float y = luaL_checknumber(L, 4);
        float z = luaL_checknumber(L, 5);
        **transform = glms_spin(**transform, angle, (vec3s){x, y, z});
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_scale(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        float z = luaL_checknumber(L, 4);
        **transform = glms_scale(**transform, (vec3s){x, y, z});
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_identity(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        **transform = glms_mat4_identity();
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_zero(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        **transform = glms_mat4_zero();
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_ortho(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float left = luaL_checknumber(L, 2);
        float right = luaL_checknumber(L, 3);
        float bottom = luaL_checknumber(L, 4);
        float top = luaL_checknumber(L, 5);
        float near = luaL_checknumber(L, 6);
        float far = luaL_checknumber(L, 7);
        **transform = glms_ortho(left, right, bottom, top, near, far);
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_perspective(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float fov = luaL_checknumber(L, 2);
        float aspect = luaL_checknumber(L, 3);
        float near = luaL_checknumber(L, 4);
        float far = luaL_checknumber(L, 5);
        **transform = glms_perspective(fov, aspect, near, far);
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_lookat(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float eye_x = luaL_checknumber(L, 2);
        float eye_y = luaL_checknumber(L, 3);
        float eye_z = luaL_checknumber(L, 4);
        float center_x = luaL_checknumber(L, 5);
        float center_y = luaL_checknumber(L, 6);
        float center_z = luaL_checknumber(L, 7);
        float up_x = luaL_checknumber(L, 8);
        float up_y = luaL_checknumber(L, 9);
        float up_z = luaL_checknumber(L, 10);
        **transform = glms_lookat((vec3s){eye_x, eye_y, eye_z}, (vec3s){center_x, center_y, center_z}, (vec3s){up_x, up_y, up_z});
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_viewport(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        float width = luaL_checknumber(L, 4);
        float height = luaL_checknumber(L, 5);
        **transform = glms_ortho(x, x + width, y + height, y, -1.0f, 1.0f);
    }
    else
    {
        return luaL_error(L, "invalid transform");
    }
    return 0;
}

static int l_m_transform_multiply(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        mat4s * other = luaL_checkudata(L, 2, FLN_USERTYPE_TRANSFORM);
        if (other)
        {
            **transform = glms_mat4_mul(**transform, *other);
        }
        else
        {
            return luaL_error(L, "invalid transform (former)");
        }
    }
    else
    {
        return luaL_error(L, "invalid transform (latter)");
    }
    return 0;
}

static int l_m_transform_release(lua_State * L)
{
    mat4s ** transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
    if (transform && *transform)
    {
        fln_free((void *)*transform);
        *transform = nullptr;
    }
    return 0;
}

int fln_luaopen_math(lua_State *L)
{
    const luaL_Reg meths_transform[] = {
        {"set", l_m_transform_set},
        {"single", l_m_transform_single},
        {"translate", l_m_transform_translate},
        {"rotate", l_m_transform_rotate},
        {"rotate_at", l_m_transform_rotate_at},
        {"spin", l_m_transform_spin},
        {"scale", l_m_transform_scale},
        {"identity", l_m_transform_identity},
        {"zero", l_m_transform_zero},
        {"ortho", l_m_transform_ortho},
        {"perspective", l_m_transform_perspective},
        {"lookat", l_m_transform_lookat},
        {"viewport", l_m_transform_viewport},
        {"multiply", l_m_transform_multiply},
        {"__mul", l_m_transform_multiply},
        {"release", l_m_transform_release},
        {"__gc", l_m_transform_release},
        {nullptr, nullptr}
    };
    luaL_newmetatable(L, FLN_USERTYPE_TRANSFORM);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, meths_transform, 0);
    const luaL_Reg func[] = {
        {"transform", l_transform},
        {nullptr, nullptr},
    };
    luaL_newlib(L, func);
    return 1;
}
