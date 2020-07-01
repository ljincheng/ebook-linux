
#ifndef __BOOK_ERM_H__
#define __BOOK_ERM_H__

#include <gtk/gtk.h>



G_BEGIN_DECLS

#define BOOK_TYPE_ERM (book_erm_get_type())
G_DECLARE_FINAL_TYPE (BookErm, book_erm, BOOK, ERM, GtkBox)

GtkWidget*  book_erm_new(GtkWidget* window);

void book_erm_set_accel(BookErm *bookErm,GtkAccelGroup *accelGroup );
G_END_DECLS

#endif
