/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#include "data.h"

#include "error.h"
#include "memory.h"
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
	const char *data = luaL_checkstring(L, 1);
	lua_len(L, 1);
	size_t size = lua_tointeger(L, -1);

	png_image context;
	fln_image_format_t fmt;

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
	fln_image_t *image = lua_newuserdata(L, sizeof(fln_image_t));
	luaL_setmetatable(L, FLN_USERTYPE_IMAGE);
	image->width = context.width;
	image->height = context.height;
	image->format = fmt;
	image->data = img_data;
	return 1;
}

static int l_image_size(lua_State *L) {
	fln_image_t *image = luaL_checkudata(L, 1, FLN_USERTYPE_IMAGE);
	if (image->data) {
		lua_pushinteger(L, image->width);
		lua_pushinteger(L, image->height);
	}
	return 2;
}

static int l_image_release(lua_State *L) {
	fln_image_t *image = luaL_checkudata(L, 1, FLN_USERTYPE_IMAGE);
	if (image->data) {
		fln_free(image->data);
		image->data = nullptr;
	}
	return 0;
}

/*
static int l_ttf(lua_State * L)
{
	const char * data = luaL_checkstring(L, 1);
	lua_len(L, 1);
	size_t len = lua_tointeger(L, 1);
	lua_pop(L, 1);
	struct fln_font_t * font = lua_newuserdata(L, sizeof(struct fln_font_t));
	luaL_setmetatable(L, FLN_USERTYPE_FONT);
	font->raw_data = fln_alloc(sizeof(unsigned char) * len);
	memcpy(font->raw_data, data, sizeof(unsigned char) * len);
	font->raw_data_size = sizeof(unsigned char) * len;
	if (!stbtt_InitFont(&font->info, font->raw_data, 0))
	{
		return fln_error(L, "failed to initialize TTF font");
	}
	return 1;
}

static int l_font_generate(lua_State * L)
{
	struct fln_font_t * font = luaL_checkudata(L, 1, FLN_USERTYPE_FONT);
	const char text[] = {0x53, 0x54, 0x42, 0x00};//luaL_checkstring(L, 2);
	float font_size = luaL_checknumber(L, 3);
	float scale = stbtt_ScaleForPixelHeight(&font->info, font_size);
	int ascent = 0;
	int descent = 0;
	int line_gap = 0;
	stbtt_GetFontVMetrics(&font->info, &ascent, &descent, &line_gap);
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);
	int image_w = 0;
	int image_h = 0;
	// 计算图像尺寸
	int x = 0;
	for (size_t i = 0; i < strlen(text); i++)
	{
		int advance_width = 0;
		int left_side_bearing = 0;
		stbtt_GetCodepointHMetrics(&font->info, text[i], &advance_width, &left_side_bearing);
		int broder_x1, broder_y1, broder_x2, broder_y2;
		stbtt_GetCodepointBitmapBox(&font->info, text[i], scale, scale, &broder_x1, &broder_y1, &broder_x2, &broder_y2);
		int y = ascent + broder_y1;
		x += roundf(advance_width * scale);
		int kern = stbtt_GetCodepointKernAdvance(&font->info, text[i], text[i + 1]);
		x += roundf(kern * scale);
	}
	lua_pushinteger(L, x);
	return 1;
}

static int l_font_release(lua_State * L)
{
	struct fln_font_t * font = luaL_checkudata(L, 1, FLN_USERTYPE_FONT);
	if (font->raw_data)
	{
		fln_free(font->raw_data);
		font->raw_data = nullptr;
	}
	return 0;
}
*/
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
	const luaL_Reg funcs[] = { { "png", l_png }, /*{"ttf", l_ttf},*/ { nullptr, nullptr } };
	luaL_newlib(L, funcs);
	return 1;
}
