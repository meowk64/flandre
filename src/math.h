/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#pragma once

#include <lua.h>
#define FLN_USERTYPE_TRANSFORM "fln.transform"

int fln_luaopen_math(lua_State *L);
