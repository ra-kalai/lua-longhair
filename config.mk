CC := gcc
CFLAGS := -Wall -fPIC -nostartfiles -shared  -fomit-frame-pointer -funroll-loops -O2
#CFLAGS += -I/usr/include/lua5.1/
#CFLAGS += -I/usr/include/lua5.2/
CFLAGS += -I/usr/include/lua5.3/
CFLAGS += -Ilonghair/longhair-mobile/

#cmoddir = /usr/local/lib/lua/5.1/
#cmoddir = /usr/local/lib/lua/5.2/
cmoddir = /usr/local/lib/lua/5.3/
