CFLAGS?=-O2 -std=c17 -pedantic -Wall -Wextra

.PHONY: all clean install uninstall

all:
	$(CC) $(CFLAGS) tinywm.c -o tinywm -lX11

clean:
	rm -f tinywm

install: all
	mkdir -pv $(DESTDIR)/usr/local/bin
	cp -fv tinywm $(DESTDIR)/usr/local/bin/
	chmod 755 $(DESTDIR)/usr/local/bin/tinywm

uninstall:
	rm -fv $(DESTDIR)/usr/local/bin/tinywm
