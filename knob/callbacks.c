
#include "knob.h"
#include "callbacks.h"
#include "defs.h"
void button_clicked_cb(GtkWidget *widget,gpointer data) {
	MyWidgets *myWidgets=(MyWidgets *)data;	

	Knob *knob=KNOB(myWidgets->knob);
 	knob_alter_value(knob,g_strtod(gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)),NULL));
// 	knob_set_value(knob,g_strtod(gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)),NULL));
}

void knob_position_changed_cb(Knob *kob,double value,gpointer data) {
	
	g_printf("Knob Value: %.2lf\n",value);	
}
