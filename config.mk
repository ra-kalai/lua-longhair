CC := gcc
CFLAGS := -Wall -fPIC -fomit-frame-pointer -funroll-loops -O2
CFLAGS += -Ilonghair/longhair-mobile/

#LUA_CFLAGS += -I/usr/include/lua5.1/
#LUA_CFLAGS += -I/usr/include/lua5.2/
LUA_CFLAGS := -I/usr/include/lua5.3/

LDFLAGS := -fPIC -nostartfiles -shared

#cmoddir = /usr/local/lib/lua/5.1/
#cmoddir = /usr/local/lib/lua/5.2/
cmoddir = /usr/local/lib/lua/5.3/
