#pragma once

#include <lua.h>
#include <lauxlib.h>
#include <stb/stb_truetype.h>

#define FLN_USERTYPE_IMAGE "fln.image"
#define FLN_USERTYPE_FONT "fln.font"

struct fln_image_t
{
    int width;
    int height;
    int channels;
    unsigned char * data;
};

struct fln_font_t
{
    unsigned char * raw_data;
    size_t data_size;
    stbtt_fontinfo info;
    unsigned char * bitmap;
    size_t bitmap_width;
    size_t bitmap_height;
    float scale;
};

int fln_luaopen_decoder(lua_State * L);
