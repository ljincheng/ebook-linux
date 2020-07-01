
#ifndef __BOOK_WINDOW_H__
#define __BOOK_WINDOW_H__

#include <gtk/gtk.h>



G_BEGIN_DECLS

#define BOOK_TYPE_WINDOW (book_window_get_type())
G_DECLARE_FINAL_TYPE (BookWindow, book_window, BOOK, WINDOW, GtkApplicationWindow)

GtkWidget*                book_window_new                  (GtkApplication       *application);

 GtkWidget * book_window_create(gchar *bookFile,GtkWidget * do_widge,const gchar * initInfo);

G_END_DECLS

#endif
