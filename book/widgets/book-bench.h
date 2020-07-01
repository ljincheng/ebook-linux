
#ifndef __BOOK_BENCH_H__
#define __BOOK_BENCH_H__

#include <gtk/gtk.h>



G_BEGIN_DECLS

#define BOOK_TYPE_BENCH (book_bench_get_type())
G_DECLARE_FINAL_TYPE (BookBench, book_bench, BOOK, BENCH, GtkBox)

GtkWidget*  book_bench_new(GtkWidget* window);

void book_bench_query_key(BookBench *bookBench,const gchar * keyStr);

void book_bench_set_accel(BookBench *bookBench,GtkAccelGroup *accelGroup );
G_END_DECLS

#endif
