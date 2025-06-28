#include "decoder.h"

#include <lauxlib.h>
#include <lua.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

/*
// WARN: 内部分配内存
// TODO: 这个函数不应该调用 luaL_error 打断控制流
static int read_file(lua_State * L, FILE * fp, size_t * size, void ** data)
{
    if (!fp)
    {
        return luaL_error(L, "invalid FILE* object");
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    void * file_data = fln_allocate(file_size);
    if (!file_data)
    {
        return luaL_error(L, "failed to allocate memory for file data");
    }
    if (fread(file_data, 1, file_size, fp) != file_size)
    {
        fln_free(file_data);
        return luaL_error(L, "failed to read file data");
    }
    *data = file_data;
    return 0;
}
*/

static int l_png(lua_State * L)
{
    const char * data = luaL_checkstring(L, 1);
    lua_len(L, 1);
    size_t size = lua_tointeger(L, -1);

    int width, height, channels;
    unsigned char * img_data = stbi_load_from_memory((const unsigned char *)data, size, &width, &height, &channels, STBI_rgb_alpha);
    //                                                      ↑ 十分神秘的操作 ↑

    if (!img_data)
    {
        return luaL_error(L, "failed to load PNG image: %s", stbi_failure_reason());
    }
    struct fln_image_t * image = lua_newuserdata(L, sizeof(struct fln_image_t));
    luaL_setmetatable(L, FLN_USERTYPE_IMAGE);
    image->width = width;
    image->height = height;
    image->channels = 4;
    image->data = img_data;

    return 1;
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

static int l_ttf(lua_State * L)
{
    const char * data = luaL_checkstring(L, 1);
    struct fln_font_t font;

    return 1;
}

static int l_ttf_release(lua_State * L)
{
    struct fln_font_t * font = luaL_checkudata(L, 1, FLN_USERTYPE_FONT);

    return 0;
}

int fln_luaopen_decoder(lua_State * L)
{
    stbi_set_flip_vertically_on_load(1);
    const luaL_Reg image_meths[] = {
        {"release", l_image_release},
        {"__gc", l_image_release},
        {nullptr, nullptr}};
    luaL_newmetatable(L, "fln.image");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, image_meths, 0);

    const luaL_Reg funcs[] = {{"png", l_png}, {nullptr, nullptr}};
    luaL_newlib(L, funcs);
    return 1;
}