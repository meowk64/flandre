/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#include "math.h"

#include <cglm/cglm.h>
#include <lauxlib.h>
#include <lua.h>

#include "error.h"
#include "memory.h"

static int l_transform(lua_State *L) {
	mat4 **transform = lua_newuserdata(L, sizeof(mat4*));
	*transform = fln_alloc(sizeof(mat4));
	luaL_setmetatable(L, FLN_USERTYPE_TRANSFORM);
	glm_mat4_identity(**transform);
	return 1;
}

static int l_m_transform_translate(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		float x = luaL_checknumber(L, 2);
		float y = luaL_checknumber(L, 3);
		float z = luaL_checknumber(L, 4);
		glm_translate(**transform, (vec3){x, y, z});
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_rotate(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		float angle = luaL_checknumber(L, 2);
		float x = luaL_checknumber(L, 3);
		float y = luaL_checknumber(L, 4);
		float z = luaL_checknumber(L, 5);
		glm_rotate(**transform, angle, (vec3){ x, y, z });
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_rotate_at(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		float angle = luaL_checknumber(L, 2);
		float at_x = luaL_checknumber(L, 3);
		float at_y = luaL_checknumber(L, 4);
		float at_z = luaL_checknumber(L, 5);
		float x = luaL_checknumber(L, 6);
		float y = luaL_checknumber(L, 7);
		float z = luaL_checknumber(L, 8);
		glm_rotate_at(**transform, (vec3){ at_x, at_y, at_z }, angle, (vec3){ x, y, z });
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_spin(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		float angle = luaL_checknumber(L, 2);
		float x = luaL_checknumber(L, 3);
		float y = luaL_checknumber(L, 4);
		float z = luaL_checknumber(L, 5);
		glm_spin(**transform, angle, (vec3){ x, y, z });
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_scale(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		float x = luaL_checknumber(L, 2);
		float y = luaL_checknumber(L, 3);
		float z = luaL_checknumber(L, 4);
		glm_scale(**transform, (vec3){ x, y, z });
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_identity(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		glm_mat4_identity(**transform);
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_zero(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		glm_mat4_zero(**transform);
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_ortho(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		float left = luaL_checknumber(L, 2);
		float right = luaL_checknumber(L, 3);
		float bottom = luaL_checknumber(L, 4);
		float top = luaL_checknumber(L, 5);
		float near = luaL_checknumber(L, 6);
		float far = luaL_checknumber(L, 7);
		glm_ortho(left, right, bottom, top, near, far, **transform);
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_perspective(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		float fov = luaL_checknumber(L, 2);
		float aspect = luaL_checknumber(L, 3);
		float near = luaL_checknumber(L, 4);
		float far = luaL_checknumber(L, 5);
		glm_perspective(fov, aspect, near, far, **transform);
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_lookat(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		float eye_x = luaL_checknumber(L, 2);
		float eye_y = luaL_checknumber(L, 3);
		float eye_z = luaL_checknumber(L, 4);
		float center_x = luaL_checknumber(L, 5);
		float center_y = luaL_checknumber(L, 6);
		float center_z = luaL_checknumber(L, 7);
		float up_x = luaL_checknumber(L, 8);
		float up_y = luaL_checknumber(L, 9);
		float up_z = luaL_checknumber(L, 10);
		glm_lookat((vec3){ eye_x, eye_y, eye_z }, (vec3){ center_x, center_y, center_z }, (vec3){ up_x, up_y, up_z }, **transform);
	} else {
		return fln_error(L, "invalid transform");
	}
	return 0;
}

static int l_m_transform_multiply(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		mat4 **other = luaL_checkudata(L, 2, FLN_USERTYPE_TRANSFORM);
		if (other && *other) {
			glm_mat4_mul(**transform, **other, **transform);
		} else {
			return fln_error(L, "invalid transform (former)");
		}
	} else {
		return fln_error(L, "invalid transform (latter)");
	}
	return 0;
}

static int l_m_transform_release(lua_State *L) {
	mat4 **transform = luaL_checkudata(L, 1, FLN_USERTYPE_TRANSFORM);
	if (transform && *transform) {
		fln_free((void *)*transform);
		*transform = nullptr;
	}
	return 0;
}

int fln_luaopen_math(lua_State *L) {
	const luaL_Reg meths_transform[] = {
		{ "translate", l_m_transform_translate },
		{ "rotate", l_m_transform_rotate },
		{ "rotate_at", l_m_transform_rotate_at },
		{ "spin", l_m_transform_spin },
		{ "scale", l_m_transform_scale },
		{ "identity", l_m_transform_identity },
		{ "zero", l_m_transform_zero },
		{ "ortho", l_m_transform_ortho },
		{ "perspective", l_m_transform_perspective },
		{ "lookat", l_m_transform_lookat },
		{ "multiply", l_m_transform_multiply },
		{ "__mul", l_m_transform_multiply },
		{ "release", l_m_transform_release },
		{ "__gc", l_m_transform_release },
		{ nullptr, nullptr }
	};
	luaL_newmetatable(L, FLN_USERTYPE_TRANSFORM);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, meths_transform, 0);
	const luaL_Reg func[] = {
		{ "transform", l_transform },
		{ nullptr, nullptr },
	};
	luaL_newlib(L, func);
	return 1;
}
