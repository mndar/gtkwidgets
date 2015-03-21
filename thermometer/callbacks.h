#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__
#include "thermometer.h"
void button_clicked_cb(GtkWidget *,gpointer);
void thermometer_danger_level_cb(Thermometer *,double,gpointer);
#endif
