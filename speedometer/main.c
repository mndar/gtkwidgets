// gcc -o speedometer `pkg-config --libs --cflags gtk+-2.0 librsvg-2.0`  speedometer1.c main.c
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include "speedometer.h"
#include "callbacks.h"
#include "defs.h"

int main (int argc, char **argv)
{
	GtkWidget *window;
	GtkWidget *speedoMeter,*vbox,*button,*entry;
	MyWidgets myWidgets;
	gtk_init (&argc, &argv);
	rsvg_init();
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	speedoMeter = speedo_meter_new (0,500,5,20);
	speedo_meter_marker_value_show(speedoMeter,9.0);
	entry=gtk_entry_new();
	vbox = gtk_vbox_new(FALSE, 2);
	button = gtk_button_new_with_label("Set");
	myWidgets.speedoMeter=speedoMeter;
	myWidgets.button=button;
	myWidgets.entry=entry;

 	gtk_widget_set_size_request(speedoMeter, 250, 250 );
		
	gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
 	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE,0);
     	gtk_box_pack_start(GTK_BOX(vbox), speedoMeter, FALSE, FALSE, 0);

	
//     	gtk_container_add (GTK_CONTAINER (window), speedoMeter);
      	gtk_container_add (GTK_CONTAINER (window), vbox);
	g_signal_connect (window, "destroy",G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (button,"clicked",G_CALLBACK(button_clicked_cb),&myWidgets);
	gtk_widget_show_all (window);

	gtk_main ();
}

