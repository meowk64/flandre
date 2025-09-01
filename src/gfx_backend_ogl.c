/*
	This file is part of Flandre
	Copyright (c) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.  See `LICENSE` for more details
*/
#include "gfx_backend_ogl.h"

#include <SDL3/SDL.h>
#include <cglm/struct.h>
#include <lauxlib.h>
#include <lua.h>
#include <uthash.h>

#include "data.h"
#include "error.h"
#include "gfx_interface.h"
#include "math.h"
#include "memory.h"
#include "opengl/glad.h"

// OpenGL 的 Uniform 缓存
typedef struct gfx_uniform_cache_entry {
	char name[64];
	GLuint location;
	UT_hash_handle hh;
} gfx_uniform_cache_entry;

// OpenGL 的 Pipeline 实现
typedef struct gfx_pipeline {
	GLuint shader_program;
	gfx_uniform_cache_entry *uniform_cache;
} gfx_pipeline;

// OpenGL 的 Mesh 实现
typedef struct gfx_mesh {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	unsigned int vertices_count;
} gfx_mesh;

// OpenGL 的 Texture 实现
typedef struct gfx_texture2d {
	GLuint id;
	int width;
	int height;
} gfx_texture2d;

// tools ---------------------------------------------------------------------

// 获取着色器日志（错误日志）
static char *get_shader_log(GLuint sh) {
	GLint len = 0;
	GLint len_written = 0;
	glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		char *log = fln_alloc(len);
		glGetShaderInfoLog(sh, len, &len_written, log);
		return log;
	}
	return nullptr;
}

// 获取着色器状态
static int shader_status(GLuint sh) {
	GLint status;
	glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
	return status;
}

// 获取着色器程序日志（错误日志）
static char *get_program_log(GLuint prog) {
	GLint len = 0;
	GLint len_written = 0;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		char *log = fln_alloc(len);
		glGetProgramInfoLog(prog, len, &len_written, log);
		return log;
	}
	return nullptr;
}

// 获取程序状态
static int program_status(GLuint prog) {
	GLint status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	return status;
}

// 编译着色器
// 出现错误时不会调用 lua_error，防止内存泄漏
static int compile_shader(lua_State *L, GLuint shader, const char *src) {
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	if (!shader_status(shader)) {
		char *log = get_shader_log(shader);
		if (log) {
			lua_pushstring(L, log);
			fln_free(log);
			return 0;
		}
	}
	return 1;
}

// 获取 Uniform 位置（带缓存）
static GLuint get_uniform_location_cache(gfx_pipeline *pl, const char *name) {
	gfx_uniform_cache_entry *entry = nullptr;
	HASH_FIND_STR(pl->uniform_cache, name, entry);
	if (entry) {
		return entry->location;
	} else {
		GLuint location = glGetUniformLocation(pl->shader_program, name);
		if (location != -1) {
			entry = (gfx_uniform_cache_entry *)fln_alloc(sizeof(gfx_uniform_cache_entry));
			if (!entry) {
				return -2; // 内存分配失败
			}
			strncpy(entry->name, name, sizeof(entry->name) - 1);
			entry->name[sizeof(entry->name) - 1] = '\0';
			entry->location = location;
			HASH_ADD_STR(pl->uniform_cache, name, entry);
		}
		return location;
	}
}

// 清理 Uniform 缓存
static void clear_uniform_cache(gfx_pipeline *pl) {
	gfx_uniform_cache_entry *entry, *tmp;
	HASH_ITER(hh, pl->uniform_cache, entry, tmp) {
		HASH_DEL(pl->uniform_cache, entry);
		fln_free(entry);
	}
}

// tools (end) ---------------------------------------------------------------------

static int l_pipeline(lua_State *L) {
	/*
	{
		shaders = {
			vertex = `string`,
			fragment = `string`
		},
		vsync = `bool`,  (TODO)
		depth = `bool`,  (TODO)
		cull = `bool`,  (TODO)
		blend = `bool`  (TODO)

	}
	*/

	lua_settop(L, 1);
	luaL_checktype(L, 1, LUA_TTABLE);

	gfx_pipeline *pl = lua_newuserdata(L, sizeof(gfx_pipeline));
	luaL_setmetatable(L, FLN_USERTYPE_PIPELINE);

	// shaders --------------------------------------------------------

	pl->shader_program = 0;

	const char *vsh_src;
	const char *fsh_src;

	lua_getfield(L, 1, "shaders");
	if (lua_type(L, -1) != LUA_TTABLE) {
		return fln_error(L, "`shaders` not found, or invalid type");
	}

	// vertex
	lua_getfield(L, -1, "vertex");
	vsh_src = luaL_checkstring(L, -1);
	GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
	if (!compile_shader(L, vsh, vsh_src)) {
		glDeleteShader(vsh);
		return lua_error(L);
	}

	lua_pop(L, 1);

	// fragment
	lua_getfield(L, -1, "fragment");
	fsh_src = luaL_checkstring(L, -1);
	GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);
	if (!compile_shader(L, fsh, fsh_src)) {
		glDeleteShader(vsh);
		glDeleteShader(fsh);
		return lua_error(L);
	}

	lua_pop(L, 2);

	// program
	GLuint program = glCreateProgram();
	glAttachShader(program, vsh);
	glAttachShader(program, fsh);
	glLinkProgram(program);
	if (!program_status(program)) {
		char *log = get_program_log(program);
		if (log) {
			lua_pushstring(L, log);
			fln_free(log);
			glDeleteShader(vsh);
			glDeleteShader(fsh);
			glDeleteProgram(program);
			return lua_error(L);
		}
	}
	glDeleteShader(vsh);
	glDeleteShader(fsh);
	pl->shader_program = program;
	pl->uniform_cache = nullptr; // 一定不要忘了
	lua_settop(L, 2);
	return 1;
}

static GLuint current_shader_program = 0;
static GLuint current_vao = 0;
static int texture_unit_count = 0; // 用于记录纹理单元，以支持自动传入多个纹理

static int l_m_pipeline_submit(lua_State *L) {
	gfx_pipeline *pl = luaL_checkudata(L, 1, FLN_USERTYPE_PIPELINE);
	if (pl->shader_program == 0) {
		return fln_error(L, "invalid pipeline");
	}

	if (current_shader_program != pl->shader_program) {
		glUseProgram(pl->shader_program);
		current_shader_program = pl->shader_program;
	}

	gfx_mesh *mesh = luaL_checkudata(L, 2, FLN_USERTYPE_MESH);
	if (mesh->vertices_count == 0 || mesh->ebo == 0 || mesh->vao == 0 || mesh->vbo == 0) {
		return fln_error(L, "invalid mesh");
	}

	if (mesh->vao != current_vao) {
		glBindVertexArray(mesh->vao);
		current_vao = mesh->vao;
	}

	glDrawElements(GL_TRIANGLES, mesh->vertices_count, GL_UNSIGNED_INT, nullptr);

	texture_unit_count = 0;

	return 0;
}

static int l_m_pipeline_submit_instanced(lua_State *L) {
	gfx_pipeline *pl = luaL_checkudata(L, 1, FLN_USERTYPE_PIPELINE);
	lua_Integer num = luaL_checkinteger(L, 2);
	if (pl->shader_program == 0) {
		return fln_error(L, "invalid pipeline");
	}

	if (current_shader_program != pl->shader_program) {
		glUseProgram(pl->shader_program);
		current_shader_program = pl->shader_program;
	}

	gfx_mesh *mesh = luaL_checkudata(L, 2, FLN_USERTYPE_MESH);
	if (mesh->vertices_count == 0 || mesh->ebo == 0 || mesh->vao == 0 || mesh->vbo == 0) {
		return fln_error(L, "invalid mesh");
	}

	if (mesh->vao != current_vao) {
		glBindVertexArray(mesh->vao);
		current_vao = mesh->vao;
	}

	glDrawElementsInstanced(GL_TRIANGLES, mesh->vertices_count, GL_UNSIGNED_INT, nullptr, num);

	texture_unit_count = 0;

	return 0;
}

static int l_m_pipeline_release(lua_State *L) {
	gfx_pipeline *pl = luaL_checkudata(L, 1, FLN_USERTYPE_PIPELINE);
	if (pl->shader_program == 0) {
		return 0;
	}
	// 缓存处理
	if (current_shader_program == pl->shader_program) {
		current_shader_program = 0;
	}
	if (current_vao == pl->shader_program) {
		current_vao = 0;
	}
	glDeleteProgram(pl->shader_program);
	pl->shader_program = 0;
	// 清理 Uniform 缓存
	clear_uniform_cache(pl);
	return 0;
}

static int l_m_pipeline_uniform(lua_State *L) {
	gfx_pipeline *pl = luaL_checkudata(L, 1, FLN_USERTYPE_PIPELINE);
	if (pl->shader_program == 0) {
		return fln_error(L, "invalid pipeline");
	}
	glUseProgram(pl->shader_program);
	const char *name = luaL_checkstring(L, 2);
	GLuint location = get_uniform_location_cache(pl, name);
	if (location == -1) {
		return fln_error(L, "uniform '%s' not found", name);
	}
	else if (location == -2) {
		return fln_error(L, "failed to allocate memory for uniform cache entry");
	}

	int size = lua_gettop(L) - 2; // 除去 self 和 uniform 名称，之后的参数都是要传入 uniform 的
	if (size == 1 && lua_type(L, 3) == LUA_TUSERDATA) {
		void *texture2d_test = luaL_testudata(L, 3, FLN_USERTYPE_TEXTURE2D);
		void *transform_test = luaL_testudata(L, 3, FLN_USERTYPE_TRANSFORM);
		if (transform_test) {
			mat4 **transform = (mat4 **)transform_test;

			if (transform && *transform) {
				// glms_mat4_print(*transform, stdout);
				glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat *)*transform);
			} else {
				return fln_error(L, "invalid transform");
			}
		} else if (texture2d_test) {
			if (texture_unit_count > 15) {
				return fln_error(L, "the number of texture units has reached the maximum limit (%d)", texture_unit_count);
			}

			if (pl->shader_program == 0) {
				return fln_error(L, "invalid pipeline");
			}

			if (current_shader_program != pl->shader_program) {
				glUseProgram(pl->shader_program);
			}
			gfx_texture2d *texture = (gfx_texture2d *)texture2d_test;

			GLuint location = get_uniform_location_cache(pl, name);
			if (location == -1) {
				return fln_error(L, "uniform '%s' not found", name);
			}
			else if (location == -2) {
				return fln_error(L, "failed to allocate memory for uniform cache entry");
			}

			glActiveTexture(GL_TEXTURE0 + texture_unit_count);
			glBindTexture(GL_TEXTURE_2D, texture->id);
			glUniform1i(location, texture_unit_count);
			texture_unit_count++;
		} else {
			return fln_error(L, "invalid userdata");
		}
	} else if (size == 1 && lua_type(L, 3) == LUA_TNUMBER) {
		glUniform1f(location, luaL_checknumber(L, 3));
	}
	else if (size == 2 && lua_type(L, 3) == LUA_TNUMBER && lua_type(L, 4) == LUA_TNUMBER) {
		glUniform2f(location, luaL_checknumber(L, 3), luaL_checknumber(L, 4));
	} else if (size == 3 && lua_type(L, 3) == LUA_TNUMBER && lua_type(L, 4) == LUA_TNUMBER && lua_type(L, 5) == LUA_TNUMBER) {
		glUniform3f(location, luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5));
	} else if (size == 4 && lua_type(L, 3) == LUA_TNUMBER && lua_type(L, 4) == LUA_TNUMBER && lua_type(L, 5) == LUA_TNUMBER && lua_type(L, 6) == LUA_TNUMBER) {
		glUniform4f(location, luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6));
	} else {
		return fln_error(L, "unsupported uniform arguments (invalid size or type)");
	}
	return 0;
}

// 可能只会用在创建四边形三角形上（
// data 能加载模型的说
static int l_mesh(lua_State *L) {
	lua_settop(L, 3);
	const float *vertices = (const float *)luaL_checkstring(L, 1);
	const size_t vertices_size = luaL_len(L, 1);
	// const size_t vertices_count = vertices_size / sizeof(float);

	const unsigned int *indices = (const unsigned int *)luaL_checkstring(L, 2);
	const size_t indices_size = luaL_len(L, 2);
	const size_t indices_count = indices_size / sizeof(unsigned int);

	const unsigned int *attributes = (const unsigned int *)luaL_checkstring(L, 3);
	const size_t attributes_size = luaL_len(L, 3);
	const size_t attributes_count = attributes_size / sizeof(unsigned int);

	const unsigned int *divisors = (const unsigned int *)luaL_optstring(L, 4, nullptr);
	size_t divisors_size = 0;
	size_t divisors_count = 0;
	if (divisors) {
		divisors_size = luaL_len(L, 4);
		divisors_count = divisors_size / sizeof(unsigned int);
		if (attributes_count != divisors_count) {
			return fln_error(L, "attributes count must equal to divisors count");
		}
	}

	GLuint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

	size_t stride = 0;
	for (size_t i = 0; i < attributes_count; i++) {
		stride += attributes[i] * sizeof(float);
	}

	size_t offset = 0;
	for (size_t i = 0; i < attributes_count; i++) {
		glVertexAttribPointer(i, attributes[i], GL_FLOAT, GL_FALSE, stride, (void *)(uintptr_t)offset);
		glEnableVertexAttribArray(i);
		if (divisors) {
			glVertexAttribDivisor(i, divisors[i]);
		}
		offset += attributes[i] * sizeof(float);
	}

	gfx_mesh *mesh = lua_newuserdata(L, sizeof(gfx_mesh));
	luaL_setmetatable(L, FLN_USERTYPE_MESH);
	mesh->vao = vao;
	mesh->vbo = vbo;
	mesh->ebo = ebo;
	mesh->vertices_count = indices_count;
	glBindVertexArray(0);
	return 1;
}

static int l_m_mesh_release(lua_State *L) {
	gfx_mesh *mesh = luaL_checkudata(L, -1, FLN_USERTYPE_MESH);
	if (mesh->vertices_count == 0 || mesh->ebo == 0 || mesh->vao == 0 || mesh->vbo == 0) {
		return 0;
	}
	glDeleteBuffers(1, &mesh->vbo);
	mesh->vbo = 0;
	glDeleteBuffers(1, &mesh->ebo);
	mesh->ebo = 0;
	glDeleteVertexArrays(1, &mesh->vao);
	mesh->vao = 0;
	mesh->vertices_count = 0;
	return 0;
}

static int l_texture2d(lua_State *L) {
	fln_image *image = luaL_checkudata(L, 1, FLN_USERTYPE_IMAGE);
	if (!image->data) {
		return fln_error(L, "invalid image data");
	}
	GLint format;
	if (image->format == FLN_IMAGE_FORMAT_RGBA8) {
		format = GL_RGBA;
	}
	else if (image->format == FLN_IMAGE_FORMAT_RGB8) {
		format = GL_RGB;
	}
	else {
		return fln_error(L, "invalid image format: %d", image->format);
	}
	if (image->width <= 0 || image->height <= 0) {
		return fln_error(L, "invalid image size: %dx%d", image->width, image->height);
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// 以后也能自定义参数？
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (image->width % 4 != 0) {
		if (image->width % 2 != 0) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		} else {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		}
	} else {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->data);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, 0);

	gfx_texture2d *texture_data = lua_newuserdata(L, sizeof(gfx_texture2d));
	luaL_setmetatable(L, FLN_USERTYPE_TEXTURE2D);
	texture_data->id = texture;
	texture_data->width = image->width;
	texture_data->height = image->height;

	return 1;
}

static int l_texture2d_size(lua_State *L) {
	gfx_texture2d *texture = luaL_checkudata(L, 1, FLN_USERTYPE_TEXTURE2D);
	lua_pushinteger(L, texture->width);
	lua_pushinteger(L, texture->height);
	return 2;
}

static int l_texture2d_release(lua_State *L) {
	gfx_texture2d *texture = luaL_checkudata(L, 1, FLN_USERTYPE_TEXTURE2D);
	if (texture->id) {
		glDeleteTextures(1, &texture->id);
		texture->id = 0;
	}
	return 0;
}

static SDL_WindowFlags sdl_configure(fln_app_state *appstate) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	return SDL_WINDOW_OPENGL;
}

static bool init(fln_app_state *appstate) {
	appstate->ogl_context = SDL_GL_CreateContext(appstate->window);
	if (!appstate->ogl_context) {
		printf("failed to call SDL_GL_CreateContext()\n");
		return false;
	}
	if (!SDL_GL_MakeCurrent(appstate->window, appstate->ogl_context)) {
		printf("failed to call SDL_GL_MakeCurrent()\n");
		return false;
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		printf("failed to call gladLoadGLLoader()\n");
		return false;
	}
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	return true;
}

static bool begin_drawing(fln_app_state *appstate) {
	glClear(GL_COLOR_BUFFER_BIT);
	return true;
}

static bool end_drawing(fln_app_state *appstate) {
	SDL_GL_SwapWindow(appstate->window);
	int err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("OpenGL error: %d\n", err);
		return false;
	}
	return true;
}

static bool destroy_resource(fln_app_state *appstate) {
	if (!SDL_GL_DestroyContext(appstate->ogl_context)) {
		printf("failed to call SDL_GL_DestroyContext()\n");
	}
	return true;
}

static void receive_window_events(fln_app_state *appstate, const SDL_Event *event) {
	if (event->type == SDL_EVENT_WINDOW_RESIZED) {
		int w, h;
		SDL_GetWindowSize(appstate->window, &w, &h);
		glViewport(0, 0, w, h); // TODO: 不能直接更改 viewport，因为 SDL 的事件接收回调可能会在别的线程被调用
	}
}

fln_gfx_backend fln_gfx_init_backend_ogl() {
	fln_gfx_backend backend;
	backend.sdl_configure = sdl_configure;
	backend.init = init;
	backend.begin_drawing = begin_drawing;
	backend.end_drawing = end_drawing;
	backend.destroy_resource = destroy_resource;
	backend.receive_window_events = receive_window_events;
	backend.l_pipeline = l_pipeline;
	backend.l_pipeline_release = l_m_pipeline_release;
	backend.l_pipeline_uniform = l_m_pipeline_uniform;
	backend.l_pipeline_submit = l_m_pipeline_submit;
	backend.l_pipeline_submit_instanced = l_m_pipeline_submit_instanced;
	backend.l_mesh = l_mesh;
	backend.l_mesh_release = l_m_mesh_release;
	backend.l_texture2d = l_texture2d;
	backend.l_texture2d_size = l_texture2d_size;
	backend.l_texture2d_release = l_texture2d_release;
	return backend;
}
