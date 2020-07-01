#include "book-erm.h"
#include <webkit2/webkit2.h>
#include "book-common.h"
#include "book-web-widget.h"

struct _BookErm {
GtkBox parent;
GtkWidget *window;
GtkAccelGroup *accelGroup;
  WebKitWebView *webview;
};


G_DEFINE_TYPE (BookErm, book_erm, GTK_TYPE_BOX)

static WebKitSettings *webkit_settings = NULL;
static gpointer
book_erm_webkit_initsettings (gpointer user_data)
{
  guint i;

  webkit_settings = webkit_settings_new_with_settings ("enable-developer-extras", TRUE,
                                                       "enable-fullscreen", TRUE,
                                                       "enable-javascript", TRUE,
                                                       "enable-dns-prefetching", TRUE,
                                                       "enable-mediasource", TRUE,
                                                       "javascript-can-open-windows-automatically", TRUE,
                                                       NULL);

  webkit_settings_set_hardware_acceleration_policy (webkit_settings, WEBKIT_HARDWARE_ACCELERATION_POLICY_NEVER);

  return webkit_settings;
}

static void
book_erm_uri_changed_cb (WebKitWebView *web_view,
                GParamSpec *spec,
                gpointer data)
{
    book_log_debug("book_erm_uri_changed_cb======================\n");
}

static void book_erm_init (BookErm *self)
{
self->accelGroup=NULL;
}

static void book_erm_finalize (GObject *object)
{
  book_log_debug("[BookBench对象退出清理内存]\n");
   //BookBench *self = BOOK_BENCH(object);

  G_OBJECT_CLASS (book_erm_parent_class)->finalize (object);
}


static void
book_erm_class_init (BookErmClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_erm_finalize;

}

static void
book_erm_show_web_view_cb (WebKitWebView* web_view,GParamSpec *spec,
                gpointer data)
{
   char *uri;
  book_log_debug("book_erm_show_web_view_cb使用中");
    g_object_get (web_view, "uri", &uri, NULL);
  book_log_debug("book_erm_show_web_view_cb:uri=%s\n",uri);
    g_free (uri);

  if (webkit_web_view_is_loading (web_view))
    return;

 GtkWidget*  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
      gtk_widget_set_size_request (window, 800, 350);
  gtk_container_add(GTK_CONTAINER(window), web_view);
gtk_widget_show_all(window);
return TRUE;
}

static WebKitWebView*
book_erm_create_web_view_cb (WebKitWebView* web_view,GtkWidget *window)
{
GtkWidget* new_window;
GtkWidget* scrolled_window;
  book_log_debug("book_erm_create_web_view_cb使用中");
GtkWidget* new_web_view = webkit_web_view_new();

g_signal_connect (G_OBJECT (new_web_view), "web-view-ready", G_CALLBACK (book_erm_show_web_view_cb), NULL);

new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
scrolled_window = gtk_scrolled_window_new(NULL, NULL);
gtk_widget_show(scrolled_window);
gtk_container_add(GTK_CONTAINER(new_window), scrolled_window);
gtk_container_add(GTK_CONTAINER(scrolled_window), new_web_view);

return WEBKIT_WEB_VIEW(new_web_view);
}

static void
book_load_changed_cb (WebKitWebView *web_view,
                 WebKitLoadEvent load_event,
                 gpointer user_data)
{
   char *uri;
  //book_log_debug("book_load_changed_cb使用中");
    g_object_get (web_view, "uri", &uri, NULL);
  //book_log_debug("book_load_changed_cb:uri=%s\n",uri);
    g_free (uri);

  GObject *object = G_OBJECT (web_view);

  g_object_freeze_notify (object);

  switch (load_event) {
  case WEBKIT_LOAD_STARTED: {
    const char *loading_uri = NULL;
    loading_uri = webkit_web_view_get_uri (web_view);
    //g_signal_emit_by_name (view, "new-document-now", loading_uri);
   // book_log_debug("WEBKIT_LOAD_STARTED:uri=%s\n",loading_uri);
    break;
  }
  case WEBKIT_LOAD_REDIRECTED:
    /* TODO: Update the loading uri */
    break;
  case WEBKIT_LOAD_COMMITTED: {
    const char *uri;
    /* Title and location. */
    uri = webkit_web_view_get_uri (web_view);
//book_log_debug("WEBKIT_LOAD_COMMITTED:uri=%s\n",uri);

    break;
  }
  case WEBKIT_LOAD_FINISHED:

    break;
  }

  g_object_thaw_notify (object);
}


static void
book_mouse_target_changed_cb (WebKitWebView *web_view,
                         WebKitHitTestResult *hit_test_result,
                         guint modifiers,
                         gpointer data)
{
  const char *message = NULL;

  if (webkit_hit_test_result_context_is_link (hit_test_result))
    message = webkit_hit_test_result_get_link_uri (hit_test_result);

//book_log_debug("book_mouse_target_changed_cb使用中：msg=%s\n",message);
  //ephy_web_view_set_link_message (web_view, message);
}

static void
book_new_window_cb (WebKitWebView *view,
               WebKitWebView *new_view,
               gpointer user_data)
{
  book_log_debug("book_new_window_cb使用中\n");
  /*
  EphyEmbedContainer *container;

  g_return_if_fail (new_view != NULL);

  container = EPHY_EMBED_CONTAINER (gtk_widget_get_toplevel (GTK_WIDGET (new_view)));
  g_return_if_fail (container != NULL || !gtk_widget_is_toplevel (GTK_WIDGET (container)));

  popups_manager_add_window (view, container);
   */
}


static void
book_mixed_content_detected_cb (WebKitWebView *web_view,
                           WebKitInsecureContentEvent event,
                           gpointer user_data)
{
    book_log_debug("book_mixed_content_detected_cb使用中\n");

}


static void
book_ge_popup_blocked_cb (WebKitWebView *view,
                     const char *url,
                     const char *name,
                     const char *features,
                     gpointer user_data)
{
   book_log_debug("book_ge_popup_blocked_cb:url=%s\n",url);
}

GtkWidget* book_erm_new(GtkWidget* window){
  BookErm *obj;
  obj=g_object_new(BOOK_TYPE_ERM,NULL);
  obj->window=window;

  static GOnce once_init = G_ONCE_INIT;
  obj->webview=book_web_widget_new();
 gtk_container_add (GTK_CONTAINER (obj), obj->webview);
 // g_signal_connect (G_OBJECT (obj->webview), "notify::uri", G_CALLBACK (book_erm_show_web_view_cb), window);

   // g_signal_connect (G_OBJECT (obj->webview), "load-changed", G_CALLBACK (book_load_changed_cb), window);
 //  g_signal_connect (G_OBJECT (obj->webview), "mouse-target-changed", G_CALLBACK (book_mouse_target_changed_cb), window);
  //  g_signal_connect (G_OBJECT (obj->webview), "insecure-content-detected", G_CALLBACK (book_mixed_content_detected_cb), window);
  //  g_signal_connect (G_OBJECT (obj->webview), "new-window", G_CALLBACK (book_new_window_cb),   window);
 //   g_signal_connect (G_OBJECT (obj->webview), "ge_popup_blocked",  G_CALLBACK (book_ge_popup_blocked_cb),  window);

 // g_signal_connect (G_OBJECT (obj->webview), "create", G_CALLBACK (book_erm_create_web_view_cb), window);
  //webkit_web_view_load_uri(WEBKIT_WEB_VIEW(obj->webview), "https://blog.csdn.net/ddl007/article/details/7762137?locationNum=1&fps=1");
  //webkit_web_view_load_uri(WEBKIT_WEB_VIEW(obj->webview), "https://wx.qq.com");
  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(obj->webview), "http://cfb2.qianshenghua.com/platform/main.do");
   gtk_widget_set_hexpand (obj->webview, TRUE);
gtk_widget_set_vexpand (obj->webview, TRUE);
 // book_erm_show_web_view_cb(obj->webview);
  return obj;
}



  void book_erm_set_accel(BookErm *bookErm,GtkAccelGroup *accelGroup ){
if(bookErm->accelGroup==NULL)
      {
        bookErm->accelGroup=accelGroup;
      }
  }
