#ifndef __KNOB_H__
#define __KNOB_H__
#include <gtk/gtk.h>
G_BEGIN_DECLS 

typedef struct _Knob            Knob;
typedef struct _KnobClass       KnobClass;

#define KNOB_TYPE             (knob_get_type ())
#define KNOB(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), KNOB_TYPE, Knob))
#define KNOB_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), KNOB,  KnobClass))
#define KNOB_IS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KNOB))
#define KNOB_IS_FACE_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), KNOB))
#define KNOB_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), KNOB, KnobClass))

 

struct _Knob
{
	GtkDrawingArea parent;
	/* private */
};

struct _KnobClass
{
	GtkDrawingAreaClass parent_class;
	void  (*position_changed)  (Knob *knob,double level);
};

GtkWidget * knob_new (double,double,double,int);
static gboolean knob_expose (GtkWidget *, GdkEventExpose *);
static void knob_class_init (KnobClass *);
static void knob_init (Knob *);
static gboolean knob_expose (GtkWidget *, GdkEventExpose *);
static void draw (GtkWidget *, cairo_t *);
static void knob_increment(GtkWidget *,gpointer);
static void knob_redraw_canvas (Knob *);
static gboolean knob_button_press (GtkWidget *,GdkEventButton *);
static gboolean knob_update (gpointer );
void knob_run_once(GtkWidget *,gpointer);
void knob_set_value(Knob *,gdouble );
void knob_alter_value(Knob *,gdouble);
gdouble knob_get_value(Knob *);
static gboolean knob_button_release (GtkWidget *, GdkEventButton *);
static gboolean knob_motion_notify (GtkWidget *, GdkEventMotion *);
static gboolean knob_scroll_event (GtkWidget *, GdkEventScroll *);
G_END_DECLS
#endif
