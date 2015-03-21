
#include "valuebox.h"
#include "callbacks.h"
#include "defs.h"
void button_clicked_cb(GtkWidget *widget,gpointer data) {
	MyWidgets *myWidgets=(MyWidgets *)data;	
 	value_box_set_value(VALUE_BOX(myWidgets->valueBox),(gchar *)gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)));
	value_box_set_title(VALUE_BOX(myWidgets->valueBox),(gchar *)gtk_entry_get_text(GTK_ENTRY(myWidgets->entry)));
}

void value_box_value_changed_cb(ValueBox *valueBox,gchar *value,gpointer data) {
	printf("Value Changed: %s\n",value);
}

void value_box_title_changed_cb(ValueBox *valueBox,gchar* title,gpointer data) {
	printf("Title Changed: %s\n",title);
}
