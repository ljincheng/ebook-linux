
#ifndef __BOOK_DEV_TABLE_H__
#define __BOOK_DEV_TABLE_H__

#include <gtk/gtk.h>



G_BEGIN_DECLS

#define BOOK_TYPE_DEV_TABLE (book_dev_table_get_type())
G_DECLARE_FINAL_TYPE (BookDevTable, book_dev_table, BOOK, DEV_TABLE, GtkBox)

GtkWidget*  book_dev_table_new(GtkWidget* window);

G_END_DECLS

#endif
