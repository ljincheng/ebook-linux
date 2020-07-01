/* GepubWidget
 *
 * Copyright (C) 2016 Daniel Garcia <danigm@wadobo.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <config.h>
#include <gtk/gtk.h>
#include <JavaScriptCore/JSValueRef.h>
#include <locale.h>
#include "gepub-utils.h"
#include "book-widget.h"
#include "book-mng.h"


struct _BookWidget {
	WebKitWebView parent;
	gint themes;//1 default,2 dark ,3 book
	BookDoc *bookDoc;
	gboolean paginate;
	gint chapter_length;    // real chapter length
	gint chapter_pos;       // position in the chapter, a percentage based on chapter_length
	gint length;
	gint init_chapter_pos;
	gint margin;    // lateral margin in px
	gint font_size; // font size in pt
	gchar *font_family;
	gfloat line_height;
};

struct _BookWidgetClass {
	WebKitWebViewClass parent_class;
};

enum {
	PROP_0,
	PROP_DOC,
	PROP_PAGINATE,
	PROP_CHAPTER,
	PROP_N_CHAPTERS,
	PROP_CHAPTER_POS,
	NUM_PROPS
};

static GParamSpec *properties[NUM_PROPS] = { NULL, };

G_DEFINE_TYPE(BookWidget, book_widget, WEBKIT_TYPE_WEB_VIEW)

#define HUNDRED_PERCENT 100.0

static void
book_scroll_to_chapter_pos(BookWidget *widget)
{

	gchar *script = g_strdup_printf("document.querySelector('body').scrollTo(%d, 0)", widget->chapter_pos);

	webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(widget), script, NULL, NULL, NULL);
	g_free(script);
}

static void
adjust_chapter_pos(BookWidget *widget)
{
	// integer division to make a page start
	gint page = widget->chapter_pos / widget->length;
	gint next = page + 1;
	gint d1 = widget->chapter_pos - (widget->length * page);
	gint d2 = (widget->length * next) - widget->chapter_pos;
  printf("d1=%d,d2=%d,chapter_pos=%d \n\r",d1,d2, widget->chapter_pos);
	if (d1 < d2) {
		widget->chapter_pos = widget->length * page;
	} else {
		widget->chapter_pos = widget->length * next;
	}
	book_scroll_to_chapter_pos(widget);
}

static void
pagination_initialize_finished(GObject      *object,
			       GAsyncResult *result,
			       gpointer user_data)
{

	WebKitJavascriptResult *js_result;
	JSValueRef value;
	JSGlobalContextRef context;
	GError                 *error = NULL;
	BookWidget            *widget = BOOK_WIDGET(user_data);

	js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);
	if (!js_result) {
		g_warning("Error running javascript: %s", error->message);
		g_error_free(error);
		return;
	}

	context = webkit_javascript_result_get_global_context(js_result);
	value = webkit_javascript_result_get_value(js_result);
	if (JSValueIsNumber(context, value)) {
		double n;

		n = JSValueToNumber(context, value, NULL);
		widget->chapter_length = (int)n;
printf("执行pagination_initialize_finished[init_chapter_pos=%d,chapter_pos=%d,chapter_length=%d ]\n",widget->init_chapter_pos,widget->chapter_pos, widget->chapter_length);
		if (widget->init_chapter_pos) {
			widget->chapter_pos = widget->init_chapter_pos * widget->chapter_length / HUNDRED_PERCENT;
			if (widget->chapter_pos > (widget->chapter_length - widget->length)) {
				widget->chapter_pos = (widget->chapter_length - widget->length);
			}
			widget->init_chapter_pos = 0;
		}

		if (widget->chapter_pos) {
			adjust_chapter_pos(widget);
		}
	} else {
		g_warning("Error running javascript: unexpected return value");
	}
	webkit_javascript_result_unref(js_result);
}



static void
book_get_length_finished(GObject      *object,
			 GAsyncResult *result,
			 gpointer user_data)
{
	WebKitJavascriptResult *js_result;
	JSValueRef value;
	JSGlobalContextRef context;
	GError                 *error = NULL;
	BookWidget            *widget = BOOK_WIDGET(user_data);

	js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);
	if (!js_result) {
		g_warning("Error running javascript: %s", error->message);
		g_error_free(error);
		return;
	}

	context = webkit_javascript_result_get_global_context(js_result);
	value = webkit_javascript_result_get_value(js_result);
	if (JSValueIsNumber(context, value)) {
		double n;

		n = JSValueToNumber(context, value, NULL);
		widget->length = (int)n;
	} else {
		g_warning("Error running javascript: unexpected return value");
	}
	webkit_javascript_result_unref(js_result);
}

static void reload_themes(BookWidget *gwidget)
{
	WebKitWebView *web_view = WEBKIT_WEB_VIEW(gwidget);
	gint themes;

	themes = gwidget->themes;

	GdkRGBA rgba;
	rgba.red = 0.0;
	rgba.green = 0.0;
	rgba.blue = 0.0;
	rgba.alpha = 0.6;
	if (themes == 1) {
		webkit_web_view_run_javascript(web_view,
//   "document.body.style.background = '#ffffff';"
					       "document.body.style.color= '#000000';",
					       NULL, NULL, NULL);
		gdk_rgba_parse(&rgba, "#ffffff");
	}else if (themes == 2) {
		webkit_web_view_run_javascript(web_view,
					       "document.body.style.color= '#D3D7CF';",
					       NULL, NULL, NULL);
		gdk_rgba_parse(&rgba, "#2E3436");
	}  else if (themes == 3) {
		webkit_web_view_run_javascript(web_view,
					       "document.body.style.color= '#333';",
					       NULL, NULL, NULL);
		gdk_rgba_parse(&rgba, "#f3eacb");
	}

	webkit_web_view_set_background_color(web_view, &rgba);
}
 

static void
book_reload_length_cb(GtkWidget *widget,
		      GdkRectangle *allocation,
		      gpointer user_data)
{
	BookWidget *gwidget = BOOK_WIDGET(widget);
	WebKitWebView *web_view = WEBKIT_WEB_VIEW(widget);
	int margin, font_size;
	float line_height;
	gchar *script, *font_family;
	gint themes;

	webkit_web_view_run_javascript(web_view,
				       "window.innerWidth",
				       NULL, book_get_length_finished, (gpointer)widget);

	margin = gwidget->margin;
	font_size = gwidget->font_size;
	font_family = gwidget->font_family;
	line_height = gwidget->line_height;
	themes = gwidget->themes;

	reload_themes(gwidget);


	script = g_strdup_printf(
		"if (!document.querySelector('#gepubwrap'))"
		"document.body.innerHTML = '<div id=\"gepubwrap\">' + document.body.innerHTML + '</div>';"
		"document.querySelector('#gepubwrap').style.marginLeft = '%dpx';"
		"document.querySelector('#gepubwrap').style.marginRight = '%dpx';"
		, margin, margin);
	webkit_web_view_run_javascript(web_view, script, NULL, NULL, NULL);
	g_free(script);

	if (font_size) {
		script = g_strdup_printf(
			"document.querySelector('#gepubwrap').style.fontSize = '%dpt';"
			, font_size);
		webkit_web_view_run_javascript(web_view, script, NULL, NULL, NULL);
		g_free(script);
	}

	if (font_family) {
		script = g_strdup_printf(
			"document.querySelector('#gepubwrap').style.fontFamily = '%s';"
			, font_family);
		webkit_web_view_run_javascript(web_view, script, NULL, NULL, NULL);
		g_free(script);
	}

	if (line_height) {
		gchar line_height_buffer[G_ASCII_DTOSTR_BUF_SIZE];

		g_ascii_formatd(line_height_buffer,
				G_ASCII_DTOSTR_BUF_SIZE,
				"%f",
				line_height);
		script = g_strdup_printf(
			"document.querySelector('#gepubwrap').style.lineHeight = %s;"
			, line_height_buffer);
		webkit_web_view_run_javascript(web_view, script, NULL, NULL, NULL);
		g_free(script);
	}

	if (gwidget->paginate) {
		webkit_web_view_run_javascript(web_view,
					      // "document.body.style.overflow = 'hidden';"
					       "document.body.style.margin = '20px 0px 20px 0px';"
					       "document.body.style.padding = '0px';"
					       "document.body.style.columnWidth = window.innerWidth+'px';"
					       "document.body.style.height = (window.innerHeight - 40) +'px';"
					       "document.body.style.columnGap = '60px';"
"document.body.style.columnRule='1px outset #ff0000';"
					       "document.body.scrollWidth",
					       NULL, pagination_initialize_finished, (gpointer)widget);
	}
  gint winWidth, height;
  gtk_widget_get_size_request(widget,&winWidth,&height);
	printf("书窗口宽度=%d:\n",winWidth);
	printf("book_reload_length_cb:\n");
}

static void
book_docready_cb(WebKitWebView  *web_view,
		 WebKitLoadEvent load_event,
		 gpointer user_data)
{
	BookWidget *widget = BOOK_WIDGET(web_view);

	if (load_event == WEBKIT_LOAD_FINISHED) {
		book_reload_length_cb(GTK_WIDGET(widget), NULL, NULL);
	}
//	printf("book_docready_cb:\n");
}

void
book_chapter_uri_callback(WebKitURISchemeRequest *request, gpointer user_data)
{
	GInputStream *stream;
	gchar *uri;
	gchar *mime = NULL;
	BookWidget *widget = user_data;
	GBytes *contents;
	BookData *bookData = NULL;
gchar **arr, *path;
	gint i;

	uri = g_strdup(webkit_uri_scheme_request_get_uri(request));
	 
  
	arr = g_strsplit(uri + 8, "#", -1);
	for (i = 0; arr[i] != NULL; i++) {
		if (i == 0) {
			path = arr[0];
		}
	}

	g_free(arr);
	bookData = book_doc_get_chapter_by_uri(widget->bookDoc, path);

	if (bookData != NULL && bookData->spineIndex >= 0) {
contents=bookData->data;
mime=bookData->mime;
		widget->bookDoc->chapterIndex = bookData->spineIndex;
	}
	g_free(path);
	 




	if (contents == NULL || !contents) {
		contents = g_byte_array_free_to_bytes(g_byte_array_sized_new(0));
		mime = g_strdup("application/octet-stream");
	}
	stream = g_memory_input_stream_new_from_bytes(contents);
	webkit_uri_scheme_request_finish(request, stream, g_bytes_get_size(contents), mime);



	g_object_unref(stream);
	g_bytes_unref(contents);
	g_free(mime);
	g_free(uri);

}

static void
book_resource_callback(WebKitURISchemeRequest *request, gpointer user_data)
{
	GInputStream *stream=NULL;
	gchar *path=NULL;
	gchar *uri=NULL;
	gchar *mime = NULL;
	BookWidget *widget =user_data;
	GBytes *contents=NULL;
	GBytes *htmlContents=NULL;
	BookData *bookData = NULL;

	uri = g_strdup(webkit_uri_scheme_request_get_uri(request));
	// removing "epub:///"
	path = uri + 8;
	//ljc 增加外理地址文件
	gchar **arr;
	gint i;

	arr = g_strsplit(path, "#", -1);
	for (i = 0; arr[i] != NULL; i++) {
		// printf("切path[%d]=%s\n", i,arr[i]);
		if (i == 0) {
			path = arr[0];
		}
	}

	g_free(arr);

	//printf("准备读取地址path=%s,url=%s\n", path, uri);

	bookData = book_doc_get_resource(widget->bookDoc, path);
	if (bookData != NULL) {
		mime = bookData->mime;
		contents = bookData->data;
	}
 
	if (!contents) {
//		printf("%s加载的资源数据是空\n",uri);
		contents = g_byte_array_free_to_bytes(g_byte_array_sized_new(0));
		mime = g_strdup("application/octet-stream");
	}

	if (mime ==NULL || !mime) {
		//printf("%s加载的资源mime是空\n",uri);
		mime = g_strdup("application/octet-stream");
	}
	//printf("load url content ok!,size=%d ,mime=%s \n",g_bytes_get_size(contents),mime);
	stream = g_memory_input_stream_new_from_bytes(contents);
	webkit_uri_scheme_request_finish(request, stream, g_bytes_get_size(contents), mime);



	g_object_unref(stream);
	g_bytes_unref(contents);
	g_free(mime);
	g_free(uri);

	//printf("load url content success.\n");
}

static void
book_widget_set_property(GObject      *object,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec   *pspec)
{
	BookWidget *widget = BOOK_WIDGET(object);

	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void
book_widget_get_property(GObject    *object,
			 guint prop_id,
			 GValue     *value,
			 GParamSpec *pspec)
{
	BookWidget *widget = BOOK_WIDGET(object);

	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void
book_widget_finalize(GObject *object)
{
	BookWidget *widget = BOOK_WIDGET(object);



	G_OBJECT_CLASS(book_widget_parent_class)->finalize(object);
}

static void
book_widget_init(BookWidget *widget)
{

	widget->themes = 1;
	widget->bookDoc = NULL;
	widget->chapter_length = 0;
	widget->paginate = FALSE;
	widget->chapter_pos = 0;
	widget->length = 0;
	widget->init_chapter_pos = 0;
	widget->margin = 20;
	widget->font_size = 0;
	widget->font_family = NULL;
	widget->line_height = 0;

}

static void
book_widget_constructed(GObject *object)
{
	WebKitWebContext *ctx;
	BookWidget *widget = BOOK_WIDGET(object);

	G_OBJECT_CLASS(book_widget_parent_class)->constructed(object);
	ctx = webkit_web_view_get_context(WEBKIT_WEB_VIEW(widget));
	webkit_web_context_register_uri_scheme(ctx, "epub", book_resource_callback, widget, NULL);
	webkit_web_context_register_uri_scheme(ctx, "mobi", book_chapter_uri_callback, widget, NULL);
 	webkit_web_context_register_uri_scheme(ctx, "book", book_webview_resource_callback, widget, NULL);
	g_signal_connect(widget, "load-changed", G_CALLBACK(book_docready_cb), NULL);
	g_signal_connect(widget, "size-allocate", G_CALLBACK(book_reload_length_cb), NULL);
}

static void
book_widget_class_init(BookWidgetClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->constructed = book_widget_constructed;
	object_class->finalize = book_widget_finalize;
	object_class->set_property = book_widget_set_property;
	object_class->get_property = book_widget_get_property;

}

/**
 * gepub_widget_new:
 *
 * Returns: (transfer full): the new GepubWidget created
 */
GtkWidget *
book_widget_new(void)
{
	return g_object_new(BOOK_TYPE_WIDGET,
			    NULL);
}


static void
reload_current_chapter(BookWidget *widget)
{
	GBytes *current;

}



void book_widget_set_html(BookWidget *widget, BookData *bookData)
{


	if (bookData != NULL && bookData->data != NULL) {
//printf("读取内容：");
//printf("长度:%d,mime:%s\n",g_bytes_get_size(bookData->data),bookData->mime);
		webkit_web_view_load_bytes(WEBKIT_WEB_VIEW(widget),
					   bookData->data,
					   bookData->mime,
					   "UTF-8", NULL);
		//  g_bytes_unref (current);
		//book_data_free(bookData);
	}

}

gint book_widget_load_uri(BookWidget *widget, const gchar* uri)
{
 	//widget->init_chapter_pos = HUNDRED_PERCENT;
//printf("直接跳转地址：%s\n",uri);

	gchar *basepath = g_strdup_printf("mobi:///%s", uri);

	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(widget), basepath);

/*
	BookData *bookData = NULL;
	gchar **arr, *path;
	gint i;

	arr = g_strsplit(uri, "#", -1);
	for (i = 0; arr[i] != NULL; i++) {
		if (i == 0) {
			path = arr[0];
		}
	}

	g_free(arr);
	bookData = book_doc_get_chapter_by_uri(widget->bookDoc, path);

	if (bookData != NULL && bookData->spineIndex >= 0) {
		widget->bookDoc->chapterIndex = bookData->spineIndex;
		webkit_web_view_load_bytes (WEBKIT_WEB_VIEW (widget),
                                bookData->data,
                                bookData->mime,
                                "UTF-8", NULL);
		g_bytes_unref (bookData->data);
		g_free( bookData->mime);
	}
	g_free(path);
	 
*/
	return widget->bookDoc->chapterIndex;

//	return 0;
}

void printf_spine(GList * spine)
{
	g_printf("Here's the list:%05zu \n", spine->data);
}

gboolean          book_widget_set_chapter(BookWidget *widget, gint chapterIndex)
{
	//printf("跳到第%d章\n", chapterIndex);
//	GBytes * html = NULL;
	BookData *bookData;
	if (widget->bookDoc != NULL) {
		if (chapterIndex >= 0 && chapterIndex < widget->bookDoc->chapterNum) {
			widget->bookDoc->chapterIndex = chapterIndex;
			bookData = book_doc_get_n_chapter(widget->bookDoc, widget->bookDoc->chapterIndex);
			book_widget_set_html(widget, bookData);
		}
		return FALSE;
	}else{
		return FALSE;
	}
}

BookDoc *book_widget_get_doc(BookWidget *widget)
{

	return widget->bookDoc;
}

void  book_widget_set_doc(BookWidget *widget, BookDoc *bookDoc)
{
	g_return_if_fail(BOOK_IS_WIDGET(widget));
	if (widget->bookDoc == bookDoc)
		return;

	BookData *bookData;
	g_return_if_fail(bookDoc);
	widget->bookDoc = bookDoc;
	//bookData = book_doc_get_n_chapter(bookDoc, bookDoc->chapterIndex);
	//book_widget_set_html(widget, bookData);
	book_widget_set_chapter(widget, 0);
}

gboolean book_widget_chapter_next(BookWidget *widget)
{

	if (widget->bookDoc != NULL) {
		if ( widget->bookDoc->chapterIndex < widget->bookDoc->chapterNum) {
			widget->bookDoc->chapterIndex++;
			book_widget_set_chapter(widget, widget->bookDoc->chapterIndex);
		}
		return FALSE;
	}else{
		return FALSE;
	}

}
gboolean book_widget_chapter_prev(BookWidget *widget)
{
	GBytes * html = NULL;

	if (widget->bookDoc != NULL) {
		if ( widget->bookDoc->chapterIndex > 0) {
			widget->bookDoc->chapterIndex--;
			book_widget_set_chapter(widget, widget->bookDoc->chapterIndex);
			 
		}
		return FALSE;
	}else{
		return FALSE;
	}
}


gint   book_widget_get_chapter(BookWidget *widget)
{
	if (widget->bookDoc != NULL) {
		return widget->bookDoc->chapterIndex;
	}
	return 0;
}
 
void              book_widget_set_themes(BookWidget *widget,     gint themes)
{
	widget->themes = themes;
	reload_themes(widget);
}

gint              book_widget_get_themes                 (BookWidget *widget)
{
	return widget->themes;
}
gboolean book_widget_page_next(BookWidget *widget)
{
  if (widget->paginate)
    {
      widget->chapter_pos = widget->chapter_pos + widget->length;
	    if (widget->chapter_pos > (widget->chapter_length - widget->length)) {
		        widget->chapter_pos =HUNDRED_PERCENT;//(widget->chapter_length - widget->length);
        return book_widget_chapter_next(widget);
	            }

	    book_scroll_to_chapter_pos(widget);
      return true;

  }else{
      return book_widget_chapter_next(widget);
         }

	//return TRUE;
}
gboolean book_widget_page_prev(BookWidget *widget)
{
  if (widget->paginate)
    {
	widget->chapter_pos = widget->chapter_pos - widget->length;

	if (widget->chapter_pos < 0) {
		widget->init_chapter_pos = (widget->chapter_length - widget->length);
		return book_widget_chapter_prev(widget);
	}

	book_scroll_to_chapter_pos(widget);
      return TRUE;
    }else{
      return book_widget_chapter_prev(widget);
    }
	//return TRUE;
}


gfloat book_widget_get_pos (BookWidget *widget){

    if (!widget->chapter_length) {
        return 0;
           }

    return widget->chapter_pos * HUNDRED_PERCENT / (float)(widget->chapter_length);
}
void book_widget_set_pos (BookWidget *widget,gfloat index){
    widget->chapter_pos = index * widget->chapter_length / HUNDRED_PERCENT;
    adjust_chapter_pos (widget);

}

void              book_widget_splitPage                       (BookWidget *widget,gboolean isSplitPage){
  widget->paginate=isSplitPage;
webkit_web_view_reload(widget);

}
