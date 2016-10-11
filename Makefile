include config.mk

clib := longhair.so

all: longhair/longhair-mobile/cauchy_256.c $(clib)

%.o: %.c
	$(CC) $(CFLAGS) $(LUA_CFLAGS) -Ilonghair/longhair-mobile/  -c $< -o $@

longhair/longhair-mobile/cauchy_256.c:
	git submodule init
	git submodule update

$(clib): src/main.o longhair/longhair-mobile/cauchy_256.o
	$(CC) $(LDFLAGS) $< -o $@

install: $(clib)
	install -m 644 $< $(cmoddir)

clean:
	rm -f longhair.so src/main.o
