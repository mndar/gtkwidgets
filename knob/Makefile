all: knob

knob: knob1.c knob.h main.c knob-marshallers.c callbacks.h callbacks.c knob-marshallers.h
	gcc -g -o knob callbacks.c knob1.c main.c knob-marshallers.c `pkg-config --libs --cflags gtk+-2.0 librsvg-2.0`

knob-marshallers.c: knob-marshallers.list
	glib-genmarshal --prefix _knob_marshal --body $< > $@

knob-marshallers.h: knob-marshallers.list
	glib-genmarshal --prefix _knob_marshal --header $< > $@

