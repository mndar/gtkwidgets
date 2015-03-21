#include "valuebox.h"
#include <math.h>
#include <string.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include "valuebox-marshallers.h"
#define VALUE_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), VALUE_BOX_TYPE, ValueBoxPrivate))

G_DEFINE_TYPE (ValueBox, value_box, GTK_TYPE_DRAWING_AREA);

typedef struct _ValueBoxPrivate ValueBoxPrivate;

enum
{
	VALUE_CHANGED,
	TITLE_CHANGED,
	LAST_SIGNAL
};

 static guint value_box_class_signals[LAST_SIGNAL] = { 0 };


struct _ValueBoxPrivate
{
	gchar title[7],value[7];	
};

static void value_box_class_init (ValueBoxClass *class)
{
	GtkWidgetClass *widget_class;
	GObjectClass *obj_class;

	obj_class = G_OBJECT_CLASS (class);
	widget_class = GTK_WIDGET_CLASS (class);
	widget_class->expose_event = value_box_expose;
	
	g_type_class_add_private (obj_class, sizeof (ValueBoxPrivate));
	value_box_class_signals[VALUE_CHANGED] = g_signal_new (
			"value-changed",
			G_OBJECT_CLASS_TYPE (obj_class),
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (ValueBoxClass, value_changed),
			NULL, NULL,
			_value_box_marshal_VOID__STRING,			
			G_TYPE_NONE, 1,
			G_TYPE_STRING);
	value_box_class_signals[TITLE_CHANGED] = g_signal_new (
			"title-changed",
			G_OBJECT_CLASS_TYPE (obj_class),
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (ValueBoxClass, title_changed),
			NULL, NULL,
			_value_box_marshal_VOID__STRING,			
			G_TYPE_NONE, 1,
			G_TYPE_STRING);
}

static void emit_value_changed_signal (ValueBox *value_box, gchar* value) {
	 
	  g_signal_emit (value_box,
				value_box_class_signals[VALUE_CHANGED],
				0,value);
 }
 
static void emit_title_changed_signal (ValueBox *value_box, gchar* value) {
	 
	 g_signal_emit (value_box,
			value_box_class_signals[TITLE_CHANGED],
			0,value);
 }
 
static void value_box_init (ValueBox *value_box)
{
	ValueBoxPrivate *priv;	
	priv = VALUE_BOX_GET_PRIVATE (value_box);
	strcpy(priv->value,"");
}

GtkWidget * value_box_new (gchar *title,gchar *value)
{
	ValueBox *value_box;
	ValueBoxPrivate *priv;
	GtkWidget *widget;
	widget=g_object_new (VALUE_BOX_TYPE, NULL);
	value_box=(ValueBox *)widget;
	priv=VALUE_BOX_GET_PRIVATE(value_box);
	strcpy(priv->title,title);
	strcpy(priv->value,value);
	return widget;
}

static gboolean value_box_expose (GtkWidget *value_box, GdkEventExpose *event)
{
	cairo_t *cr;
	/* get a cairo_t */
	cr = gdk_cairo_create (value_box->window);
// 	cairo_set_antialias(cr,CAIRO_ANTIALIAS_GRAY);
	cairo_set_antialias(cr,CAIRO_ANTIALIAS_DEFAULT);
	/* set a clip region for the expose event */
   	cairo_rectangle (cr,event->area.x, event->area.y,event->area.width, event->area.height);
  	cairo_clip (cr);
	draw (value_box, cr);
	cairo_destroy (cr);
	return FALSE;
}

static void draw(GtkWidget *value_box, cairo_t *cr)
{
	static int firstCall=1;
	
	int count=0;
	double width,height,startX,startY,indicatorSourceX,indicatorSourceY;
	double value_boxHeight,value_boxWidth;	
	double posX,posY;	
	double x1,x2,y1,y2;

	GError *pError=NULL;
	ValueBoxPrivate *priv;
  	RsvgHandle *rsvgHandle=rsvg_handle_new_from_file("../images/valuebox.svg",&pError); 	
// 	RsvgHandle *rsvgHandle2=rsvg_handle_new_from_data(level_top2_svg,strlen(level_top2_svg),&pError);
	
	priv = VALUE_BOX_GET_PRIVATE (value_box);
	if(firstCall==1) {
		firstCall=0;
// 		strcpy(priv->value,"");
	}	
	value_boxWidth=value_box->allocation.width;
	value_boxHeight=value_box->allocation.height;
	
	cairo_save(cr);
	cairo_scale(cr,value_boxWidth/50,value_boxHeight/30);
 	rsvg_handle_render_cairo_sub(rsvgHandle,cr,"#background");
	
		
	//write title
	cairo_set_font_size(cr,9.0);
	cairo_text_path(cr,priv->title);
	cairo_path_extents(cr,&x1,&y1,&x2,&y2);
	cairo_new_path(cr);
	cairo_move_to(cr,25.0-(x2-x1)/2,10);
	cairo_set_source_rgba(cr,0x9a/255.0,0xae/255.0,0xde/255.0,0.9);
	cairo_text_path(cr,priv->title);
	cairo_fill(cr);
	
	//write value
	cairo_set_font_size(cr,8.0);
	cairo_text_path(cr,priv->value);
	cairo_path_extents(cr,&x1,&y1,&x2,&y2);
	cairo_new_path(cr);
	cairo_move_to(cr,42.828-(x2-x1),22.591);
// 	cairo_set_source_rgba(cr,0x9a/255.0,0xae/255.0,0xde/255.0,0.9);
	
	cairo_set_source_rgba(cr,0xdc/255.0,0xe3/255.0,0xf6/255.0,1.0);
	cairo_text_path(cr,priv->value);
	cairo_fill(cr);
 	cairo_restore(cr);
}


static void value_box_redraw_canvas (ValueBox *value_box)
{
	GtkWidget *widget;
	GdkRegion *region;

	widget = GTK_WIDGET (value_box);

	if (!widget->window) return;

	region = gdk_drawable_get_clip_region (widget->window);
	/* redraw the cairo canvas completely by exposing it */
	gdk_window_invalidate_region (widget->window, region, TRUE);
	gdk_window_process_updates (widget->window, TRUE);

	gdk_region_destroy (region);
}

void value_box_set_value(ValueBox *valueBox,gchar *value) {	
	ValueBoxPrivate *priv=VALUE_BOX_GET_PRIVATE(valueBox);	
	strcpy(priv->value,value);
	value_box_redraw_canvas (valueBox);
	emit_value_changed_signal(valueBox,priv->value);
}

void value_box_set_title(ValueBox *valueBox,gchar *title) {	
	ValueBoxPrivate *priv=VALUE_BOX_GET_PRIVATE(valueBox);	
	strcpy(priv->title,title);
	value_box_redraw_canvas (valueBox);	
	emit_title_changed_signal(valueBox,priv->title);
}


gchar* value_box_get_value(ValueBox *valueBox) {	
	ValueBoxPrivate *priv=VALUE_BOX_GET_PRIVATE(valueBox);	
	return priv->value;	
}

gchar* value_box_get_title(ValueBox *valueBox) {	
	ValueBoxPrivate *priv=VALUE_BOX_GET_PRIVATE(valueBox);	
	return priv->title;	
}
