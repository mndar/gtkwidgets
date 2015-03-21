// gcc -o speedometer `pkg-config --libs --cflags gtk+-2.0 librsvg-2.0`  speedometer1.c main.c
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include "knob.h"
#include "callbacks.h"
#include "defs.h"

int main (int argc, char **argv)
{
	GtkWidget *window;
	GtkWidget *knob,*vbox,*button,*entry;
	GtkFixed *fixed;
	MyWidgets myWidgets;
	gtk_init (&argc, &argv);
	rsvg_init();
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	knob = knob_new (0,100,5,20);
	
	entry=gtk_entry_new();
	vbox = gtk_vbox_new(FALSE, 2);
	button = gtk_button_new_with_label("Set");
	fixed=(GtkFixed *)gtk_fixed_new();
	myWidgets.knob=knob;
	myWidgets.button=button;
	myWidgets.entry=entry;

 	gtk_widget_set_size_request(knob, 250, 250 );
	
	/*	
	gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
 	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE,0);
      	gtk_box_pack_start(GTK_BOX(vbox), knob, FALSE, FALSE, 0);
	*/
	gtk_fixed_put(fixed,entry,20,20);
	gtk_fixed_put(fixed,button,20,50);
//   	gtk_fixed_put(fixed,knob,20,100);
	
          gtk_container_add (GTK_CONTAINER (window), knob);
// 	gtk_container_add (GTK_CONTAINER (window), vbox);

//  	gtk_container_add (GTK_CONTAINER (window),(GtkWidget*) fixed);
	g_signal_connect (window, "destroy",G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (button,"clicked",G_CALLBACK(button_clicked_cb),&myWidgets);
 	g_signal_connect (knob,"position-changed",G_CALLBACK(knob_position_changed_cb),&myWidgets);
	gtk_widget_show_all (window);

	gtk_main ();
}

