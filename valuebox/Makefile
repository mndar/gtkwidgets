all: valuebox

valuebox: valuebox1.c valuebox.h main.c valuebox-marshallers.c valuebox-marshallers.h callbacks.c
	gcc -g -o valuebox callbacks.c valuebox1.c main.c valuebox-marshallers.c `pkg-config --libs --cflags gtk+-2.0 librsvg-2.0`

valuebox-marshallers.c: valuebox-marshallers.list
	glib-genmarshal --prefix _value_box_marshal --body $< > $@

valuebox-marshallers.h: valuebox-marshallers.list
	glib-genmarshal --prefix _value_box_marshal --header $< > $@

