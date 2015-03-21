
#include "thermometer.h"
#include "callbacks.h"
#include "defs.h"
void button_clicked_cb(GtkWidget *widget,gpointer data) {
	MyWidgets *myWidgets=(MyWidgets *)data;	

	Thermometer *thermometer=THERMOMETER(myWidgets->thermometer);
  	thermometer_alter_value(thermometer,g_strtod(gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)),NULL));
//  	thermometer_set_value(thermometer,g_strtod(gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)),NULL));
}

void thermometer_danger_level_cb(Thermometer *thermometer,double value,gpointer data) {
	
	g_printf("Holy Crap: Danger Level Reached: %.2lf\n",value);	
}
