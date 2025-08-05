/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#pragma once

#include <lauxlib.h>
#include <lua.h>
#include <stb/stb_truetype.h>
#include <stddef.h>

#define FLN_USERTYPE_ARCHIVE "fln.archive" // TODO
#define FLN_USERTYPE_IMAGE "fln.image"
#define FLN_USERTYPE_MODEL "fln.model" // TODO
#define FLN_USERTYPE_FONT "fln.font"

typedef enum fln_image_format_e {
	FLN_IMAGE_FORMAT_R8,
	FLN_IMAGE_FORMAT_RG8,
	FLN_IMAGE_FORMAT_RGB8,
	FLN_IMAGE_FORMAT_RGBA8,
} fln_image_format_t;

typedef struct fln_image_s {
	int width;
	int height;
	fln_image_format_t format;
	unsigned char *data;
} fln_image_t;

typedef struct fln_font_s {
	stbtt_fontinfo info;
	unsigned char *raw_data;
	size_t raw_data_size;
} fln_font_t;

int fln_luaopen_data(lua_State *L);
