#include "decoder.h"

#include <lauxlib.h>
#include <lua.h>
#include "error.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

static int l_png(lua_State * L)
{
    const char * data = luaL_checkstring(L, 1);
    lua_len(L, 1);
    size_t size = lua_tointeger(L, -1);

    int width, height, channels;
    unsigned char * img_data = stbi_load_from_memory((const unsigned char *)data, size, &width, &height, &channels, STBI_rgb_alpha);

    if (!img_data)
    {
        return fln_error(L, "failed to load PNG image: %s", stbi_failure_reason());
    }
    struct fln_image_t * image = lua_newuserdata(L, sizeof(struct fln_image_t));
    luaL_setmetatable(L, FLN_USERTYPE_IMAGE);
    image->width = width;
    image->height = height;
    image->channels = 4;
    image->data = img_data;

    return 1;
}

static int l_image_size(lua_State * L)
{
    struct fln_image_t * image = luaL_checkudata(L, 1, FLN_USERTYPE_IMAGE);
    if (image->data)
    {
        lua_pushinteger(L, image->width);
        lua_pushinteger(L, image->height);
    }
    return 2;
}

static int l_image_release(lua_State * L)
{
    struct fln_image_t * image = luaL_checkudata(L, 1, FLN_USERTYPE_IMAGE);
    if (image->data)
    {
        stbi_image_free(image->data);
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
        log_info("www");
        int advance_width = 0;
        int left_side_bearing = 0;
        stbtt_GetCodepointHMetrics(&font->info, text[i], &advance_width, &left_side_bearing);
        log_info("www");
        int broder_x1, broder_y1, broder_x2, broder_y2;
        stbtt_GetCodepointBitmapBox(&font->info, text[i], scale, scale, &broder_x1, &broder_y1, &broder_x2, &broder_y2);
        int y = ascent + broder_y1;
        log_info("www");
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
int fln_luaopen_decoder(lua_State * L)
{
    stbi_set_flip_vertically_on_load(1);
    const luaL_Reg image_meths[] = {
        {"size", l_image_size},
        {"release", l_image_release},
        {"__gc", l_image_release},
        {nullptr, nullptr}};
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
    const luaL_Reg funcs[] = {{"png", l_png}, /*{"ttf", l_ttf},*/ {nullptr, nullptr}};
    luaL_newlib(L, funcs);
    return 1;
}
