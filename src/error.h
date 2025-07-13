#pragma once

#include <lua.h>
#include "log.h"

#define fln_error(L, ...) log_error("(in source) " __VA_ARGS__), luaL_error(L, __VA_ARGS__)
