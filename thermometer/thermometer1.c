#include "thermometer.h"
#include <math.h>
#include <string.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include "thermometer-marshallers.h"
#define THERMOMETER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), THERMOMETER_TYPE, ThermometerPrivate))

G_DEFINE_TYPE (Thermometer, thermometer, GTK_TYPE_DRAWING_AREA);

typedef struct _ThermometerPrivate ThermometerPrivate;
 enum
{
	DANGER_LEVEL,
	LAST_SIGNAL
};

 static guint thermometer_class_signals[LAST_SIGNAL] = { 0 };


struct _ThermometerPrivate
{
	double value;
	double startValue,endValue;
	double fillLevel;
	TempType tempType;
};

static void thermometer_class_init (ThermometerClass *class)
{
	GtkWidgetClass *widget_class;
	GObjectClass *obj_class;

	obj_class = G_OBJECT_CLASS (class);
	widget_class = GTK_WIDGET_CLASS (class);
	widget_class->expose_event = thermometer_expose;
	
	g_type_class_add_private (obj_class, sizeof (ThermometerPrivate));
	thermometer_class_signals[DANGER_LEVEL] = g_signal_new (
			"danger-level",
			G_OBJECT_CLASS_TYPE (obj_class),
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (ThermometerClass, danger_level),
			NULL, NULL,
			_thermometer_marshal_VOID__DOUBLE,			
			G_TYPE_NONE, 1,
			G_TYPE_DOUBLE);

}

 static void emit_danger_level_signal (Thermometer *thermometer, double value) {
	 
	  g_signal_emit (thermometer,
				thermometer_class_signals[DANGER_LEVEL],
				0,value);
 }
 
static void thermometer_init (Thermometer *thermometer)
{
	ThermometerPrivate *priv;	
	priv = THERMOMETER_GET_PRIVATE (thermometer);
	priv->value=-1;
// 	g_timeout_add (10, thermometer_update, thermometer);
}

GtkWidget * thermometer_new (double startValue,double endValue,TempType type)
{
	Thermometer *thermometer;
	ThermometerPrivate *priv;
	GtkWidget *widget;
	widget=g_object_new (THERMOMETER_TYPE, NULL);
	thermometer=(Thermometer *)widget;
	priv=THERMOMETER_GET_PRIVATE(thermometer);
	priv->startValue=startValue;
	priv->endValue=endValue;
	priv->tempType=type;
	return widget;
}

static gboolean thermometer_expose (GtkWidget *thermometer, GdkEventExpose *event)
{
	cairo_t *cr;
	/* get a cairo_t */
	cr = gdk_cairo_create (thermometer->window);
// 	cairo_set_antialias(cr,CAIRO_ANTIALIAS_GRAY);
	cairo_set_antialias(cr,CAIRO_ANTIALIAS_DEFAULT);
	/* set a clip region for the expose event */
   	cairo_rectangle (cr,event->area.x, event->area.y,event->area.width, event->area.height);
  	cairo_clip (cr);
	draw (thermometer, cr);
	cairo_destroy (cr);
	return FALSE;
}

static void draw(GtkWidget *thermometer, cairo_t *cr)
{
	static int firstCall=1;
	
	int count=0;
	char text[6];
	double width,height;
	double thermometerHeight,thermometerWidth;	
	double posX,posY;
	int i;
	double fillLevel;
	double x1,x2,y1,y2;
	GError *pError=NULL;
	ThermometerPrivate *priv;
  	RsvgHandle *rsvgHandle=rsvg_handle_new_from_file("../images/thermometer.svg",&pError);
 	
	
	priv = THERMOMETER_GET_PRIVATE (thermometer);
	if(firstCall==1) {
		firstCall=0;
		priv->value=priv->startValue;
	}	
	thermometerWidth=thermometer->allocation.width;
	thermometerHeight=thermometer->allocation.height;
	
	if(priv->value>=(0.5*(priv->endValue-priv->startValue)))
		emit_danger_level_signal((Thermometer *)thermometer,priv->value);

	fillLevel=85.193*(priv->value-priv->startValue)/(priv->endValue-priv->startValue);
// 	printf("Fill Level: %.2f\n",fillLevel);
	posY=104.75-fillLevel;
	
	cairo_save(cr);
	cairo_scale(cr,thermometerWidth/20,thermometerHeight/140);
	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#background");
	cairo_set_source_rgb(cr,149.0/255.0,18.0/255.0,15.0/255.0);	
// 	cairo_rectangle(cr,11.2,19.672,1.0,86.235);
	cairo_rectangle(cr,10.35,posY,1.0,fillLevel+10.2);		
	cairo_fill(cr);
	cairo_set_source_rgb(cr,0,0,0);
	cairo_set_font_size(cr,2.5);
	
	if(priv->tempType==TEMP_CELSIUS)
		sprintf(text,"%.1f%cC",priv->value,32);
	else
		sprintf(text,"%.1f%cF",priv->value,32);
 	cairo_text_path(cr,text);
	cairo_path_extents(cr,&x1,&y1,&x2,&y2);
	cairo_new_path(cr);
	cairo_move_to(cr,11.4-(x2-x1)/2,8.0);
	cairo_text_path(cr,text);
	cairo_fill(cr);
	int incrementValue=(int)(priv->endValue-priv->startValue)/10;
	for(i=0;i<11;i++) {
		cairo_move_to(cr,11.5,105.64-i*8.5);		
		sprintf(text,"%d",(int)priv->startValue+i*incrementValue);
		cairo_text_path(cr,text);
		cairo_fill(cr);
	}
    	cairo_restore(cr);
}


static void thermometer_redraw_canvas (Thermometer *thermometer)
{
	GtkWidget *widget;
	GdkRegion *region;

	widget = GTK_WIDGET (thermometer);

	if (!widget->window) return;

	region = gdk_drawable_get_clip_region (widget->window);
	/* redraw the cairo canvas completely by exposing it */
	gdk_window_invalidate_region (widget->window, region, TRUE);
	gdk_window_process_updates (widget->window, TRUE);

	gdk_region_destroy (region);
}


void thermometer_set_value(Thermometer *meter,gdouble value) {	
	ThermometerPrivate *priv=THERMOMETER_GET_PRIVATE(meter);	
// 	printf("%.2lf %.2lf\n",priv->endValue,value);
	if(value<priv->startValue || value>priv->endValue) {
		g_warning("Min Value is %.2lf Max Value is %.2lf. You asked for %.2lf. Will NOT comply",priv->startValue,priv->endValue,value);
	}
	else {
		priv->value=value;
		thermometer_redraw_canvas (meter);
	}
}

void thermometer_alter_value(Thermometer *meter,gdouble change) {	
	ThermometerPrivate *priv=THERMOMETER_GET_PRIVATE(meter);	
// 	printf("%.2lf %.2lf\n",priv->endValue,value);
	double value=priv->value+change;
	if(value<priv->startValue || value>priv->endValue) {
		g_warning("Min Value is %.2lf Max Value is %.2lf. You asked for %.2lf. Will NOT comply",priv->startValue,priv->endValue,value);
	}
	else {
		priv->value=value;
		thermometer_redraw_canvas (meter);
	}
}

gdouble thermometer_get_value(Thermometer *meter) {
	ThermometerPrivate *priv=THERMOMETER_GET_PRIVATE(meter);
	return priv->value;	
}

