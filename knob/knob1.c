#include "knob.h"
#include <math.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include "knob-marshallers.h"
#define KNOB_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), KNOB_TYPE, KnobPrivate))

G_DEFINE_TYPE (Knob, knob, GTK_TYPE_DRAWING_AREA);

typedef struct _KnobPrivate KnobPrivate;

 enum
{
	POSITION_CHANGED,
	LAST_SIGNAL
};

 static guint knob_class_signals[LAST_SIGNAL] = { 0 };


struct _KnobPrivate
{
	char isRunning;
	double value,degreeIncrement,everyDegreeValue;	
	double markersAt;
	int majorMarkerEvery,midMarkerEvery,numMarkers;
	double startValue,endValue;
	double finalValue,stepValue;
	double valueFontSize;
	double fillLevel;
	double prevDeg;
	gboolean dragging;
};

static void knob_class_init (KnobClass *class)
{
	GtkWidgetClass *widget_class;
	GObjectClass *obj_class;

	obj_class = G_OBJECT_CLASS (class);
	widget_class = GTK_WIDGET_CLASS (class);
	widget_class->expose_event = knob_expose;
 	widget_class->button_press_event = knob_button_press;
 	widget_class->button_release_event = knob_button_release;
 	widget_class->motion_notify_event = knob_motion_notify;
	widget_class->scroll_event = knob_scroll_event;

	

	g_type_class_add_private (obj_class, sizeof (KnobPrivate));
	knob_class_signals[POSITION_CHANGED] = g_signal_new (
			"position-changed",
			G_OBJECT_CLASS_TYPE (obj_class),
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (KnobClass, position_changed),
			NULL, NULL,
			_knob_marshal_VOID__DOUBLE,			
			G_TYPE_NONE, 1,
			G_TYPE_DOUBLE);

}

double get_closest_knob_value(double deg) {
	
	if(deg>(360-11.25))
		return 360;
	else if(deg>(337.5-11.25))
		return 337.5;
	else if(deg>(315-11.25))
		return 315;
	else if(deg>(292.5-11.25))
		return 292.5;
	else if(deg>(270-11.25))
		return 270;
	else if(deg>(247.5-11.25))
		return 247.5;
	else if(deg>(225-11.25))
		return 225;
	else if(deg>(202.5-11.25))
		return 202.5;
	else if(deg>(180-11.25))
		return 180;
	else if(deg>(157.5-11.25))
		return 157.5;
	else if(deg>(135-11.25))
		return 135;
	else if(deg>(112.5-11.25))
		return 112.5;
	else if(deg>(90-11.25))
		return 90;
	else if(deg>(67.5-11.25))
		return 67.5;
	else if(deg>(45-11.25))
		return 45;
	else if(deg>(22.5-11.25))
		return 22.5;
	else 
		return 0;
	
}
static gboolean knob_scroll_event (GtkWidget *widget, GdkEventScroll *event) {
	KnobPrivate *priv;
	double closestDeg;
	priv = KNOB_GET_PRIVATE (widget);
	
	if(event->direction==GDK_SCROLL_UP) {
		closestDeg=priv->prevDeg+22.5;
		if(closestDeg>360)
			closestDeg=closestDeg-360;
	}
	else {
		closestDeg=priv->prevDeg-22.5;
		if(closestDeg<0)
			closestDeg=360+closestDeg;	
	}
	
	priv->prevDeg=get_closest_knob_value(closestDeg);
	knob_set_value(KNOB(widget),-priv->prevDeg);
	priv->prevDeg=closestDeg;
	g_signal_emit (widget, knob_class_signals[POSITION_CHANGED],0,priv->prevDeg);	
	return TRUE;
}

 static void emit_position_changed_signal (Knob *knob, int x, int y)
{
	KnobPrivate *priv;
	double deg,lx,ly,diffDeg,X,Y,closestDeg;
	
	priv = KNOB_GET_PRIVATE (knob);
// 	printf("At %d %d\n",x,y);
	lx=fabs(GTK_WIDGET(knob)->allocation.width/2-x);
	ly=fabs(GTK_WIDGET(knob)->allocation.height/2-y);
	X=GTK_WIDGET(knob)->allocation.width/2;
	Y=GTK_WIDGET(knob)->allocation.height/2;
// 	printf("l %.2lf %.2lf %.2lf %.2lf\n",lx,ly,X,Y);
	deg=atan(ly/lx)*180/M_PI;
	if(x>X && y<=Y)
		deg=180-deg;
	else if(x>=X && y>Y)
		deg=180+deg;
	else if(x<X && y>Y)
		deg=360-deg;
	diffDeg=fabs(deg-priv->prevDeg);
	
//  	printf("Deg: %.2lf %.2lf %.2lf\n",deg,priv->prevDeg,diffDeg);	
	if(diffDeg>22.5) {		
		closestDeg=get_closest_knob_value(deg);
// 		printf("Deg: %.2lf Closest Deg: %.2lf\n",deg,closestDeg);
		knob_set_value(knob,-closestDeg);		
		priv->prevDeg=closestDeg;
		g_signal_emit (knob, knob_class_signals[POSITION_CHANGED],0,closestDeg);	
	}
 	
}

 static gboolean knob_button_release (GtkWidget *knob, GdkEventButton *event)
{
	KnobPrivate *priv;

	priv = KNOB_GET_PRIVATE(knob);

	if (priv->dragging)
	{		
		priv->dragging = FALSE;
		emit_position_changed_signal (KNOB (knob),
					  event->x, event->y);
	}

	return FALSE;
}


static gboolean knob_button_press (GtkWidget *knob, GdkEventButton *event)
{
	
	KnobPrivate *priv;
	int minutes;
	double lx, ly;
	double px, py;
	double u, d;
	
	priv = KNOB_GET_PRIVATE (knob);

	lx=knob->allocation.width/2-event->x;
	ly=knob->allocation.height/2-event->y;
	d=sqrt(lx*lx + ly*ly);
// 	printf("%.2lf %.2lf %.2lf %.2lf %.2lf\n",event->x,event->y,lx,ly,d);
	if(d<0.86*knob->allocation.width/2) {
// 		printf("Got Mouse\n");		
		priv->dragging=TRUE;
	}
	return FALSE;
}

static gboolean knob_motion_notify (GtkWidget *knob, GdkEventMotion *event)
{
	KnobPrivate *priv;
	int x, y;

	priv = KNOB_GET_PRIVATE (knob);

	if (priv->dragging)
	{		
		emit_position_changed_signal (KNOB(knob),event->x, event->y);
	}
}


 
static void knob_init (Knob *knob)
{
	KnobPrivate *priv;	
	priv = KNOB_GET_PRIVATE (knob);
	priv->value=0;
	gtk_widget_add_events (GTK_WIDGET (knob),
			       GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
					       GDK_POINTER_MOTION_MASK);
	priv->dragging=FALSE;
	priv->prevDeg=0;
// 	g_timeout_add (10, knob_update, knob);
}

GtkWidget * knob_new (double startValue,double endValue,double markersAt,int majorMarkerEvery)
{
	Knob *knob;
	KnobPrivate *priv;
	GtkWidget *widget;
	widget=g_object_new (KNOB_TYPE, NULL);
	knob=(Knob *)widget;
	priv=KNOB_GET_PRIVATE(knob);
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

static gboolean knob_expose (GtkWidget *knob, GdkEventExpose *event)
{
	cairo_t *cr;
	/* get a cairo_t */
	cr = gdk_cairo_create (knob->window);
// 	cairo_set_antialias(cr,CAIRO_ANTIALIAS_GRAY);
	cairo_set_antialias(cr,CAIRO_ANTIALIAS_DEFAULT);
	/* set a clip region for the expose event */
   	cairo_rectangle (cr,event->area.x, event->area.y,event->area.width, event->area.height);
  	cairo_clip (cr);
	draw (knob, cr);
	cairo_destroy (cr);
	return FALSE;
}

static void draw(GtkWidget *knob, cairo_t *cr)
{
	static int firstCall=1;
	
	int count=0;
	double width,height,startX,startY,indicatorSourceX,indicatorSourceY;
	double knobHeight,knobWidth;
	double radius,degValue;
	double posX,posY;
	double i,presentDeg,presentVal;
	double markerStartX,markerEndX,markerStartY,markerEndY;
	double fillLevel;
	GError *pError=NULL;
	KnobPrivate *priv;
  	RsvgHandle *rsvgHandle=rsvg_handle_new_from_file("../images/knob.svg",&pError);
	
	double r,dx,dy,newx,newy;
	priv = KNOB_GET_PRIVATE (knob);
	knobWidth=MIN(knob->allocation.width,knob->allocation.height);;
	knobHeight=MIN(knob->allocation.width,knob->allocation.height);
	
	r=sqrt(5000.0);
// 	priv->value=292.5;
 	cairo_save(cr);
 	cairo_scale(cr,knobWidth/100,knobHeight/100);
	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#back");
	newx=r*sin((135-priv->value)*M_PI/180);
	newy=-r*cos((135-priv->value)*M_PI/180);
	dx=newx-50;
	dy=newy-50;
// 	printf("%.2lf %.2lf %.2lf %.2lf %.2lf \n",r,newx,newy,dx,dy);
	cairo_translate(cr,-dx,-dy);
  	cairo_rotate(cr,-priv->value*M_PI/180);	
	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#realKnob");
 	cairo_restore(cr);	
}


static void knob_redraw_canvas (Knob *knob)
{
	GtkWidget *widget;
	GdkRegion *region;

	widget = GTK_WIDGET (knob);

	if (!widget->window) return;

	region = gdk_drawable_get_clip_region (widget->window);
	/* redraw the cairo canvas completely by exposing it */
	gdk_window_invalidate_region (widget->window, region, TRUE);
	gdk_window_process_updates (widget->window, TRUE);

	gdk_region_destroy (region);
}


void knob_set_value(Knob *meter,gdouble value) {	
	KnobPrivate *priv=KNOB_GET_PRIVATE(meter);	
// 	printf("%.2lf %.2lf\n",priv->endValue,value);	
	priv->value=value;
	knob_redraw_canvas (meter);	
}

void knob_alter_value(Knob *meter,gdouble change) {	
	KnobPrivate *priv=KNOB_GET_PRIVATE(meter);	
// 	printf("%.2lf %.2lf\n",priv->endValue,value);
	double value=priv->value+change;
	priv->value=value;
	knob_redraw_canvas (meter);	
}

gdouble knob_get_value(Knob *meter) {
	KnobPrivate *priv=KNOB_GET_PRIVATE(meter);
	return priv->value;	
}

