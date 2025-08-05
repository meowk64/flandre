/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#pragma once

#include <lua.h>

#define fln_error(L, ...) printf("runtime error!\n(in source) %s:%d\n", __FILE__, __LINE__), luaL_error(L, __VA_ARGS__)
#define fln_warning(...) printf(__VA_ARGS__)