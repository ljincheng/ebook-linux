
#ifndef __BOOK_DEV_BENCH_H__
#define __BOOK_DEV_BENCH_H__

#include <gtk/gtk.h>



G_BEGIN_DECLS

#define BOOK_TYPE_DEV_BENCH (book_dev_bench_get_type())
G_DECLARE_FINAL_TYPE (BookDevBench, book_dev_bench, BOOK, DEV_BENCH, GtkBox)

GtkWidget*  book_dev_bench_new(GtkWidget* window);

G_END_DECLS

#endif
