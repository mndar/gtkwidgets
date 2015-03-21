#include "speedometer.h"
#include <math.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#define SPEEDO_METER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), SPEEDO_METER_TYPE, SpeedoMeterPrivate))

G_DEFINE_TYPE (SpeedoMeter, speedo_meter, GTK_TYPE_DRAWING_AREA);

typedef struct _SpeedoMeterPrivate SpeedoMeterPrivate;

struct _SpeedoMeterPrivate
{
	char isRunning;
	double value,degreeIncrement,everyDegreeValue;	
	double markersAt;
	int majorMarkerEvery,midMarkerEvery,numMarkers;
	double startValue,endValue;
	double finalValue,stepValue;
	double valueFontSize;
};

static void speedo_meter_class_init (SpeedoMeterClass *class)
{
	GtkWidgetClass *widget_class;
	GObjectClass *obj_class;

	obj_class = G_OBJECT_CLASS (class);
	widget_class = GTK_WIDGET_CLASS (class);
	widget_class->expose_event = speedo_meter_expose;
	
	g_type_class_add_private (obj_class, sizeof (SpeedoMeterPrivate));
}

static void speedo_meter_init (SpeedoMeter *meter)
{
	SpeedoMeterPrivate *priv;	
	priv = SPEEDO_METER_GET_PRIVATE (meter);
	priv->value=-1;
	priv->isRunning=0;	
	priv->valueFontSize=-1;
// 	g_timeout_add (10, speedo_meter_update, meter);
}

GtkWidget * speedo_meter_new (double startValue,double endValue,double markersAt,int majorMarkerEvery)
{
	SpeedoMeter *meter;
	SpeedoMeterPrivate *priv;
	GtkWidget *widget;
	widget=g_object_new (SPEEDO_METER_TYPE, NULL);
	meter=(SpeedoMeter *)widget;
	priv=SPEEDO_METER_GET_PRIVATE(meter);
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

static gboolean speedo_meter_expose (GtkWidget *meter, GdkEventExpose *event)
{
	cairo_t *cr;
	/* get a cairo_t */
	cr = gdk_cairo_create (meter->window);
	/* set a clip region for the expose event */
   	cairo_rectangle (cr,event->area.x, event->area.y,event->area.width, event->area.height);
  	cairo_clip (cr);
	draw (meter, cr);
	cairo_destroy (cr);
	return FALSE;
}

static void draw(GtkWidget *meter, cairo_t *cr)
{
	static int firstCall=1;
	
	int count=0;
	double width,height,startX,startY,indicatorSourceX,indicatorSourceY;
	double radius,degValue;
	double posX,posY;
	double i,presentDeg,presentVal;
	double markerStartX,markerEndX,markerStartY,markerEndY;
	GError *pError=NULL;
	SpeedoMeterPrivate *priv;
  	RsvgHandle *rsvgHandle=rsvg_handle_new_from_file("../images/background.svg",&pError);
	
	priv = SPEEDO_METER_GET_PRIVATE (meter);
	if(firstCall==1) {
		firstCall=0;
		priv->value=priv->startValue;
	}
	
	indicatorSourceX=meter->allocation.width/2;
	indicatorSourceY=meter->allocation.height/2;
// 	printf("%d %d %.2lf %.2lf\n",meter->allocation.x,meter->allocation.y,indicatorSourceX,indicatorSourceY);
	radius=MIN(meter->allocation.height,meter->allocation.width)/2;
		
	cairo_save(cr);
 	cairo_translate(cr,meter->allocation.width/2-radius,meter->allocation.height/2-radius);
	cairo_scale(cr,radius*2/100,radius*2/100);
 	rsvg_handle_render_cairo(rsvgHandle,cr);
    	cairo_restore(cr);
	
/*	radius-=3;
 	cairo_set_line_width(cr,6);
	cairo_set_source_rgb(cr, 255,0, 0);
 	cairo_arc (cr, indicatorSourceX, indicatorSourceY, radius, 0, 2 * M_PI); 	
  	cairo_stroke(cr);*/
	if(priv->value==-1)
		degValue=315;
	else
		degValue=315+(priv->value-priv->startValue)/priv->everyDegreeValue;
 	cairo_move_to(cr,indicatorSourceX,indicatorSourceY);
	posX=indicatorSourceX-0.7*radius*cos(degValue*M_PI/180);
	posY=indicatorSourceY-0.7*radius*sin(degValue*M_PI/180);
//  	cairo_line_to(cr,startX+priv->value,startY);
 	cairo_line_to(cr,posX,posY);
	cairo_set_source_rgb(cr, 255,255, 255);
  	cairo_stroke(cr);	
	

	cairo_move_to(cr,indicatorSourceX,indicatorSourceY);
	posX=indicatorSourceX-0.1*radius*cos((degValue+180)*M_PI/180);
	posY=indicatorSourceY-0.1*radius*sin((degValue+180)*M_PI/180);
//  	cairo_line_to(cr,startX+priv->value,startY);
	cairo_line_to(cr,posX,posY);
	cairo_set_source_rgb(cr, 255,255, 255);
	cairo_stroke(cr);	
	
	for(i=0;;i+=priv->degreeIncrement) {
// 		printf("i:%.2lf\n",i);
		presentDeg=(i+315);
// 		printf("i=%.0lf presentDeg=%.0lf\n",i,presentDeg);
		
		if(count % (int)priv->majorMarkerEvery == 0) {
			markerStartX=indicatorSourceX-0.77*radius*cos(presentDeg*M_PI/180);			
			markerStartY=indicatorSourceY-0.77*radius*sin(presentDeg*M_PI/180); 			
			presentVal=i*priv->everyDegreeValue;
			if(priv->valueFontSize>0) {
				if((int)presentVal==(int)priv->endValue*0.5)
					cairo_move_to(cr,markerStartX-priv->valueFontSize*0.75,markerStartY+15);
				else if(presentVal>priv->endValue*0.5)
					cairo_move_to(cr,markerStartX-priv->valueFontSize*2,markerStartY);
				else 
					cairo_move_to(cr,markerStartX+6,markerStartY);
				cairo_set_font_size(cr,priv->valueFontSize);
				cairo_set_source_rgb(cr, 255,255, 255);
				cairo_show_text(cr,g_strdup_printf("%.0lf",presentVal));
			}
		}
		else if (count % priv->midMarkerEvery == 0) {			
			markerStartX=indicatorSourceX-0.83*radius*cos(presentDeg*M_PI/180);			
			markerStartY=indicatorSourceY-0.83*radius*sin(presentDeg*M_PI/180);			
		}
		else {
			markerStartX=indicatorSourceX-0.88*radius*cos(presentDeg*M_PI/180);			
			markerStartY=indicatorSourceY-0.88*radius*sin(presentDeg*M_PI/180);			
		}
		
		markerEndX=indicatorSourceX-0.92*radius*cos(presentDeg*M_PI/180);
		markerEndY=indicatorSourceY-0.92*radius*sin(presentDeg*M_PI/180);
		cairo_move_to(cr,markerStartX,markerStartY);
		cairo_line_to(cr,markerEndX,markerEndY);
		cairo_set_source_rgb(cr, 255,255, 255);
		cairo_stroke(cr);
				
		
		cairo_arc (cr, indicatorSourceX, indicatorSourceY, 0.03*radius, 0, 2 * M_PI); 	
		cairo_set_source_rgb(cr,255,255,255);
		cairo_fill(cr);
		count++;
		if(count==priv->numMarkers)
			break;
	}
	
}


static void speedo_meter_redraw_canvas (SpeedoMeter *meter)
{
	GtkWidget *widget;
	GdkRegion *region;

	widget = GTK_WIDGET (meter);

	if (!widget->window) return;

	region = gdk_drawable_get_clip_region (widget->window);
	/* redraw the cairo canvas completely by exposing it */
	gdk_window_invalidate_region (widget->window, region, TRUE);
	gdk_window_process_updates (widget->window, TRUE);

	gdk_region_destroy (region);
}


void speedo_meter_marker_value_show(SpeedoMeter *meter,gdouble valueFontSize) {
	SpeedoMeterPrivate *priv=SPEEDO_METER_GET_PRIVATE(meter);
	priv->valueFontSize=valueFontSize;
}

void speedo_meter_set_value(SpeedoMeter *meter,gdouble value) {	
	SpeedoMeterPrivate *priv=SPEEDO_METER_GET_PRIVATE(meter);	
// 	printf("%.2lf %.2lf\n",priv->endValue,value);
	if(value<priv->startValue || value>priv->endValue) {
		g_warning("Min Value is %.2lf Max Value is %.2lf. You asked for %.2lf. Will NOT comply",priv->startValue,priv->endValue,value);
	}
	else {
		priv->value=value;
		speedo_meter_redraw_canvas (meter);
	}
}

void speedo_meter_alter_value(SpeedoMeter *meter,gdouble change) {	
	SpeedoMeterPrivate *priv=SPEEDO_METER_GET_PRIVATE(meter);	
// 	printf("%.2lf %.2lf\n",priv->endValue,value);
	double value=priv->value+change;
	if(value<priv->startValue || value>priv->endValue) {
		g_warning("Min Value is %.2lf Max Value is %.2lf. You asked for %.2lf. Will NOT comply",priv->startValue,priv->endValue,value);
	}
	else {
		priv->value=value;
		speedo_meter_redraw_canvas (meter);
	}
}

gdouble speedo_meter_get_value(SpeedoMeter *meter) {
	SpeedoMeterPrivate *priv=SPEEDO_METER_GET_PRIVATE(meter);
	return priv->value;
	
}



