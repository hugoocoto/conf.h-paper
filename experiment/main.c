#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCLUDE_CONF_IMPLEMENTATION
#include "conf.h"

#include "perf.h"

#define TEST(_x_, ...)                                                        \
	_PERF_INIT("Get Int: " _x_);                                          \
	if (Conf_get_int(conf, &val, _x_, ##__VA_ARGS__)) {                   \
		fprintf(stderr, "Error: Can not read field " _x_ "\n", ##__VA_ARGS__); \
	}                                                                     \
	_PERF_FINI();

int
lua_nested_test(void *ctx)
{
	_PERF_START();
	Conf conf;
	int val;

	_PERF_INIT("Open");
	if (Conf_open(&conf, "test/nested.lua")) {
		errx(1, "Can not read %s\n", "test/nested.lua");
	}
	_PERF_FINI();

	/* warm-up round */
	_perf_mute = 1;
	for (int j = 0; j < 1000; j++) {
		if (Conf_get_int(conf, &val, "Number"))
			fprintf(stderr, "Error: Can not read field Number\n");
		if (Conf_get_int(conf, &val, "Table.number"))
			fprintf(stderr, "Error: Can not read field Table.number\n");
		if (Conf_get_int(conf, &val, "Table.table.number"))
			fprintf(stderr, "Error: Can not read field Table.table.number\n");
		if (Conf_get_int(conf, &val, "Table.table.table.number"))
			fprintf(stderr, "Error: Can not read field Table.table.table.number\n");
		if (Conf_get_int(conf, &val, "Table.table.table.table.number"))
			fprintf(stderr, "Error: Can not read field Table.table.table.table.number\n");
		if (Conf_get_int(conf, &val, "Table.table.table.table.table.number"))
			fprintf(stderr, "Error: Can not read field Table.table.table.table.table.number\n");
		if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.number"))
			fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.number\n");
		if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.table.number"))
			fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.table.number\n");
		if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.table.table.number"))
			fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.table.table.number\n");
		if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.table.table.table.number"))
			fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.table.table.table.number\n");
	}

	/* measurement round: 1000 iterations per depth */
	_perf_mute = 0;
	_perf_iters = 1000;
	{
		_PERF_INIT("Get Int: Number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Number"))
				fprintf(stderr, "Error: Can not read field Number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.number"))
				fprintf(stderr, "Error: Can not read field Table.number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.table.number"))
				fprintf(stderr, "Error: Can not read field Table.table.number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.table.table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.table.table.number"))
				fprintf(stderr, "Error: Can not read field Table.table.table.number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.table.table.table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.table.table.table.number"))
				fprintf(stderr, "Error: Can not read field Table.table.table.table.number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.table.table.table.table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.table.table.table.table.number"))
				fprintf(stderr, "Error: Can not read field Table.table.table.table.table.number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.table.table.table.table.table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.number"))
				fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.table.table.table.table.table.table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.table.number"))
				fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.table.number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.table.table.table.table.table.table.table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.table.table.number"))
				fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.table.table.number\n");
		_PERF_FINI();
	}
	{
		_PERF_INIT("Get Int: Table.table.table.table.table.table.table.table.table.number");
		for (int j = 0; j < 1000; j++)
			if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.table.table.table.number"))
				fprintf(stderr, "Error: Can not read field Table.table.table.table.table.table.table.table.table.number\n");
		_PERF_FINI();
	}
	_perf_iters = 1;

	_PERF_INIT("Close");
	Conf_close(conf);
	_PERF_FINI();

	fflush(stdout);
	return 0;
	(void) ctx;
}

int
lua_loop_test(void *ctx)
{
	_PERF_START();
	Conf conf;
	int val;

	_PERF_INIT("Open");
	if (Conf_open(&conf, "test/for.lua")) {
		errx(1, "Can not read %s\n", "test/for.lua");
	}
	_PERF_FINI();

	_perf_mute = 1;  /* warm-up round */
	for (int i = 0; i < 1000; i++) {
		TEST("Table.table.table.table.table.table.table.table.table.%d.number", i);
	}

	/* measurement round: 1000 iterations batched */
	_perf_mute = 0;
	_perf_iters = 1000;
	{
		_PERF_INIT("Get Int: Table.table.table.table.table.table.table.table.table.%%d.number");
		for (int i = 0; i < 1000; i++)
			if (Conf_get_int(conf, &val, "Table.table.table.table.table.table.table.table.table.%d.number", i))
				fprintf(stderr, "Error: Can not read field Table...\n");
		_PERF_FINI();
	}
	_perf_iters = 1;

	_PERF_INIT("Close");
	Conf_close(conf);
	_PERF_FINI();

	fflush(stdout);
	return 0;
	(void) ctx;
}

int
main(int argc, char *argv[])
{
	_perf_trial = argc > 1 ? atoi(argv[1]) : 0;
	_perf_lib   = "confh";
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
