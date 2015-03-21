#include "speedometer.h"
#include "callbacks.h"
#include "defs.h"
void button_clicked_cb(GtkWidget *widget,gpointer data) {
	MyWidgets *myWidgets=(MyWidgets *)data;	
	SpeedoMeter *meter=SPEEDO_METER(myWidgets->speedoMeter);
//  	speedo_meter_set_value(meter,g_strtod(gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)),NULL));
	speedo_meter_alter_value(meter,g_strtod(gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)),NULL));
}
