
#include "barrel.h"
#include "callbacks.h"
#include "defs.h"
void button_clicked_cb(GtkWidget *widget,gpointer data) {
	MyWidgets *myWidgets=(MyWidgets *)data;	

	Barrel *barrel=BARREL(myWidgets->barrel);
	barrel_alter_value(barrel,g_strtod(gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)),NULL));
	
}

void barrel_danger_level_cb(Barrel *barrel,double value,gpointer data) {
	
	g_printf("Holy Crap: Danger Level Reached: %.2lf\n",value);	
}
