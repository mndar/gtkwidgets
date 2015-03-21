#ifndef __THERMOMETER_H__
#define __THERMOMETER_H__
#include <gtk/gtk.h>
G_BEGIN_DECLS 

typedef struct _Thermometer            Thermometer;
typedef struct _ThermometerClass       ThermometerClass;

#define THERMOMETER_TYPE             (thermometer_get_type ())
#define THERMOMETER(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), THERMOMETER_TYPE, Thermometer))
#define THERMOMETER_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), THERMOMETER,  ThermometerClass))
#define THERMOMETER_IS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), THERMOMETER))
#define THERMOMETER_IS_FACE_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), THERMOMETER))
#define THERMOMETER_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), THERMOMETER, ThermometerClass))

 typedef enum {TEMP_CELSIUS,TEMP_FARENHIET}TempType;
struct _Thermometer
{
	GtkDrawingArea parent;
	/* private */
};

struct _ThermometerClass
{
	GtkDrawingAreaClass parent_class;
	void  (*danger_level)  (Thermometer *thermometer,double level);
};

GtkWidget * thermometer_new (double,double,TempType);
static gboolean thermometer_expose (GtkWidget *, GdkEventExpose *);
static void thermometer_class_init (ThermometerClass *);
static void thermometer_init (Thermometer *);
static gboolean thermometer_expose (GtkWidget *, GdkEventExpose *);
static void draw (GtkWidget *, cairo_t *);
static void thermometer_increment(GtkWidget *,gpointer);
static void thermometer_redraw_canvas (Thermometer *);
void thermometer_set_value(Thermometer *,gdouble );
void thermometer_alter_value(Thermometer *,gdouble);
gdouble thermometer_get_value(Thermometer *);
G_END_DECLS
#endif
