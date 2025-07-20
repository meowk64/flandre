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
	FLN_IMAGE_FORMAT_RGBA8U,
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

int fln_luaopen_decoder(lua_State *L);
