#ifndef __SPEEDO_METER_H__
#define __SPEEDO_METER_H__
#include <gtk/gtk.h>
G_BEGIN_DECLS 

typedef struct _SpeedoMeter            SpeedoMeter;
typedef struct _SpeedoMeterClass       SpeedoMeterClass;

#define SPEEDO_METER_TYPE             (speedo_meter_get_type ())
#define SPEEDO_METER(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SPEEDO_METER_TYPE, SpeedoMeter))
#define SPEEDO_METER_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), SPEEDO_METER,  SpeedoMeterClass))
#define SPEEDO_METER_IS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SPEEDO_METER))
#define SPEEDO_METER_IS_FACE_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), SPEEDO_METER))
#define SPEEDO_METER_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), SPEEDO_METER, SpeedoMeterClass))

 

struct _SpeedoMeter
{
	GtkDrawingArea parent;

	/* private */
};

struct _SpeedoMeterClass
{
	GtkDrawingAreaClass parent_class;
};

GtkWidget * speedo_meter_new (double,double,double,int);
static gboolean speedo_meter_expose (GtkWidget *, GdkEventExpose *);
static void speedo_meter_class_init (SpeedoMeterClass *);
static void speedo_meter_init (SpeedoMeter *);
static gboolean speedo_meter_expose (GtkWidget *, GdkEventExpose *);
static void draw (GtkWidget *, cairo_t *);
static void speedo_meter_increment(GtkWidget *,gpointer);
static void speedo_meter_redraw_canvas (SpeedoMeter *);
static gboolean speedo_meter_update (gpointer );
void speedo_meter_run_once(GtkWidget *,gpointer);
void speedo_meter_set_value(SpeedoMeter *,gdouble );
void speedo_meter_alter_value(SpeedoMeter *,gdouble);
gdouble speedo_meter_get_value(SpeedoMeter *);
G_END_DECLS
#endif
