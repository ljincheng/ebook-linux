#ifndef __BOOK_APP_BOOKINFOTREEVIEW_H
#define __BOOK_APP_BOOKINFOTREEVIEW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BOOK_TYPE_APP_BOOKINFO_TREEVIEW (book_app_bookinfo_treeview_get_type())

G_DECLARE_FINAL_TYPE (BookAppBookinfoTreeview, book_app_bookinfo_treeview, BOOK, APP_BOOKINFO_TREEVIEW, GtkTreeView)

GtkWidget * book_app_bookinfo_treeview_new(GtkWidget *window,GSList *bookInfoList);

GtkWidget * book_app_bookinfo_treeview_reset_list(BookAppBookinfoTreeview *treeview,GSList *bookInfoList);


G_END_DECLS


#endif /* __BOOK_APP_BOOKINFOTREEVIEW_H */
