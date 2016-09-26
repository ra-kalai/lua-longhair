include config.mk

clib := longhair.so

all: $(clib)

$(clib): src/main.c longhair/longhair-mobile/cauchy_256.c
	$(CC) $(CFLAGS) -Ilonghair/longhair-mobile/  $? -o $@

longhair/longhair-mobile/cauchy_256.c:
	git submodule init
	git submodule update

install: $(clib)
	install -m 644 $< $(cmoddir)

clean:
	rm -f longhair.so
