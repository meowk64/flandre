#pragma once

#include "log.h"
#include <lua.h>

#define fln_error(L, ...) log_error("runtime error! here is the place in source"), luaL_error(L, __VA_ARGS__)
