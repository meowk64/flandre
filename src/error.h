#pragma once

#include "log.h"
#include <lua.h>

#define fln_error(L, ...) log_error("(in source) " __VA_ARGS__), luaL_error(L, __VA_ARGS__)
