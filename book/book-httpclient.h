
#ifndef __BOOK_HTTPCLIENT_H__
#define __BOOK_HTTPCLIENT_H__

#include <glib-object.h>
#include <glib.h>


G_BEGIN_DECLS


#define BOOK_TYPE_HTTP_CLIENT           (book_http_client_get_type ())
G_DECLARE_FINAL_TYPE (BookHttpClient, book_http_client, BOOK, HTTP_CLIENT, GObject)

BookHttpClient* book_http_client_new(void);

gchar* book_http_client_post(BookHttpClient* httpclient,const char* url,GHashTable * form);


book_http_client_test(void);

G_END_DECLS
#endif /* __BOOK_HTTPCLIENT_H__ */

