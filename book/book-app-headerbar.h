#ifndef __BOOK_APP_HEADERBAR_H
#define __BOOK_APP_HEADERBAR_H

#include <gtk/gtk.h>


G_BEGIN_DECLS

#define BOOK_TYPE_APP_HEADERBAR (book_app_headerbar_get_type())

G_DECLARE_FINAL_TYPE (BookAppHeaderbar, book_app_headerbar, BOOK, APP_HEADERBAR, GtkHeaderBar)

GtkWidget * book_app_header_bar_new();

GtkWidget * book_app_header_bar_get_titleBtn(BookAppHeaderbar *headerBar,gint index);

GtkWidget * book_app_header_bar_get_searchEntry(BookAppHeaderbar *headerBar);

G_END_DECLS


#endif /* __BOOK_APP_HEADERBAR_H */
