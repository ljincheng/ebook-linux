
#ifndef __BOOK_WEBVIEW_H__
#define __BOOK_WEBVIEW_H__

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <glib-object.h>
#include <glib.h>
#include "book-doc.h"


G_BEGIN_DECLS

#define BOOK_TYPE_WEBVIEW           (book_webview_get_type ())
#define BOOK_WEBVIEW(obj)           (G_TYPE_CHECK_INSTANCE_CAST (obj, BOOK_TYPE_WEBVIEW, BookWebview))
#define BOOK_WEBVIEW_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST (cls, BOOK_TYPE_WEBVIEW, BookWebviewClass))
#define BOOK_IS_WEBVIEW(obj)        (G_TYPE_CHECK_INSTANCE_TYPE (obj, BOOK_TYPE_WEBVIEW))
#define BOOK_IS_WEBVIEW_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE (obj, BOOK_TYPE_WEBVIEW))
#define BOOK_WEBVIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), BOOK_TYPE_WEBVIEW, BookWebviewClass))

typedef struct _BookWebview      BookWebview;
typedef struct _BookWebviewClass BookWebviewClass;

GType             book_webview_get_type                        (void) G_GNUC_CONST;

GtkWidget        *book_webview_new                             (void);

void book_webview_set_doc(BookWebview *webview, BookDoc *doc);

BookDoc * book_webview_get_doc(BookWebview *webview);

void book_webview_chapter(BookWebview *webview,gint index);

void book_webview_nav_go(BookWebview *webview,const gchar *path);

/**
 * 下一章
 */
void book_webview_chapter_next(BookWebview *webview);

/**
 * 上一章
 */
void book_webview_chapter_prev(BookWebview *webview);

/**
 *下一页
 */
void book_webview_page_next(BookWebview *webview);
/**
 * 上一页
 */
void book_webview_page_prev(BookWebview *webview);

/**
 * 获取当前参数信息
 */
gchar * book_webview_get_info(BookWebview *webview);


void book_webview_set_info(BookWebview *webview,const gchar *info);

gint book_webview_get_pageWidth(BookWebview *webview);
gint book_webview_get_pageHeight(BookWebview *webview);

void book_webview_set_background(BookWebview *webview,gint backgroundStyle);




G_END_DECLS


#endif
