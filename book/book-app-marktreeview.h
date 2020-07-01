#ifndef __BOOK_APP_MARKTREEVIEW_H
#define __BOOK_APP_MARKTREEVIEW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BOOK_TYPE_APP_MARK_TREEVIEW (book_app_mark_treeview_get_type())

G_DECLARE_FINAL_TYPE (BookAppMarkTreeview, book_app_mark_treeview, BOOK, APP_MARK_TREEVIEW, GtkTreeView)

GtkWidget * book_app_mark_treeview_new(gchar *data);


void book_app_mark_treeview_get_row_flag(GtkTreeModel *model, GtkTreeIter *iter,char *value);

G_END_DECLS


#endif /* __BOOK_APP_MARKTREEVIEW_H */
