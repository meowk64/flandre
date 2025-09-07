/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#pragma once

extern "C" {
#include <lauxlib.h>
#include <lua.h>
}

#include <stddef.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define FLN_USERTYPE_ARCHIVE "fln.archive" // TODO
#define FLN_USERTYPE_IMAGE "fln.image"
#define FLN_USERTYPE_MODEL "fln.model" // TODO
#define FLN_USERTYPE_FONT "fln.font"

enum fln_image_format {
	FLN_IMAGE_FORMAT_R8,
	FLN_IMAGE_FORMAT_RG8,
	FLN_IMAGE_FORMAT_RGB8,
	FLN_IMAGE_FORMAT_RGBA8,
} ;

struct fln_image {
	int width;
	int height;
	fln_image_format format;
	unsigned char *data;
};

struct fln_font {
	FT_Library context;
	FT_Face face;
};

int fln_luaopen_data(lua_State *L);
