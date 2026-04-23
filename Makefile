all:
	gcc swm.c -o swm -lX11
install:
	cp swm /usr/local/bin/
