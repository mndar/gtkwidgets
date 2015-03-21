#include "barrel.h"
#include <math.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include "barrel-marshallers.h"
#define BARREL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BARREL_TYPE, BarrelPrivate))

G_DEFINE_TYPE (Barrel, barrel, GTK_TYPE_DRAWING_AREA);

typedef struct _BarrelPrivate BarrelPrivate;

 enum
{
	DANGER_LEVEL,
	LAST_SIGNAL
};

 static guint barrel_class_signals[LAST_SIGNAL] = { 0 };


struct _BarrelPrivate
{
	char isRunning;
	double value,degreeIncrement,everyDegreeValue;	
	double markersAt;
	int majorMarkerEvery,midMarkerEvery,numMarkers;
	double startValue,endValue;
	double finalValue,stepValue;
	double valueFontSize;
	double fillLevel;
};

static void barrel_class_init (BarrelClass *class)
{
	GtkWidgetClass *widget_class;
	GObjectClass *obj_class;

	obj_class = G_OBJECT_CLASS (class);
	widget_class = GTK_WIDGET_CLASS (class);
	widget_class->expose_event = barrel_expose;
	
	g_type_class_add_private (obj_class, sizeof (BarrelPrivate));
	barrel_class_signals[DANGER_LEVEL] = g_signal_new (
			"danger-level",
			G_OBJECT_CLASS_TYPE (obj_class),
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (BarrelClass, danger_level),
			NULL, NULL,
			_barrel_marshal_VOID__DOUBLE,			
			G_TYPE_NONE, 1,
			G_TYPE_DOUBLE);

}

 static void emit_danger_level_signal (Barrel *barrel, double value) {
	 
	  g_signal_emit (barrel,
				barrel_class_signals[DANGER_LEVEL],
				0,value);
 }
 
static void barrel_init (Barrel *barrel)
{
	BarrelPrivate *priv;	
	priv = BARREL_GET_PRIVATE (barrel);
	priv->value=-1;
	priv->isRunning=0;	
	priv->valueFontSize=-1;
// 	g_timeout_add (10, barrel_update, barrel);
}

GtkWidget * barrel_new (double startValue,double endValue,double markersAt,int majorMarkerEvery)
{
	Barrel *barrel;
	BarrelPrivate *priv;
	GtkWidget *widget;
	widget=g_object_new (BARREL_TYPE, NULL);
	barrel=(Barrel *)widget;
	priv=BARREL_GET_PRIVATE(barrel);
	priv->startValue=startValue;
	priv->endValue=endValue;
	priv->markersAt=markersAt;
	priv->majorMarkerEvery=majorMarkerEvery;
	priv->midMarkerEvery=majorMarkerEvery/2;
	priv->numMarkers=(endValue-startValue)/markersAt+1;
	/*number of degress to increment for each marker */
	priv->degreeIncrement=270/((endValue-startValue)/markersAt);
	priv->everyDegreeValue=(endValue-startValue)/270;
	
// 	printf("numMarkers:%d degreeIncrement: %.2lf\n",priv->numMarkers,priv->degreeIncrement);
	return widget;
}

static gboolean barrel_expose (GtkWidget *barrel, GdkEventExpose *event)
{
	cairo_t *cr;
	/* get a cairo_t */
	cr = gdk_cairo_create (barrel->window);
// 	cairo_set_antialias(cr,CAIRO_ANTIALIAS_GRAY);
	cairo_set_antialias(cr,CAIRO_ANTIALIAS_DEFAULT);
	/* set a clip region for the expose event */
   	cairo_rectangle (cr,event->area.x, event->area.y,event->area.width, event->area.height);
  	cairo_clip (cr);
	draw (barrel, cr);
	cairo_destroy (cr);
	return FALSE;
}

static void draw(GtkWidget *barrel, cairo_t *cr)
{
	static int firstCall=1;
	
	int count=0;
	double width,height,startX,startY,indicatorSourceX,indicatorSourceY;
	double barrelHeight,barrelWidth;
	double radius,degValue;
	double posX,posY;
	double i,presentDeg,presentVal;
	double markerStartX,markerEndX,markerStartY,markerEndY;
	double fillLevel;
	GError *pError=NULL;
	BarrelPrivate *priv;
  	RsvgHandle *rsvgHandle=rsvg_handle_new_from_file("../images/barrel.svg",&pError);
	RsvgHandle *rsvgHandle2=rsvg_handle_new_from_file("../images/barrel_layer.svg",&pError);
	RsvgHandle *rsvgHandle3=rsvg_handle_new_from_file("../images/level_top.svg",&pError);
	
	priv = BARREL_GET_PRIVATE (barrel);
	if(firstCall==1) {
		firstCall=0;
		priv->value=priv->startValue;
	}	
	barrelWidth=MIN(barrel->allocation.width,barrel->allocation.height);;
	barrelHeight=MIN(barrel->allocation.width,barrel->allocation.height);
	
	if(priv->value>=(0.5*priv->endValue))
		emit_danger_level_signal((Barrel *)barrel,priv->value);
	
// 	fillLevel=76.5;
	fillLevel=76.5*priv->value/priv->endValue;
 	if(fillLevel<1.2)
 		fillLevel=1.2;
	posY=88.5-fillLevel;
	cairo_save(cr);

	cairo_scale(cr,barrelWidth/100,barrelHeight/100);
 	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#bottom");
	cairo_translate(cr,0.0,posY);	
	cairo_rectangle(cr,1.658,0,96.5,fillLevel);
	cairo_set_source_rgb(cr,134.0/255.0,1.0,0.0);
	cairo_fill(cr);	
	cairo_translate(cr,0.0,-posY);
	
	posY-=11.862;
	cairo_translate(cr,0.8,posY);	
	rsvg_handle_render_cairo(rsvgHandle3,cr);
	cairo_translate(cr,-0.8,-posY);	
// 	rsvg_handle_render_cairo(rsvgHandle2,cr);
 	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#top");
  	rsvg_handle_render_cairo(rsvgHandle2,cr);
    	cairo_restore(cr);
		
}


static void barrel_redraw_canvas (Barrel *barrel)
{
	GtkWidget *widget;
	GdkRegion *region;

	widget = GTK_WIDGET (barrel);

	if (!widget->window) return;

	region = gdk_drawable_get_clip_region (widget->window);
	/* redraw the cairo canvas completely by exposing it */
	gdk_window_invalidate_region (widget->window, region, TRUE);
	gdk_window_process_updates (widget->window, TRUE);

	gdk_region_destroy (region);
}


void barrel_set_value(Barrel *meter,gdouble value) {	
	BarrelPrivate *priv=BARREL_GET_PRIVATE(meter);	
// 	printf("%.2lf %.2lf\n",priv->endValue,value);
	if(value<priv->startValue || value>priv->endValue) {
		g_warning("Min Value is %.2lf Max Value is %.2lf. You asked for %.2lf. Will NOT comply",priv->startValue,priv->endValue,value);
	}
	else {
		priv->value=value;
		barrel_redraw_canvas (meter);
	}
}

void barrel_alter_value(Barrel *meter,gdouble change) {	
	BarrelPrivate *priv=BARREL_GET_PRIVATE(meter);	
// 	printf("%.2lf %.2lf\n",priv->endValue,value);
	double value=priv->value+change;
	if(value<priv->startValue || value>priv->endValue) {
		g_warning("Min Value is %.2lf Max Value is %.2lf. You asked for %.2lf. Will NOT comply",priv->startValue,priv->endValue,value);
	}
	else {
		priv->value=value;
		barrel_redraw_canvas (meter);
	}
}

gdouble barrel_get_value(Barrel *meter) {
	BarrelPrivate *priv=BARREL_GET_PRIVATE(meter);
	return priv->value;	
}

