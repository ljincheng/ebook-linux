#ifndef __BOOK_DEV_TOOLLIST_H
#define __BOOK_DEV_TOOLLIST_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BOOK_TYPE_DEV_TOOLLIST (book_dev_toollist_get_type())

G_DECLARE_FINAL_TYPE (BookDevToollist, book_dev_toollist, BOOK, DEV_TOOLLIST, GtkTreeView)

GtkWidget * book_dev_toollist_new(void);



G_END_DECLS


#endif /* __BOOK_DEV_TOOLLIST_H */
