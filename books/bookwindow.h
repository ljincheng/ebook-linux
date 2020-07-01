#ifndef __BOOKWINDOW_H__
#define __BOOKWINDOW_H__

#include <gtk/gtk.h>

// 打开书籍文档窗口
 GtkWidget * book_open_epub( const gchar *epubfile);

GtkWidget * book_widget( const gchar *epubfile);

 void book_window_page_change(GtkWidget *widget);
#endif
