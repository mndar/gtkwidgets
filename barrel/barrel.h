#ifndef __BARREL_H__
#define __BARREL_H__
#include <gtk/gtk.h>
G_BEGIN_DECLS 

typedef struct _Barrel            Barrel;
typedef struct _BarrelClass       BarrelClass;

#define BARREL_TYPE             (barrel_get_type ())
#define BARREL(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), BARREL_TYPE, Barrel))
#define BARREL_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), BARREL,  BarrelClass))
#define BARREL_IS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BARREL))
#define BARREL_IS_FACE_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), BARREL))
#define BARREL_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), BARREL, BarrelClass))

 

struct _Barrel
{
	GtkDrawingArea parent;
	/* private */
};

struct _BarrelClass
{
	GtkDrawingAreaClass parent_class;
	void  (*danger_level)  (Barrel *barrel,double level);
};

GtkWidget * barrel_new (double,double,double,int);
static gboolean barrel_expose (GtkWidget *, GdkEventExpose *);
static void barrel_class_init (BarrelClass *);
static void barrel_init (Barrel *);
static gboolean barrel_expose (GtkWidget *, GdkEventExpose *);
static void draw (GtkWidget *, cairo_t *);
static void barrel_increment(GtkWidget *,gpointer);
static void barrel_redraw_canvas (Barrel *);
static gboolean barrel_update (gpointer );
void barrel_run_once(GtkWidget *,gpointer);
void barrel_set_value(Barrel *,gdouble );
void barrel_alter_value(Barrel *,gdouble);
gdouble barrel_get_value(Barrel *);
G_END_DECLS
#endif
