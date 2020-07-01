#ifndef __BOOK_APP_WINDOW_H
#define __BOOK_APP_WINDOW_H

#include <gtk/gtk.h>


G_BEGIN_DECLS

static GtkWidget *APPAPINNER;

#define BOOK_TYPE_APP_WINDOW (book_app_window_get_type())
G_DECLARE_FINAL_TYPE (BookAppWindow, book_app_window, BOOK, APP_WINDOW, GtkApplicationWindow)

GtkWidget * book_app_window_new(GtkApplication *application);

GtkWidget * book_appwindow_get_headerBar(BookAppWindow *window);

GtkWidget * book_appwindow_get_markTreeview(BookAppWindow *window);

GtkWidget * book_appwindow_get_mainTreeview(BookAppWindow *window);
GtkWidget * book_appwindow_get_frame(BookAppWindow *window,gint index);

G_END_DECLS

#endif /* __BOOK_APP_WINDOW_H */
