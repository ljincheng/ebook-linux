
#ifndef __BOOK_WIDGET_H__
#define __BOOK_WIDGET_H__

#include <webkit2/webkit2.h>
#include <glib-object.h>
#include <glib.h>
#include "book-doc.h"



G_BEGIN_DECLS

#define BOOK_TYPE_WIDGET           (book_widget_get_type ())
#define BOOK_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST (obj, BOOK_TYPE_WIDGET, BookWidget))
#define BOOK_WIDGET_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST (cls, BOOK_TYPE_WIDGET, BookWidgetClass))
#define BOOK_IS_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE (obj, BOOK_TYPE_WIDGET))
#define BOOK_IS_WIDGET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE (obj, BOOK_TYPE_WIDGET))
#define BOOK_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), BOOK_TYPE_WIDGET, BookWidgetClass))

typedef struct _BookWidget      BookWidget;
typedef struct _BookWidgetClass BookWidgetClass;

GType             book_widget_get_type                        (void) G_GNUC_CONST;

GtkWidget        *book_widget_new                             (void);
void              book_widget_set_doc                         (BookWidget *widget,BookDoc *bookDoc);
BookDoc          *book_widget_get_doc                         (BookWidget *widget);
gint              book_widget_load_uri                        (BookWidget *widget, const gchar* uri);

gboolean          book_widget_chapter_next                    (BookWidget *widget);
gboolean          book_widget_chapter_prev                    (BookWidget *widget);
gboolean          book_widget_set_chapter                    (BookWidget *widget,gint chapterIndex);
gint              book_widget_get_chapter                       (BookWidget *widget);
void              book_widget_set_themes                 (BookWidget *widget,     gint   themes);
gint              book_widget_get_themes                 (BookWidget *widget);

gboolean          book_widget_page_next                       (BookWidget *widget);
gboolean          book_widget_page_prev                       (BookWidget *widget);
gfloat            book_widget_get_pos                         (BookWidget *widget);
void              book_widget_set_pos                         (BookWidget *widget,
                                                                gfloat       index);
void              book_widget_splitPage                       (BookWidget *widget,gboolean isSplitPage);


G_END_DECLS

#endif /* __BOOK_WIDGET_H__ */

