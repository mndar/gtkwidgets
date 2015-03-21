#ifndef __VALUE_BOX_H__
#define __VALUE_BOX_H__
#include <gtk/gtk.h>
G_BEGIN_DECLS 

typedef struct _ValueBox            ValueBox;
typedef struct _ValueBoxClass       ValueBoxClass;

#define VALUE_BOX_TYPE             (value_box_get_type ())
#define VALUE_BOX(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), VALUE_BOX_TYPE, ValueBox))
#define VALUE_BOX_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), VALUE_BOX,  ValueBoxClass))
#define VALUE_BOX_IS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALUE_BOX))
#define VALUE_BOX_IS_FACE_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), VALUE_BOX))
#define VALUE_BOX_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), VALUE_BOX, ValueBoxClass))

 
struct _ValueBox
{
	GtkDrawingArea parent;
	/* private */
};

struct _ValueBoxClass
{
	GtkDrawingAreaClass parent_class;
	void  (*value_changed)  (ValueBox *value_box,gchar *value);
	void  (*title_changed)  (ValueBox *value_box,gchar* title);
};

GtkWidget * value_box_new (gchar *,gchar *);
static gboolean value_box_expose (GtkWidget *, GdkEventExpose *);
static void value_box_class_init (ValueBoxClass *);
static void value_box_init (ValueBox *);
static gboolean value_box_expose (GtkWidget *, GdkEventExpose *);
static void draw (GtkWidget *, cairo_t *);
static void value_box_redraw_canvas (ValueBox *);
void value_box_set_value(ValueBox *,gchar *);
void value_box_set_title(ValueBox *,gchar *);
gchar* value_box_get_value(ValueBox *);
gchar* value_box_get_title(ValueBox *);
G_END_DECLS
#endif
