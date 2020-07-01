#include "book-dev-table.h"

#include "book-common.h"
#include <gdk/gdkkeysyms.h>

struct _BookDevTable {
GtkBox parent;
GtkWidget *window;
GtkAccelGroup *accelGroup;
};


G_DEFINE_TYPE (BookDevTable, book_dev_table, GTK_TYPE_BOX)

static gboolean hovering_over_link = FALSE;
static GdkCursor *hand_cursor = NULL;
static GdkCursor *regular_cursor = NULL;
static void book_dev_table_init (BookDevTable *self)
{
self->accelGroup=NULL;
}

static void book_dev_table_finalize (GObject *object)
{
  book_log_debug("[BookBench对象退出清理内存]\n");
   //BookBench *self = BOOK_BENCH(object);

  G_OBJECT_CLASS (book_dev_table_parent_class)->finalize (object);
}


static void
book_dev_table_class_init (BookDevTableClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_dev_table_finalize;

}



static void
insert_link (GtkTextBuffer *buffer,
             GtkTextIter   *iter,
             gchar         *text,
             gint           page)
{
  GtkTextTag *tag;

  tag = gtk_text_buffer_create_tag (buffer, NULL,
                                    "foreground", "blue",
                                    "underline", PANGO_UNDERLINE_SINGLE,
                                    NULL);
  g_object_set_data (G_OBJECT (tag), "page", GINT_TO_POINTER (page));
  gtk_text_buffer_insert_with_tags (buffer, iter, text, -1, tag, NULL);
}


static void
show_page (GtkTextBuffer *buffer,
           gint           page)
{
  GtkTextIter iter;

  gtk_text_buffer_set_text (buffer, "", 0);
  gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
  if (page == 1)
    {
      gtk_text_buffer_insert (buffer, &iter, "Some text to show that simple ", -1);
      insert_link (buffer, &iter, "hyper text", 3);
      gtk_text_buffer_insert (buffer, &iter, " can easily be realized with ", -1);
      insert_link (buffer, &iter, "tags", 2);
      gtk_text_buffer_insert (buffer, &iter, ".", -1);
    }
  else if (page == 2)
    {
      gtk_text_buffer_insert (buffer, &iter,
                              "A tag is an attribute that can be applied to some range of text. "
                              "For example, a tag might be called \"bold\" and make the text inside "
                              "the tag bold. However, the tag concept is more general than that; "
                              "tags don't have to affect appearance. They can instead affect the "
                              "behavior of mouse and key presses, \"lock\" a range of text so the "
                              "user can't edit it, or countless other things.\n", -1);
      insert_link (buffer, &iter, "Go back", 1);
    }
  else if (page == 3)
    {
      GtkTextTag *tag;

      tag = gtk_text_buffer_create_tag (buffer, NULL,
                                        "weight", PANGO_WEIGHT_BOLD,
                                        NULL);
      gtk_text_buffer_insert_with_tags (buffer, &iter, "hypertext:\n", -1, tag, NULL);
      gtk_text_buffer_insert (buffer, &iter,
                              "machine-readable text that is not sequential but is organized "
                              "so that related items of information are connected.\n", -1);
      insert_link (buffer, &iter, "Go back", 1);
    }
}

static void
follow_if_link (GtkWidget   *text_view,
                GtkTextIter *iter)
{
  GSList *tags = NULL, *tagp = NULL;

  tags = gtk_text_iter_get_tags (iter);
  for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
    {
      GtkTextTag *tag = tagp->data;
      gint page = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (tag), "page"));

      if (page != 0)
        {
          show_page (gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view)), page);
          break;
        }
    }

  if (tags)
    g_slist_free (tags);
}

static gboolean
key_press_event (GtkWidget *text_view,
                 GdkEventKey *event)
{
  GtkTextIter iter;
  GtkTextBuffer *buffer;

  switch (event->keyval)
    {
      case GDK_KEY_Return:
      case GDK_KEY_KP_Enter:
        buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
        gtk_text_buffer_get_iter_at_mark (buffer, &iter,
                                          gtk_text_buffer_get_insert (buffer));
        follow_if_link (text_view, &iter);
        break;

      default:
        break;
    }

  return FALSE;
}

/* Links can also be activated by clicking or tapping.
 */
static gboolean
event_after (GtkWidget *text_view,
             GdkEvent  *ev)
{
  GtkTextIter start, end, iter;
  GtkTextBuffer *buffer;
  gdouble ex, ey;
  gint x, y;

  if (ev->type == GDK_BUTTON_RELEASE)
    {
      GdkEventButton *event;

      event = (GdkEventButton *)ev;
      if (event->button != GDK_BUTTON_PRIMARY)
        return FALSE;

      ex = event->x;
      ey = event->y;
    }
  else if (ev->type == GDK_TOUCH_END)
    {
      GdkEventTouch *event;

      event = (GdkEventTouch *)ev;

      ex = event->x;
      ey = event->y;
    }
  else
    return FALSE;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

  /* we shouldn't follow a link if the user has selected something */
  gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
  if (gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end))
    return FALSE;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         ex, ey, &x, &y);

  if (gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (text_view), &iter, x, y))
    follow_if_link (text_view, &iter);

  return TRUE;
}




static void
set_cursor_if_appropriate (GtkTextView    *text_view,
                           gint            x,
                           gint            y)
{
  GSList *tags = NULL, *tagp = NULL;
  GtkTextIter iter;
  gboolean hovering = FALSE;

  if (gtk_text_view_get_iter_at_location (text_view, &iter, x, y))
    {
      tags = gtk_text_iter_get_tags (&iter);
      for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
        {
          GtkTextTag *tag = tagp->data;
          gint page = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (tag), "page"));

          if (page != 0)
            {
              hovering = TRUE;
              break;
            }
        }
    }

  if (hovering != hovering_over_link)
    {
      hovering_over_link = hovering;

      if (hovering_over_link)
        gdk_window_set_cursor (gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT), hand_cursor);
      else
        gdk_window_set_cursor (gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT), regular_cursor);
    }

  if (tags)
    g_slist_free (tags);
}

/* Update the cursor image if the pointer moved.
 */
static gboolean
motion_notify_event (GtkWidget      *text_view,
                     GdkEventMotion *event)
{
  gint x, y;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         event->x, event->y, &x, &y);

  set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), x, y);

  return FALSE;
}

GtkWidget * book_dev_code_view(GtkBox* parentWidget)
{
     GtkWidget *view;
   GtkTextBuffer *buffer;
    GtkWidget *sw;
   GdkDisplay *display;

      display = gtk_widget_get_display (NULL);
      hand_cursor = gdk_cursor_new_from_name (display, "pointer");
      regular_cursor = gdk_cursor_new_from_name (display, "text");
   view = gtk_text_view_new ();
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_WORD);
      gtk_text_view_set_left_margin (GTK_TEXT_VIEW (view), 20);
      gtk_text_view_set_right_margin (GTK_TEXT_VIEW (view), 20);
      g_signal_connect (view, "key-press-event",
                        G_CALLBACK (key_press_event), NULL);
      g_signal_connect (view, "event-after", G_CALLBACK (event_after), NULL);
 g_signal_connect (view, "motion-notify-event", G_CALLBACK (motion_notify_event), NULL);

   sw = gtk_scrolled_window_new (NULL, NULL);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (sw), view);

 buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  show_page (buffer, 1);
  gtk_box_pack_start (parentWidget,sw, TRUE, TRUE,0);
 gtk_widget_show_all (sw);
return view;
}

static void book_dev_codeview_btn_event_click(GtkWidget *btn,GtkWidget *textview)
{
 GtkTextIter start, end;
  gchar *detail;
  GtkTextBuffer *buffer;
  buffer=gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);

  detail = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

  book_log_debug ("|book-dev-table|book_dev_codeview_btn_event_click,detail=%s\n",detail);
  if (!detail[0])
    {
      g_free (detail);
      detail = NULL;
    }
}

GtkWidget* book_dev_table_new(GtkWidget* window){
  BookDevTable *obj;
   GtkWidget * scrolled,*hpaned;
  GtkWidget *devtoollist;
  GtkWidget *bbox;
  GtkWidget *button;
  GtkWidget *textview;
  obj=g_object_new(BOOK_TYPE_DEV_TABLE,"orientation", GTK_ORIENTATION_VERTICAL,NULL);
  obj->window=window;

   bbox = gtk_button_box_new (GTK_ORIENTATION_VERTICAL);
  button = gtk_button_new_with_label ("OK");
  gtk_container_add (GTK_CONTAINER (bbox), button);
  gtk_box_pack_start (GTK_BOX (obj),bbox, FALSE,TRUE, 5);

   textview=book_dev_code_view(obj);
   g_signal_connect (button, "clicked", G_CALLBACK (book_dev_codeview_btn_event_click), textview);
 //gtk_box_pack_start (GTK_BOX (obj),, TRUE, TRUE,0);
  return obj;
}


 
