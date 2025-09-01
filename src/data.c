/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#include "data.h"

#include "error.h"
#include "memory.h"
#include <freetype2/freetype/freetype.h>
#include <lauxlib.h>
#include <lua.h>

#include <png.h>
#include <pngconf.h>

static bool chack_png_error(png_image *context) {
	const png_uint_32 failed = PNG_IMAGE_FAILED(*context);
	if (failed & PNG_IMAGE_ERROR) {
		return true;
	}
	else {
		return false;
	}
}

static int l_png(lua_State *L) {
	const unsigned char *data = (const unsigned char *)luaL_checkstring(L, 1);
	lua_len(L, 1);
	size_t size = lua_tointeger(L, -1);

	png_image context;
	fln_image_format fmt;

	context.version = PNG_IMAGE_VERSION;
	context.opaque = nullptr;

	{
		int res = png_image_begin_read_from_memory(&context, data, size);
		if (chack_png_error(&context)) {
			return fln_error(L, "PNG error: %s", context.message);
		}
	}
	context.format &= ~(PNG_FORMAT_FLAG_BGR | PNG_FORMAT_FLAG_AFIRST | PNG_FORMAT_FLAG_LINEAR | PNG_FORMAT_FLAG_COLORMAP);
	switch (context.format) {
		case PNG_FORMAT_GRAY:
			fmt = FLN_IMAGE_FORMAT_R8;
			break;
		case PNG_FORMAT_GA:
			fmt = FLN_IMAGE_FORMAT_RG8;
			break;
		case PNG_FORMAT_RGB:
			fmt = FLN_IMAGE_FORMAT_RGB8;
			break;
		case PNG_FORMAT_RGBA:
			fmt = FLN_IMAGE_FORMAT_RGBA8;
			break;
		default:
			png_image_free(&context);
			return fln_error(L, "unsupported image format");
	}
	unsigned int stride = PNG_IMAGE_ROW_STRIDE(context);
	unsigned char *img_data = fln_alloc(PNG_IMAGE_BUFFER_SIZE(context, stride));
	if (img_data == nullptr) {
		return fln_error(L, "bad alloc");
	}
	{
		int res = png_image_finish_read(&context, nullptr, img_data, stride, nullptr);
		if (chack_png_error(&context)) {
			return fln_error(L, "PNG error: %s", context.message);
		}
	}
	fln_image *image = lua_newuserdata(L, sizeof(fln_image));
	luaL_setmetatable(L, FLN_USERTYPE_IMAGE);
	image->width = context.width;
	image->height = context.height;
	image->format = fmt;
	image->data = img_data;
	return 1;
}

static int l_image_size(lua_State *L) {
	fln_image *image = luaL_checkudata(L, 1, FLN_USERTYPE_IMAGE);
	if (image->data) {
		lua_pushinteger(L, image->width);
		lua_pushinteger(L, image->height);
	}
	return 2;
}

static int l_image_release(lua_State *L) {
	fln_image *image = luaL_checkudata(L, 1, FLN_USERTYPE_IMAGE);
	if (image->data) {
		fln_free(image->data);
		image->data = nullptr;
	}
	return 0;
}

static int l_font(lua_State *L) {
	const unsigned char *data = (const unsigned char *)luaL_checkstring(L, 1);
	lua_len(L, 1);
	size_t size = lua_tointeger(L, -1);
	FT_Library context;
	FT_Face face;
	FT_Error err;
	err = FT_Init_FreeType(&context);
	if (err != FT_Err_Ok) {
		return luaL_error(L, "failed to initialize FreeType Library: %s", FT_Error_String(err));
	}
	err = FT_New_Memory_Face(context, data, size, 0, &face);
	if (err != FT_Err_Ok) {
		return luaL_error(L, "failed to load FreeType Face in memory: %s", FT_Error_String(err));
	}
	fln_font font;
	
	return 0;
}

static int l_font_release(lua_State *L) {
	return 0;
}

int fln_luaopen_data(lua_State *L) {
	const luaL_Reg image_meths[] = {
		{ "size", l_image_size },
		{ "release", l_image_release },
		{ "__gc", l_image_release },
		{ nullptr, nullptr }
	};
	luaL_newmetatable(L, FLN_USERTYPE_IMAGE);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, image_meths, 0);
	/*
		const luaL_Reg font_meths[] = {
			{"generate", l_font_generate},
			{"release", l_font_release},
			{"__gc", l_font_release},
			{nullptr, nullptr}};
		luaL_newmetatable(L, FLN_USERTYPE_FONT);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, font_meths, 0);
	*/
	const luaL_Reg funcs[] = { { "png", l_png }, /*{"ttf", ltf},*/ { nullptr, nullptr } };
	luaL_newlib(L, funcs);
	return 1;
}
