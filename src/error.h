#pragma once

#include <lua.h>

#define fln_error(L, ...) printf("runtime error! here is the place in source\n"), luaL_error(L, __VA_ARGS__)
#define fln_warning(...) printf(__VA_ARGS__)