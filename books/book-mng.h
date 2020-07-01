
#ifndef __BOOK_MNG_H__
#define __BOOK_MNG_H__

#include <webkit2/webkit2.h>
#include <glib-object.h>
#include <glib.h>
#include "book-doc.h"


void
book_webview_resource_callback(WebKitURISchemeRequest *request, gpointer user_data);

GBytes *
book_data_replace_resources (GBytes *content, const gchar *path);

#endif /* __BOOK_MNG_H__ */

