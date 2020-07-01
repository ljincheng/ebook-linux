
#ifndef __BOOK_WEB_WIDGET_H__
#define __BOOK_WEB_WIDGET_H__

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <glib-object.h>
#include <glib.h>


G_BEGIN_DECLS

#define BOOK_TYPE_WEB_WIDGET (book_web_widget_get_type ())

G_DECLARE_FINAL_TYPE (BookWebWidget, book_web_widget, BOOK, WEB_WIDGET, WebKitWebView)


GtkWidget        *book_web_widget_new(void);
WebKitSettings * book_web_widget_get_settings (void);
void  book_web_widget_load_url (BookWebWidget *view,  const char  *url);

G_END_DECLS


#endif
