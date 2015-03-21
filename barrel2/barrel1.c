#include "barrel.h"
#include <math.h>
#include <string.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include "barrel-marshallers.h"
#define BARREL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BARREL_TYPE, BarrelPrivate))

G_DEFINE_TYPE (Barrel, barrel, GTK_TYPE_DRAWING_AREA);

typedef struct _BarrelPrivate BarrelPrivate;
const guint8 level_top2_svg[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\
		<!-- Created with Inkscape (http://www.inkscape.org/) -->\
		<svg\
		xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\
		xmlns:cc=\"http://creativecommons.org/ns#\"\
		xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\
		xmlns:svg=\"http://www.w3.org/2000/svg\"\
		xmlns=\"http://www.w3.org/2000/svg\"\
		xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"\
		xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\
		width=\"86\"\
		height=\"11\"\
		id=\"svg3350\"\
		sodipodi:version=\"0.32\"\
		inkscape:version=\"0.46\"\
		version=\"1.0\"\
		sodipodi:docname=\"level_top2.svg\"\
		inkscape:output_extension=\"org.inkscape.output.svg.inkscape\">\
		<defs\
		id=\"defs3352\">\
		<inkscape:perspective\
		sodipodi:type=\"inkscape:persp3d\"\
		inkscape:vp_x=\"0 : 526.18109 : 1\"\
		inkscape:vp_y=\"0 : 1000 : 0\"\
		inkscape:vp_z=\"744.09448 : 526.18109 : 1\"\
		inkscape:persp3d-origin=\"372.04724 : 350.78739 : 1\"\
		id=\"perspective3358\" />\
		</defs>\
		<sodipodi:namedview\
		id=\"base\"\
		pagecolor=\"#ffffff\"\
		bordercolor=\"#666666\"\
		borderopacity=\"1.0\"\
		gridtolerance=\"10000\"\
		guidetolerance=\"10\"\
		objecttolerance=\"10\"\
		inkscape:pageopacity=\"0.0\"\
		inkscape:pageshadow=\"2\"\
		inkscape:zoom=\"10.240342\"\
		inkscape:cx=\"42.902349\"\
		inkscape:cy=\"5.4999997\"\
		inkscape:document-units=\"px\"\
		inkscape:current-layer=\"layer1\"\
		showgrid=\"false\"\
		inkscape:window-width=\"1266\"\
		inkscape:window-height=\"703\"\
		inkscape:window-x=\"0\"\
		inkscape:window-y=\"0\" />\
		<metadata\
		id=\"metadata3355\">\
		<rdf:RDF>\
		<cc:Work\
		rdf:about=\"\">\
		<dc:format>image/svg+xml</dc:format>\
		<dc:type\
		rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\
		</cc:Work>\
		</rdf:RDF>\
		</metadata>\
		<g\
		inkscape:label=\"Layer 1\"\
		inkscape:groupmode=\"layer\"\
		id=\"layer1\">\
		<path\
		sodipodi:type=\"arc\"\
		style=\"opacity:1;color:#000000;fill:#00ff00;fill-opacity:0.5098038;fill-rule:nonzero;stroke:none;stroke-width:1;stroke-linecap:butt;stroke-linejoin:miter;ma\rker:none;marker-start:none;marker-mid:none;marker-end:none;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;visibility:visible;display:inline;overflow:visible;enable-background:accumulate\"\
		id=\"levelTop\"\
		sodipodi:cx=\"-442.44681\"\
		sodipodi:cy=\"-492.65735\"\
		sodipodi:rx=\"234.35539\"\
		sodipodi:ry=\"61.619305\"\
		d=\"M -208.09142,-492.65735 A 234.35539,61.619305 0 1 1 -676.8022,-492.65735 L -442.44681,-492.65735 z\"\
		transform=\"matrix(0.180621,0,0,-0.1766887,122.24467,-76.103269)\"\
		sodipodi:start=\"0\"\
		sodipodi:end=\"3.1415927\" />\
		</g>\
		</svg>";

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

GtkWidget * barrel_new (double startValue,double endValue)
{
	Barrel *barrel;
	BarrelPrivate *priv;
	GtkWidget *widget;
	widget=g_object_new (BARREL_TYPE, NULL);
	barrel=(Barrel *)widget;
	priv=BARREL_GET_PRIVATE(barrel);
	priv->startValue=startValue;
	priv->endValue=endValue;
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
	char text[4];
	double width,height,startX,startY,indicatorSourceX,indicatorSourceY;
	double barrelHeight,barrelWidth;
	double radius,degValue;
	double posX,posY;
	double i,presentDeg,presentVal;
	double markerStartX,markerEndX,markerStartY,markerEndY;
	double fillLevel;
	GError *pError=NULL;
	BarrelPrivate *priv;
  	RsvgHandle *rsvgHandle=rsvg_handle_new_from_file("../images/barrel2.svg",&pError);
 	RsvgHandle *rsvgHandle2=rsvg_handle_new_from_file("../images/level_top2.svg",&pError);
// 	RsvgHandle *rsvgHandle2=rsvg_handle_new_from_data(level_top2_svg,strlen(level_top2_svg),&pError);
	
	priv = BARREL_GET_PRIVATE (barrel);
	if(firstCall==1) {
		firstCall=0;
		priv->value=priv->startValue;
	}	
	barrelWidth=barrel->allocation.width;
	barrelHeight=barrel->allocation.height;
	
	if(priv->value>=(0.5*priv->endValue))
		emit_danger_level_signal((Barrel *)barrel,priv->value);

	fillLevel=110*priv->value/priv->endValue;;
// 	posY=133.592-fillLevel;
	posY=133.200-fillLevel;
	
	cairo_save(cr);
	cairo_scale(cr,barrelWidth/110,barrelHeight/160);
	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#shadow");
 	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#background");
 	posX=13.320	;
	cairo_translate(cr,posX,posY);	
	cairo_rectangle(cr,0,0,84.659,fillLevel);
	cairo_set_source_rgba(cr,0.0,255.0,0.0,80.0/255.0);
	cairo_fill(cr);	
	cairo_translate(cr,-posX,-posY);
		
	cairo_translate(cr,posX,posY-10.923);
	rsvg_handle_render_cairo_sub(rsvgHandle2,cr,"#levelTop");
	cairo_translate(cr,-posX,-posY+10.923);
	
	cairo_move_to(cr,35.016,85.299);
	cairo_set_source_rgb(cr,1.0,1.0,1.0);
	sprintf(text,"%02.0f%c",100*priv->value/priv->endValue,37);
			
	printf("Text:%s\n",text);
	if(!strcmp(text,"100%"))
		strcpy(text,"Max");
	cairo_set_font_size(cr,20.0);
	cairo_text_path(cr,text);
	cairo_fill(cr);
	
	
	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#bottom");
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

