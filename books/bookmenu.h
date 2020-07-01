
#ifndef __BOOKMENU_H__
#define __BOOKMENU_H__

#include <gtk/gtk.h>
#include <libmobi/mobi.h>
#include "book-widget.h"


GtkWidget * do_tree_store (GtkWidget *epub_widget);

GtkWidget * book_get_tree_widget (BookWidget *epub_widget);
#endif
