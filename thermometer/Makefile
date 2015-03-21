all: thermometer

thermometer: thermometer1.c thermometer.h main.c thermometer-marshallers.c thermometer-marshallers.h callbacks.c
	gcc -g -o thermometer callbacks.c thermometer1.c main.c thermometer-marshallers.c `pkg-config --libs --cflags gtk+-2.0 librsvg-2.0`

thermometer-marshallers.c: thermometer-marshallers.list
	glib-genmarshal --prefix _thermometer_marshal --body $< > $@

thermometer-marshallers.h: thermometer-marshallers.list
	glib-genmarshal --prefix _thermometer_marshal --header $< > $@

