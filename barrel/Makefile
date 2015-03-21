all: barrel

barrel: barrel1.c barrel.h main.c barrel-marshallers.c barrel-marshallers.h
	gcc -g -o barrel callbacks.c barrel1.c main.c barrel-marshallers.c `pkg-config --libs --cflags gtk+-2.0 librsvg-2.0`

barrel-marshallers.c: barrel-marshallers.list
	glib-genmarshal --prefix _barrel_marshal --body $< > $@

barrel-marshallers.h: barrel-marshallers.list
	glib-genmarshal --prefix _barrel_marshal --header $< > $@

