#include "book-web-widget.h"
#include "book-common.h"
#include <libsoup/soup.h>
#include "book-download.h"

struct _BookWebWidget{
  WebKitWebView parent_instance;
  WebKitWebContext *web_context;
    char *link_message;
  gboolean reader_active;
};

G_DEFINE_TYPE (BookWebWidget, book_web_widget, WEBKIT_TYPE_WEB_VIEW)

static WebKitSettings *book_webkit_settings = NULL;
static gpointer
book_web_widget_webkit_settings (gpointer user_data)
{
  guint i;

  book_webkit_settings = webkit_settings_new_with_settings ("enable-developer-extras", TRUE,
                                                       "enable-fullscreen", TRUE,
                                                       "enable-javascript", TRUE,
                                                       "enable-dns-prefetching", TRUE,
                                                       "enable-mediasource", TRUE,
                                                       "javascript-can-open-windows-automatically", TRUE,
                                                       NULL);

  webkit_settings_set_hardware_acceleration_policy (book_webkit_settings, WEBKIT_HARDWARE_ACCELERATION_POLICY_NEVER);

  return book_webkit_settings;
}


static void
download_response_changed_cb (WebKitDownload *wk_download,
                              GParamSpec     *spec,
                              gpointer user_data)
{
  WebKitURIResponse *response;
  const char *mime_type;

  response = webkit_download_get_response (wk_download);
  mime_type = webkit_uri_response_get_mime_type (response);
  book_log_debug("download_response_changed_cb,mime_type=%s\n",mime_type);
  if (!mime_type)
    return;


}

static void
book_web_widget_download_started_cb (WebKitWebContext *web_context,
                     WebKitDownload   *download,
                     gpointer             data)
{
     char *filename;
    char *message;
book_log_debug("|book-web-widget|book_web_widget_download_started_cb\n");

book_download_new(download);

}

static gboolean
book_web_widget_key_press_event (GtkWidget *widget, GdkEventKey *event)
{
 // EphyWebView *web_view = EPHY_WEB_VIEW (widget);
  gboolean key_handled = FALSE;

  key_handled = GTK_WIDGET_CLASS (book_web_widget_parent_class)->key_press_event (widget, event);
book_log_debug("|book-web-widget|book_web_widget_key_press_event,key_handled=%d\n",key_handled);
  if (key_handled)
    return TRUE;

  //g_signal_emit_by_name (web_view, "search-key-press", event, &key_handled);

  return key_handled;
}

static gboolean
book_web_widget_button_press_event (GtkWidget *widget, GdkEventButton *event)
{
  /* These are the special cases WebkitWebView doesn't handle but we have an
   * interest in handling. */
book_log_debug("|book-web-widget|book_web_widget_button_press_event,event->button=%d\n",event->button);
  /* Handle typical back/forward mouse buttons. */
  if (event->button == 8) {
    webkit_web_view_go_back (WEBKIT_WEB_VIEW (widget));
    return TRUE;
  }

  if (event->button == 9) {
    webkit_web_view_go_forward (WEBKIT_WEB_VIEW (widget));
    return TRUE;
  }
  if(event->button ==1)
    {
      BookWebWidget *web_view = BOOK_WEB_WIDGET (widget);
      if(web_view->link_message!=NULL && (!g_str_has_prefix (web_view->link_message, "javascript:")) )
        {
     // book_web_widget_load_url(web_view, web_view->link_message);
      //return TRUE;
        }
    }
  gboolean parentRet=GTK_WIDGET_CLASS (book_web_widget_parent_class)->button_press_event (widget, event);
  book_log_debug("|book-web-widget|book_web_widget_button_press_event,parentRet=%d\n",parentRet);
  return parentRet;
}


static void
book_web_widget_mouse_target_changed_cb (BookWebWidget         *web_view,
                         WebKitHitTestResult *hit_test_result,
                         guint                modifiers,
                         gpointer             data)
{
  const char *message = NULL;

  if (webkit_hit_test_result_context_is_link (hit_test_result))
    message = webkit_hit_test_result_get_link_uri (hit_test_result);

  book_log_debug("|book-web-widget|book_web_widget_mouse_target_changed_cb,message=%s\n",message);
  //ephy_web_view_set_link_message (web_view, message);
  g_free (web_view->link_message);
  if(message)
  web_view->link_message=g_strdup(message);
  else{
      web_view->link_message=NULL;
  }


}

static void
book_web_widget_init (BookWebWidget *web_view)
{
   //web_view->web_context = webkit_web_context_new_ephemeral ();

   g_signal_connect (web_view, "mouse-target-changed",
                    G_CALLBACK (book_web_widget_mouse_target_changed_cb),
                    NULL);

  /*
  g_signal_connect (EPHY_SETTINGS_READER, "changed::" EPHY_PREFS_READER_FONT_STYLE,
                    G_CALLBACK (reader_setting_changed_cb),
                    web_view);

  g_signal_connect (EPHY_SETTINGS_READER, "changed::" EPHY_PREFS_READER_COLOR_SCHEME,
                    G_CALLBACK (reader_setting_changed_cb),
                    web_view);

  g_signal_connect (web_view, "decide-policy",
                    G_CALLBACK (decide_policy_cb),
                    NULL);

  g_signal_connect (web_view, "permission-request",
                    G_CALLBACK (permission_request_cb),
                    NULL);

  g_signal_connect (web_view, "load-changed",
                    G_CALLBACK (load_changed_cb),
                    NULL);

  g_signal_connect (web_view, "close",
                    G_CALLBACK (close_web_view_cb),
                    NULL);
  g_signal_connect (web_view, "load-failed",
                    G_CALLBACK (load_failed_cb),
                    NULL);

  g_signal_connect (web_view, "load-failed-with-tls-errors",
                    G_CALLBACK (load_failed_with_tls_error_cb),
                    NULL);

  g_signal_connect (web_view, "insecure-content-detected",
                    G_CALLBACK (mixed_content_detected_cb),
                    NULL);

  g_signal_connect (web_view, "notify::zoom-level",
                    G_CALLBACK (zoom_changed_cb),
                    NULL);

  g_signal_connect (web_view, "notify::title",
                    G_CALLBACK (title_changed_cb),
                    NULL);

  g_signal_connect (web_view, "notify::uri",
                    G_CALLBACK (uri_changed_cb),
                    NULL);



  g_signal_connect (web_view, "notify::favicon",
                    G_CALLBACK (icon_changed_cb),
                    NULL);

  g_signal_connect (web_view, "script-dialog",
                    G_CALLBACK (script_dialog_cb),
                    NULL);

  g_signal_connect (web_view, "new-window",
                    G_CALLBACK (new_window_cb),
                    NULL);

  g_signal_connect_object (ephy_embed_shell_get_default (), "page-created",
                           G_CALLBACK (page_created_cb),
                           web_view, 0);
   */
}

static void
book_web_widget_finalize (GObject *object)
{
  	BookWebWidget *widget = BOOK_WEB_WIDGET(object);
  if(widget->web_context!=NULL)
    g_clear_object (&widget->web_context);
  if(widget->link_message !=NULL)
    g_free(widget->link_message);
}

static void
book_web_widget_constructed (GObject *object)
{
  BookWebWidget *web_view = BOOK_WEB_WIDGET (object);

  G_OBJECT_CLASS (book_web_widget_parent_class)->constructed (object);

  //g_signal_emit_by_name (ephy_embed_shell_get_default (), "web-view-created", web_view);

  //g_signal_connect (web_view, "web-process-terminated",  G_CALLBACK (process_terminated_cb), NULL);
 // g_signal_connect_swapped (webkit_web_view_get_back_forward_list (WEBKIT_WEB_VIEW (web_view)),  "changed", G_CALLBACK (update_navigation_flags), web_view);
}

static void book_web_widget_class_init (BookWebWidgetClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  WebKitWebViewClass *webkit_webview_class = WEBKIT_WEB_VIEW_CLASS (klass);

  gobject_class->finalize = book_web_widget_finalize;
  gobject_class->constructed = book_web_widget_constructed;

  widget_class->button_press_event = book_web_widget_button_press_event;
  widget_class->key_press_event = book_web_widget_key_press_event;

//  webkit_webview_class->run_file_chooser = ephy_web_view_run_file_chooser;

}


char *
book_web_widget_normalize_address (const char *address)
{
  char *effective_address = NULL;

  g_assert (address);


  if (!book_web_widget_address_has_web_scheme (address)) {
    SoupURI *uri;

    uri = soup_uri_new (address);

    /* Auto-prepend http:// to anything that is not
     * one according to soup, because it probably will be
     * something like "google.com". Special case localhost(:port)
     * and IP(:port), because SoupURI, correctly, thinks it is a
     * URI with scheme being localhost/IP and, optionally, path
     * being the port. Ideally we should check if we have a
     * handler for the scheme, and since we'll fail for localhost
     * and IP, we'd fallback to loading it as a domain. */
    if (!uri ||
        (uri && !g_strcmp0 (uri->scheme, "localhost")) ||
        (uri && g_hostname_is_ip_address (uri->scheme)))
      effective_address = g_strconcat ("http://", address, NULL);

    if (uri)
      soup_uri_free (uri);
  }

  return effective_address ? effective_address : g_strdup (address);
}

void
ephy_web_view_load_request (BookWebWidget      *view,
                            WebKitURIRequest *request)
{
  const char *url;
  char *effective_url;

  g_assert (BOOK_IS_WEB_WIDGET (view));
  g_assert (WEBKIT_IS_URI_REQUEST (request));

  url = webkit_uri_request_get_uri (request);
  effective_url = book_web_widget_normalize_address (url);

  webkit_uri_request_set_uri (request, effective_url);
  g_free (effective_url);

  webkit_web_view_load_request (WEBKIT_WEB_VIEW (view), request);
}

static gboolean
book_web_widget_decide_navigation_policy (WebKitWebView            *web_view,
                          WebKitPolicyDecision     *decision,
                          WebKitPolicyDecisionType  decision_type,
                          BookWebWidget               *window)
{
  WebKitNavigationPolicyDecision *navigation_decision;
  WebKitNavigationAction *navigation_action;
  WebKitNavigationType navigation_type;
  WebKitURIRequest *request;
  const char *uri;

  g_assert (WEBKIT_IS_WEB_VIEW (web_view));
  g_assert (WEBKIT_IS_NAVIGATION_POLICY_DECISION (decision));
  g_assert (decision_type != WEBKIT_POLICY_DECISION_TYPE_RESPONSE);

  navigation_decision = WEBKIT_NAVIGATION_POLICY_DECISION (decision);
  navigation_action = webkit_navigation_policy_decision_get_navigation_action (navigation_decision);
  request = webkit_navigation_action_get_request (navigation_action);
  uri = webkit_uri_request_get_uri (request);

  if (navigation_type == WEBKIT_NAVIGATION_TYPE_LINK_CLICKED) {
    gint button;
    gint state;
    gboolean inherit_session = FALSE;

    button = webkit_navigation_action_get_mouse_button (navigation_action);
    state = webkit_navigation_action_get_modifiers (navigation_action);
    book_log_debug ("|book-web-widget|book_web_widget_decide_navigation_policy,button=%d,state=%d,uri=%s\n",button,state,uri);

  }else{
    book_log_debug ("====================================:navigation_type=%d\n",navigation_type);
   //  ephy_web_view_load_request (web_view, request);

   // webkit_policy_decision_ignore (decision);
   // return TRUE;
  }
        return FALSE;
}
static gboolean
book_web_widget_decide_policy_cb (WebKitWebView           *web_view,
                  WebKitPolicyDecision    *decision,
                  WebKitPolicyDecisionType decision_type,
                  BookWebWidget              *window)
{
  WebKitNavigationPolicyDecision *navigation_decision;
  WebKitNavigationAction *navigation_action;
  WebKitURIRequest *request;
  const char *request_uri;

  if (decision_type != WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION &&
      decision_type != WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION)
    return FALSE;

  navigation_decision = WEBKIT_NAVIGATION_POLICY_DECISION (decision);
  navigation_action = webkit_navigation_policy_decision_get_navigation_action (navigation_decision);
  request = webkit_navigation_action_get_request (navigation_action);
  request_uri = webkit_uri_request_get_uri (request);
book_log_debug ("|book-web-widget|decide_policy_cb,request_uri=%s\n",request_uri);
  return book_web_widget_decide_navigation_policy (web_view, decision, decision_type, window);
}

GtkWidget        *book_web_widget_new(void){

WebKitWebContext *webContext;
  GtkWidget * web_view;
  WebKitSettings *settings;
  webContext= webkit_web_context_new_ephemeral ();
  settings=book_web_widget_get_settings ();
  web_view= g_object_new (BOOK_TYPE_WEB_WIDGET,
                       "web-context", webContext,
                 //      "user-content-manager", ephy_embed_shell_get_user_content_manager (shell),
                       "settings",settings,
                       NULL);

  g_signal_connect (webContext,
                    "download-started",
                    G_CALLBACK (book_web_widget_download_started_cb),
                    web_view);
    g_signal_connect_object (web_view, "decide-policy",
                           G_CALLBACK (book_web_widget_decide_policy_cb),
                           web_view, 0);
  gchar *user_agent=webkit_settings_get_user_agent(settings);
  book_log_debug("|book-web-widget|book_web_widget_new,user_agent=%s\n",user_agent);
  return web_view;
}


WebKitSettings *
book_web_widget_get_settings (void)
{
  static GOnce once_init = G_ONCE_INIT;

  return g_once (&once_init, book_web_widget_webkit_settings, NULL);
}


gboolean
book_web_widget_address_has_web_scheme (const char *address)
{
  gboolean has_web_scheme;
  int colonpos;

  if (address == NULL)
    return FALSE;

  colonpos = (int)((g_strstr_len (address, 12, ":")) - address);

  if (colonpos < 0)
    return FALSE;

  has_web_scheme = !(g_ascii_strncasecmp (address, "http", colonpos) &&
                     g_ascii_strncasecmp (address, "https", colonpos) &&
                     g_ascii_strncasecmp (address, "ftp", colonpos) &&
                     g_ascii_strncasecmp (address, "file", colonpos) &&
                     g_ascii_strncasecmp (address, "javascript", colonpos) &&
                     g_ascii_strncasecmp (address, "data", colonpos) &&
                     g_ascii_strncasecmp (address, "blob", colonpos) &&
                     g_ascii_strncasecmp (address, "about", colonpos) &&
                     g_ascii_strncasecmp (address, "ephy-about", colonpos) &&
                     g_ascii_strncasecmp (address, "ephy-source", colonpos) &&
                     g_ascii_strncasecmp (address, "gopher", colonpos) &&
                     g_ascii_strncasecmp (address, "inspector", colonpos));

  return has_web_scheme;
}


void  book_web_widget_load_url (BookWebWidget *view,  const char  *url){
 char *effective_url;

  g_assert (BOOK_IS_WEB_WIDGET (view));
  g_assert (url);

  view->reader_active = FALSE;

  effective_url = book_web_widget_normalize_address (url);
  book_log_debug("|book-web-widget|book_web_widget_load_url,effective_url=%s\n",effective_url);
  if (g_str_has_prefix (effective_url, "javascript:")) {
    char *decoded_url;

    decoded_url = soup_uri_decode (effective_url);
    book_log_debug("|book-web-widget|book_web_widget_load_url,decoded_url=%s\n",decoded_url);
    webkit_web_view_run_javascript (WEBKIT_WEB_VIEW (view), decoded_url, NULL, NULL, NULL);
    g_free (decoded_url);
  } else
    {
       const char *url = webkit_web_view_get_uri (WEBKIT_WEB_VIEW (view));
      gint urlLen=strlen(url);
if(g_str_has_prefix (effective_url,url) )
        {
          if(urlLen!=strlen(effective_url))
                              {
              for (gssize i = strlen (effective_url) - 1; i >= 0 && effective_url[i] == '#'; i--)
            effective_url[i] = '\0';
                                gint newUrlLen=strlen(effective_url);
            if(urlLen!=newUrlLen && newUrlLen>0)
                webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view), effective_url);
                              }

        }else{
          webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view), effective_url);
         }

    }


  g_free (effective_url);
}
