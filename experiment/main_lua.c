#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "perf.h"

static lua_State *
lua_config_open(const char *filename)
{
	lua_State *L = luaL_newstate();
	if (!L) return NULL;
	luaL_openlibs(L);
	if (luaL_dofile(L, filename) != LUA_OK) {
		lua_close(L);
		return NULL;
	}
	if (lua_gettop(L) >= 1 && lua_istable(L, -1)) {
		lua_pushnil(L);
		while (lua_next(L, -2) != 0) {
			if (lua_type(L, -2) == LUA_TSTRING) {
				lua_setglobal(L, lua_tostring(L, -2));
			} else {
				lua_pop(L, 1);
			}
		}
	}
	if (lua_gettop(L) >= 1)
		lua_pop(L, 1);
	return L;
}

int
lua_nested_test(void *ctx)
{
	_PERF_START();
	lua_State *L;
	int val;

	_PERF_INIT("Open");
	L = lua_config_open("test/nested.lua");
	if (!L) errx(1, "Can not read %s\n", "test/nested.lua");
	_PERF_FINI();

	int r_num, r_tbl[9];

	lua_getglobal(L, "Number");
	r_num = luaL_ref(L, LUA_REGISTRYINDEX);

	lua_getglobal(L, "Table");
	r_tbl[0] = luaL_ref(L, LUA_REGISTRYINDEX);

	for (int i = 1; i < 9; i++) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, r_tbl[i-1]);
		lua_pushstring(L, "table");
		lua_rawget(L, -2);
		r_tbl[i] = luaL_ref(L, LUA_REGISTRYINDEX);
		lua_pop(L, 1);
	}

	/* build path names for each depth */
	char depth_names[9][128];
	for (int i = 0; i < 9; i++) {
		int pos = snprintf(depth_names[i], sizeof(depth_names[i]), "Get Int: Table");
		for (int j = 0; j < i; j++)
			pos += snprintf(depth_names[i] + pos, sizeof(depth_names[i]) - pos, ".table");
		pos += snprintf(depth_names[i] + pos, sizeof(depth_names[i]) - pos, ".number");
	}

	/* warm-up round: 1000 iterations each */
	_perf_mute = 1;
	for (int j = 0; j < 1000; j++) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, r_num);
		val = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		for (int i = 0; i < 9; i++) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, r_tbl[i]);
			lua_pushstring(L, "number");
			lua_rawget(L, -2);
			val = (int)lua_tointeger(L, -1);
			lua_pop(L, 2);
		}
	}

	/* measurement round: 1000 iterations per depth */
	_perf_mute = 0;
	_perf_iters = 1000;
	{
		_PERF_INIT("Get Int: Number");
		for (int j = 0; j < 1000; j++) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, r_num);
			val = (int)lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		_PERF_FINI();
	}
	for (int i = 0; i < 9; i++) {
		_PERF_INIT(depth_names[i]);
		for (int j = 0; j < 1000; j++) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, r_tbl[i]);
			lua_pushstring(L, "number");
			lua_rawget(L, -2);
			val = (int)lua_tointeger(L, -1);
			lua_pop(L, 2);
		}
		_PERF_FINI();
	}
	_perf_iters = 1;

	_PERF_INIT("Close");
	lua_close(L);
	_PERF_FINI();

	fflush(stdout);
	return 0;
	(void) ctx;
}

int
lua_loop_test(void *ctx)
{
	_PERF_START();
	lua_State *L;
	int val;

	_PERF_INIT("Open");
	L = lua_config_open("test/for.lua");
	if (!L) errx(1, "Can not read %s\n", "test/for.lua");
	_PERF_FINI();

	lua_getglobal(L, "Table");
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	lua_pushstring(L, "table");
	lua_rawget(L, -2);
	int r_deep = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_pop(L, 1);

	_perf_mute = 1;  /* warm-up round */
	for (int i = 0; i < 1000; i++) {
		_PERF_INIT("Get Int: Table.table.table.table.table.table.table.table.table.%%d.number");
		lua_rawgeti(L, LUA_REGISTRYINDEX, r_deep);
		lua_rawgeti(L, -1, i);
		if (lua_isnil(L, -1)) {
			fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.table.table.table.%d.number\n", i);
			lua_pop(L, 1);
		} else {
			lua_pushstring(L, "number");
			lua_rawget(L, -2);
			val = (int)lua_tointeger(L, -1);
			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		_PERF_FINI();
	}

	/* measurement round: 1000 iterations batched */
	_perf_mute = 0;
	_perf_iters = 1000;
	{
		_PERF_INIT("Get Int: Table.table.table.table.table.table.table.table.table.%%d.number");
		for (int i = 0; i < 1000; i++) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, r_deep);
			lua_rawgeti(L, -1, i);
			if (lua_isnil(L, -1)) {
				fprintf(stderr, "Error: Can not read field Table...%d.number\n", i);
				lua_pop(L, 1);
			} else {
				lua_pushstring(L, "number");
				lua_rawget(L, -2);
				val = (int)lua_tointeger(L, -1);
				lua_pop(L, 2);
			}
			lua_pop(L, 1);
		}
		_PERF_FINI();
	}
	_perf_iters = 1;

	_PERF_INIT("Close");
	lua_close(L);
	_PERF_FINI();

	fflush(stdout);
	return 0;
	(void) ctx;
}

int
main(int argc, char *argv[])
{
	_perf_trial = argc > 1 ? atoi(argv[1]) : 0;
	_perf_lib   = "lua";
	const char *filter = argc > 2 ? argv[2] : "all";

	if (!strcmp(filter, "all") || !strcmp(filter, "nesting")) {
		_perf_test_group = "nesting";
		func_perf("parse nested lua tables", lua_nested_test, NULL);
	}
	if (!strcmp(filter, "all") || !strcmp(filter, "loop")) {
		_perf_test_group = "loop";
		func_perf("parse multiple values of the same table", lua_loop_test, NULL);
	}

	return 0;
}
