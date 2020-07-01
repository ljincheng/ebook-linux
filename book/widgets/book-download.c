/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 *  Copyright © 2011 Igalia S.L.
 *
 *  This file is part of Epiphany.
 *
 *  Epiphany is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Epiphany is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Epiphany.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "book-download.h"

#include <errno.h>
#include <glib/gi18n.h>
#include <string.h>

struct _BookDownload {
  GObject parent_instance;

  WebKitDownload *download;

  char *destination;
  char *content_type;

  gboolean show_notification;

  BookDownloadActionType action;
  guint32 start_time;
  gboolean finished;
  GError *error;
};

G_DEFINE_TYPE (BookDownload, book_download, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_DOWNLOAD,
  PROP_DESTINATION,
  PROP_ACTION,
  PROP_START_TIME,
  PROP_CONTENT_TYPE,
  LAST_PROP
};

static GParamSpec *obj_properties[LAST_PROP];

enum {
  FILENAME_SUGGESTED,
  ERROR,
  COMPLETED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

static void
book_download_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  BookDownload *download = BOOK_DOWNLOAD (object);

  switch (property_id) {
    case PROP_DOWNLOAD:
     // g_value_set_object (value, book_download_get_webkit_download (download));
      break;
    case PROP_DESTINATION:
     // g_value_set_string (value, book_download_get_destination_uri (download));
      break;
    case PROP_ACTION:
     // g_value_set_enum (value, book_download_get_action (download));
      break;
    case PROP_START_TIME:
     // g_value_set_uint (value, book_download_get_start_time (download));
      break;
    case PROP_CONTENT_TYPE:
     // g_value_set_string (value, book_download_get_content_type (download));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
book_download_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  BookDownload *download;
  download = BOOK_DOWNLOAD (object);

  switch (property_id) {
    case PROP_DESTINATION:
      book_download_set_destination_uri (download, g_value_get_string (value));
      break;
    case PROP_ACTION:
      book_download_set_action (download, g_value_get_enum (value));
      break;
    case PROP_DOWNLOAD:
    case PROP_START_TIME:
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

/**
 * book_download_get_content_type:
 * @download: an #BookDownload
 *
 * Gets content-type information for @download. If the server didn't
 * provide a content type, the destination file is queried.
 *
 * Returns: content-type for @download
 **/
const char *
book_download_get_content_type (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  return download->content_type;
}

/* From the old embed/mozilla/MozDownload.cpp */
static const char *
file_is_compressed (const char *filename)
{
  int i;
  static const char * const compression[] = { ".gz", ".bz2", ".Z", ".lz", ".xz", NULL };

  for (i = 0; compression[i] != NULL; i++) {
    if (g_str_has_suffix (filename, compression[i]))
      return compression[i];
  }

  return NULL;
}

static const char *
parse_extension (const char *filename)
{
  const char *compression;
  const char *last_separator;

  compression = file_is_compressed (filename);

  /* if the file is compressed we might have a double extension */
  if (compression != NULL) {
    int i;
    static const char * const extensions[] = { "tar", "ps", "xcf", "dvi", "txt", "text", NULL };

    for (i = 0; extensions[i] != NULL; i++) {
      char *suffix;
      suffix = g_strdup_printf (".%s%s", extensions[i], compression);

      if (g_str_has_suffix (filename, suffix)) {
        char *p;

        p = g_strrstr (filename, suffix);
        g_free (suffix);

        return p;
      }

      g_free (suffix);
    }
  }

  /* no compression, just look for the last dot in the filename */
  last_separator = strrchr (filename, G_DIR_SEPARATOR);
  return strrchr ((last_separator) ? last_separator : filename, '.');
}

static char *
book_file_tmp_filename (const char *base,
                        const char *extension)
{
  int fd;
  char *name = g_strdup (base);

  fd = g_mkstemp (name);

  if (fd != -1) {
    unlink (name);
    close (fd);
  } else {
    g_free (name);

    return NULL;
  }

  if (extension) {
    char *tmp;
    tmp = g_strconcat (name, ".",
                       extension, NULL);
    g_free (name);
    name = tmp;
  }

  return name;
}



/**
 * book_download_set_destination_uri:
 * @download: an #BookDownload
 * @destination: URI where to save @download
 *
 * Sets the destination URI of @download. It must be a proper URI, with a
 * scheme like file:/// or similar.
 **/
void
book_download_set_destination_uri (BookDownload *download,
                                   const char   *destination)
{
  g_assert (BOOK_IS_DOWNLOAD (download));
  g_assert (destination != NULL);

  webkit_download_set_destination (download->download, destination);
  g_object_notify_by_pspec (G_OBJECT (download), obj_properties[PROP_DESTINATION]);
}

/**
 * book_download_set_action:
 * @download: an #BookDownload
 * @action: #BookDownloadActionType to execute
 *
 * Sets the @action to be executed when book_download_do_download_action () is
 * called on @download or on finish when "Automatically download and open
 * files" is set.
 **/
void
book_download_set_action (BookDownload          *download,
                          BookDownloadActionType action)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  download->action = action;
  g_object_notify_by_pspec (G_OBJECT (download), obj_properties[PROP_ACTION]);
}

/**
 * book_download_get_webkit_download:
 * @download: an #BookDownload
 *
 * Gets the #WebKitDownload being wrapped by @download.
 *
 * Returns: (transfer none): a #WebKitDownload.
 **/
WebKitDownload *
book_download_get_webkit_download (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  return download->download;
}

/**
 * book_download_get_destination_uri:
 * @download: an #BookDownload
 *
 * Gets the destination URI where the download is being saved.
 *
 * Returns: (transfer none): destination URI.
 **/
const char *
book_download_get_destination_uri (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  return webkit_download_get_destination (download->download);
}

/**
 * book_download_get_action:
 * @download: an #BookDownload
 *
 * Gets the #BookDownloadActionType that this download will execute when
 * book_download_do_download_action () is called on it. This action is
 * performed automatically is "Automatically download and open files" is
 * enabled.
 *
 * Returns: the #BookDownloadActionType to be executed
 **/
BookDownloadActionType
book_download_get_action (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  return download->action;
}

/**
 * book_download_get_start_time:
 * @download: an #BookDownload
 *
 * Gets the time (returned by gtk_get_current_event_time ()) when @download was
 * started. Defaults to 0.
 *
 * Returns: the time when @download was started.
 **/
guint32
book_download_get_start_time (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  return download->start_time;
}

/**
 * book_download_cancel:
 * @download: an #BookDownload
 *
 * Cancels the wrapped #WebKitDownload.
 **/
void
book_download_cancel (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  webkit_download_cancel (download->download);
}

gboolean
book_download_is_active (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  return !download->finished;
}

gboolean
book_download_succeeded (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  return download->finished && !download->error;
}

gboolean
book_download_failed (BookDownload *download,
                      GError      **error)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  if (download->finished && download->error) {
    if (error)
      *error = download->error;
    return TRUE;
  }

  return FALSE;
}

/**
 * book_download_do_download_action:
 * @download: an #BookDownload
 * @action: one of #BookDownloadActionType
 * @user_time: GDK timestamp, for focus-stealing prevention
 *
 * Executes the given @action for @download, this can be any of
 * #BookDownloadActionType.
 *
 * Returns: %TRUE if the action was executed succesfully.
 *
 **/
gboolean
book_download_do_download_action (BookDownload          *download,
                                  BookDownloadActionType action,
                                  guint32                user_time)
{
  GFile *destination;
  const char *destination_uri;
  gboolean ret = FALSE;

  destination_uri = webkit_download_get_destination (download->download);
  destination = g_file_new_for_uri (destination_uri);
/*
  switch ((action ? action : download->action)) {
    case BOOK_DOWNLOAD_ACTION_BROWSE_TO:
      LOG ("book_download_do_download_action: browse_to");
      ret = book_file_browse_to (destination, user_time);
      break;
    case BOOK_DOWNLOAD_ACTION_OPEN:
      LOG ("book_download_do_download_action: open");
      ret = book_embed_shell_launch_handler (book_embed_shell_get_default (),
                                             destination, NULL, user_time);
      if (!ret)
        ret = book_file_browse_to (destination, user_time);
      break;
    case BOOK_DOWNLOAD_ACTION_NONE:
      LOG ("book_download_do_download_action: none");
      ret = TRUE;
      break;
    default:
      g_assert_not_reached ();
  }
 * */
  ret = TRUE;
  g_object_unref (destination);

  return ret;
}

static void
book_download_dispose (GObject *object)
{
  BookDownload *download = BOOK_DOWNLOAD (object);

 // LOG ("BookDownload disposed %p", object);

  if (download->download) {
    g_signal_handlers_disconnect_matched (download->download, G_SIGNAL_MATCH_DATA, 0, 0, 0, 0, download);
    g_object_unref (download->download);
    download->download = NULL;
  }

  g_clear_error (&download->error);
  g_clear_pointer (&download->content_type, g_free);

  G_OBJECT_CLASS (book_download_parent_class)->dispose (object);
}

static void
book_download_class_init (BookDownloadClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = book_download_get_property;
  object_class->set_property = book_download_set_property;
  object_class->dispose = book_download_dispose;

  /**
   * BookDownload::download:
   *
   * Internal WebKitDownload.
   */
  obj_properties[PROP_DOWNLOAD] =
    g_param_spec_object ("download",
                         "Internal WebKitDownload",
                         "The WebKitDownload used internally by BookDownload",
                         WEBKIT_TYPE_DOWNLOAD,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_STRINGS);

  /**
   * BookDownload::destination:
   *
   * The destination URI where to store the download.
   */
  obj_properties[PROP_DESTINATION] =
    g_param_spec_string ("destination",
                         "Destination",
                         "Destination file URI",
                         NULL,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_STRINGS);

  /**
   * BookDownload::action:
   *
   * Action to take when the download finishes or when
   * book_download_do_download_action () is called.
   */
  obj_properties[PROP_ACTION] =
    g_param_spec_enum ("action",
                       "Download action",
                       "Action to take when download finishes",
                       NULL,
                       BOOK_DOWNLOAD_ACTION_NONE,
                       G_PARAM_READABLE |
                       G_PARAM_STATIC_STRINGS);

  /**
   * BookDownload::start-time:
   *
   * User time when the download started, useful for launching applications
   * aware of focus stealing.
   */
  obj_properties[PROP_START_TIME] =
    g_param_spec_uint ("start-time",
                       "Event start time",
                       "Time for focus-stealing prevention.",
                       0, G_MAXUINT32, 0,
                       G_PARAM_READABLE |
                       G_PARAM_STATIC_STRINGS);

  obj_properties[PROP_CONTENT_TYPE] =
    g_param_spec_string ("content-type",
                         "Content Type",
                         "The download content type",
                         NULL,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, LAST_PROP, obj_properties);

  /**
   * BookDownload::filename-suggested:
   *
   * The ::filename-suggested signal is emitted when we have received the
   * suggested filename from WebKit.
   **/
  signals[FILENAME_SUGGESTED] = g_signal_new ("filename-suggested",
                                              G_OBJECT_CLASS_TYPE (object_class),
                                              G_SIGNAL_RUN_LAST,
                                              0,
                                              NULL, NULL, NULL,
                                              G_TYPE_NONE,
                                              1,
                                              G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * BookDownload::completed:
   *
   * The ::completed signal is emitted when @download has finished downloading.
   **/
  signals[COMPLETED] = g_signal_new ("completed",
                                     G_OBJECT_CLASS_TYPE (object_class),
                                     G_SIGNAL_RUN_LAST,
                                     0,
                                     NULL, NULL, NULL,
                                     G_TYPE_NONE,
                                     0);
  /**
   * BookDownload::error:
   *
   * The ::error signal wraps the @download ::error signal.
   **/
  signals[ERROR] = g_signal_new ("error",
                                 G_OBJECT_CLASS_TYPE (object_class),
                                 G_SIGNAL_RUN_LAST,
                                 0,
                                 NULL, NULL, NULL,
                                 G_TYPE_NONE,
                                 1, G_TYPE_POINTER);
}

static void
book_download_init (BookDownload *download)
{
 // LOG ("BookDownload initialising %p", download);

  download->download = NULL;

  download->action = BOOK_DOWNLOAD_ACTION_NONE;

  download->start_time = gtk_get_current_event_time ();

  download->show_notification = TRUE;
}

static void
download_response_changed_cb (WebKitDownload *wk_download,
                              GParamSpec     *spec,
                              BookDownload   *download)
{
  WebKitURIResponse *response;
  const char *mime_type;

  response = webkit_download_get_response (download->download);
  mime_type = webkit_uri_response_get_mime_type (response);
  if (!mime_type)
    return;

  download->content_type = g_content_type_from_mime_type (mime_type);
  if (download->content_type)
    g_object_notify_by_pspec (G_OBJECT (download), obj_properties[PROP_CONTENT_TYPE]);
}


static gboolean
set_destination_uri_for_suggested_filename (BookDownload *download, const char *suggested_filename)
{
  char *dest_dir;
  char *dest_name;
  char *destination_filename;
  char *destination_uri;

  dest_dir =  g_build_filename (g_get_home_dir (), ("Downloads"), NULL);

  /* Make sure the download directory exists */
  if (g_mkdir_with_parents (dest_dir, 0700) == -1) {
    g_critical ("Could not create downloads directory \"%s\": %s",
                dest_dir, strerror (errno));
    g_free (dest_dir);
    return FALSE;
  }

  if (suggested_filename != NULL) {
    dest_name = g_strdelimit (g_strdup (suggested_filename), G_DIR_SEPARATOR_S, '_');
  } else {
    dest_name = book_file_tmp_filename (".book-download-XXXXXX", NULL);
  }

  destination_filename = g_build_filename (dest_dir, dest_name, NULL);
  g_free (dest_dir);
  g_free (dest_name);

  /* Append (n) as needed. */
  if (g_file_test (destination_filename, G_FILE_TEST_EXISTS)) {
    int i = 1;
    const char *dot_pos;
    gssize position;
    char *serial = NULL;
    GString *tmp_filename;

    dot_pos = parse_extension (destination_filename);
    if (dot_pos)
      position = dot_pos - destination_filename;
    else
      position = strlen (destination_filename);

    tmp_filename = g_string_new (NULL);

    do {
      serial = g_strdup_printf ("(%d)", i++);

      g_string_assign (tmp_filename, destination_filename);
      g_string_insert (tmp_filename, position, serial);

      g_free (serial);
    } while (g_file_test (tmp_filename->str, G_FILE_TEST_EXISTS));

    destination_filename = g_strdup (tmp_filename->str);
    g_string_free (tmp_filename, TRUE);
  }

  destination_uri = g_filename_to_uri (destination_filename, NULL, NULL);
  g_free (destination_filename);

  g_assert (destination_uri);
  webkit_download_set_destination (download->download, destination_uri);
  g_free (destination_uri);

  return TRUE;
}
static gboolean
download_decide_destination_cb (WebKitDownload *wk_download,
                                const gchar    *suggested_filename,
                                BookDownload   *download)
{
  if (webkit_download_get_destination (wk_download))
    return TRUE;

  g_signal_emit (download, signals[FILENAME_SUGGESTED], 0, suggested_filename);

  if (webkit_download_get_destination (wk_download))
    return TRUE;

  return set_destination_uri_for_suggested_filename (download, suggested_filename);
}

static void
download_created_destination_cb (WebKitDownload *wk_download,
                                 const char     *destination,
                                 BookDownload   *download)
{
  char *filename;
  char *content_type;

  if (download->content_type && !g_content_type_is_unknown (download->content_type))
    return;

  /* The server didn't provide a valid content type, let's try to guess it from the
   * destination filename. We use g_content_type_guess() here instead of g_file_query_info(),
   * because we are only using the filename to guess the content type, since it doesn't make
   * sense to sniff the destination URI that will be empty until the download is completed.
   * We can't use g_file_query_info() with the partial download file either, because it will
   * always return application/x-partial-download based on the .wkdownload extension.
   */
  filename = g_filename_from_uri (destination, NULL, NULL);
  if (!filename)
    return;

  content_type = g_content_type_guess (filename, NULL, 0, NULL);
  g_free (filename);

  if (g_content_type_is_unknown (content_type)) {
    /* We could try to connect to received-data signal and sniff the contents when we have
     * enough data written in the file, but I don't think it's worth it.
     */
    g_free (content_type);
    return;
  }

  if (!download->content_type ||
      (download->content_type && !g_content_type_equals (download->content_type, content_type))) {
    g_free (download->content_type);
    download->content_type = content_type;
    g_object_notify_by_pspec (G_OBJECT (download), obj_properties[PROP_CONTENT_TYPE]);
    return;
  }

  g_free (content_type);
}

static void
display_download_finished_notification (WebKitDownload *download)
{
  GApplication *application;
  GtkWindow *toplevel;
  const char *dest;
   char *filename;
    char *message;
    GNotification *notification;

  application = G_APPLICATION (g_application_get_default ());
  toplevel = gtk_application_get_active_window (GTK_APPLICATION (application));
  dest = webkit_download_get_destination (download);

  if (!gtk_window_is_active (toplevel) && dest != NULL) {

    filename = g_filename_display_basename (dest);
    /* Translators: a desktop notification when a download finishes. */
    message = g_strdup_printf (_("完成下载<a href='%s'>%s</a>"), filename,filename);
    /* Translators: the title of the notification. */
    notification = g_notification_new (_("下载完成"));
    g_notification_set_body (notification, message);
    g_application_send_notification (application, "download-finished", notification);
    g_free (filename);
    g_free (message);
    g_object_unref (notification);
  }else if(dest!=NULL){

    filename = g_filename_display_basename (dest);
    message = g_strdup_printf (_("完成下载<a href='%s'>%s</a>"), filename,filename);
    notification = notify_notification_new ("下载完成",  message,NULL);
    notify_notification_show(notification,NULL);
    g_free (filename);
    g_free (message);
    g_object_unref (notification);
  }
}

static void
download_finished_cb (WebKitDownload *wk_download,
                      BookDownload   *download)
{
  download->finished = TRUE;

  book_download_do_download_action (download, download->action, download->start_time);

  if (download->show_notification)
    display_download_finished_notification (wk_download);

  g_signal_emit (download, signals[COMPLETED], 0);
}

static void
download_failed_cb (WebKitDownload *wk_download,
                    GError         *error,
                    BookDownload   *download)
{
  g_signal_handlers_disconnect_by_func (wk_download, download_finished_cb, download);

  //LOG ("error (%d - %d)! %s", error->code, 0, error->message);
  download->finished = TRUE;
  download->error = g_error_copy (error);
  g_signal_emit (download, signals[ERROR], 0, download->error);
}

/**
 * book_download_new:
 * @download: a #WebKitDownload to wrap
 *
 * Wraps @download in an #BookDownload.
 *
 * Returns: an #BookDownload.
 **/
BookDownload *
book_download_new (WebKitDownload *download)
{
  BookDownload *book_download;

  g_assert (WEBKIT_IS_DOWNLOAD (download));

  book_download = g_object_new (BOOK_TYPE_DOWNLOAD, NULL);

  g_signal_connect (download, "notify::response",
                    G_CALLBACK (download_response_changed_cb),
                    book_download);
  g_signal_connect (download, "decide-destination",
                    G_CALLBACK (download_decide_destination_cb),
                    book_download);
  g_signal_connect (download, "created-destination",
                    G_CALLBACK (download_created_destination_cb),
                    book_download);
  g_signal_connect (download, "finished",
                    G_CALLBACK (download_finished_cb),
                    book_download);
  g_signal_connect (download, "failed",
                    G_CALLBACK (download_failed_cb),
                    book_download);

  book_download->download = g_object_ref (download);
  g_object_set_data (G_OBJECT (download), "book-download-set", GINT_TO_POINTER (TRUE));

  return book_download;
}

/**
 * book_download_new_for_uri:
 * @uri: a source URI from where to download
 *
 * Creates an #BookDownload to download @uri.
 *
 * Returns: an #BookDownload.
 **/

void
book_download_disable_desktop_notification (BookDownload *download)
{
  g_assert (BOOK_IS_DOWNLOAD (download));

  download->show_notification = FALSE;
}
