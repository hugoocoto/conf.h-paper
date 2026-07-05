OUT = a.out
OUT_LUA = lua_perf
CC = gcc
FLAGS = -Wall -Wextra -ggdb -llua

all: $(OUT) $(OUT_LUA)

$(OUT): main.c perf.h
	$(CC) $(FLAGS) main.c -o $(OUT)

$(OUT_LUA): main_lua.c perf.h
	$(CC) $(FLAGS) main_lua.c -o $(OUT_LUA)
