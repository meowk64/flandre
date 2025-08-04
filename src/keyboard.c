/*
	This file is part of Flandre
	Copyright (C) 2025 Teabagus

	Flandre is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.
*/
#include "keyboard.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <lauxlib.h>
#include <uthash.h>

#include "memory.h"

typedef struct key_state_entry_s {
	int key;
	bool state;
	UT_hash_handle hh;
} key_state_entry_t;

static key_state_entry_t *key_states = nullptr;

static int l_pressed(lua_State *L) {
	int key = luaL_checkinteger(L, 1);
	key_state_entry_t *entry;
	HASH_FIND_INT(key_states, &key, entry);
	lua_pushboolean(L, entry ? entry->state : false);
	return 1;
}

void fln_receive_keyboard_events(const SDL_Event *event) {
	if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
		int key = event->key.scancode;
		bool is_down = (event->type == SDL_EVENT_KEY_DOWN);

		key_state_entry_t *entry;
		HASH_FIND_INT(key_states, &key, entry);

		if (entry) {
			entry->state = is_down;
		} else {
			entry = fln_alloc(sizeof(key_state_entry_t));
			if (entry) {
				entry->key = key;
				entry->state = is_down;
				HASH_ADD_INT(key_states, key, entry);
			}
		}
	}
}

void fln_clear_key_states() {
	key_state_entry_t *entry, *tmp;
	HASH_ITER(hh, key_states, entry, tmp) {
		HASH_DEL(key_states, entry);
		fln_free(entry);
	}
}

int fln_luaopen_keyboard(lua_State *L) {
	const luaL_Reg funcs[] = {
		{ "pressed", l_pressed },
		{ nullptr, nullptr }
	};
	luaL_newlib(L, funcs);
	return 1;
}
