
#ifndef __BOOK_HTTP_RES_H__
#define __BOOK_HTTP_RES_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS




#define BOOK_TYPE_HTTP_RES (book_http_res_get_type ())
G_DECLARE_FINAL_TYPE (BookHttpRes, book_http_res, BOOK, HTTP_RES, GObject)




BookHttpRes* book_http_res_new(void);

gchar* book_http_res_get_code(BookHttpRes* res);
gchar* book_http_res_get_msg(BookHttpRes* res);

G_END_DECLS
#endif /* __BOOK_HTTP_RES_H__ */
