CFLAGS?=-O2 -std=c17 -pedantic -Wall -Wextra

.PHONY: all clean install uninstall

all:
	$(CC) $(CFLAGS) decrowm.c -o decrowm -lX11

clean:
	rm -f decrowm

install: all
	mkdir -pv $(DESTDIR)/usr/local/bin
	cp -fv decrowm $(DESTDIR)/usr/local/bin/
	chmod 755 $(DESTDIR)/usr/local/bin/decrowm

uninstall:
	rm -fv $(DESTDIR)/usr/local/bin/decrowm
