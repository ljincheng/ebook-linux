
#include "bookwindow.h"
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <libgepub/gepub.h>
#include <gdk/gdkkeysyms.h>
#include "bookmenu.h"
#include "bookstorewin.h"

static gchar * EPUB_PATH=NULL;
static GtkWidget *EPUB_INFO=NULL;

static void destroy_book_window(GtkWidget *window, GtkWidget *view)
{
	GepubDoc *doc = gepub_widget_get_doc(view);

	g_object_unref(doc);
	//gtk_main_quit();
}

static void
reload_change_epub (GtkWidget *widget)
{
//g_printf("加载epub变更\n");
  GepubDoc *doc = gepub_widget_get_doc( GEPUB_WIDGET(widget));
gint themes=gepub_widget_get_themes(GEPUB_WIDGET(widget));
//  g_printf("doc:%s, reload_current_chapter:\n",gepub_doc_get_current_id (doc));

    gint chapternum = gepub_doc_get_chapter(doc);
	gint nchapternum = gepub_doc_get_n_chapters(doc);
  gint pos=gepub_widget_get_chapter_length(GEPUB_WIDGET(widget));

  gchar *docinfo;
  if(themes==1)
{
  docinfo= g_strdup_printf (
        "<span foreground='#000000'  font='10'>%d/%d/%d</span>"
        , chapternum, nchapternum,pos);
}else if(themes==2)
    {
       docinfo= g_strdup_printf (
        "<span foreground='#888A85'  font='10'>%d/%d/%d</span>"
        , chapternum, nchapternum,pos);
    }else if(themes==3)
    {
        docinfo= g_strdup_printf (
        "<span foreground='#333'   font='10'>%d/%d/%d</span>"
        , chapternum, nchapternum,pos);
    }
		//g_print("%s\n", docinfo);

gtk_label_set_markup(
GTK_LABEL(EPUB_INFO),
docinfo);

     g_free (docinfo);

}

static void
pk(gchar *key, GepubResource *value, gpointer data)
{
	g_print("%s: %s, %s\n", key, value->mime, value->uri);
}



static void
update_text (GepubDoc *doc)
{
    GList *l, *chunks;
    chunks = gepub_doc_get_chapter_all(doc);
    int i=1;
    for (l=chunks; l; l = l->next) {
            gchar *chunk = l->data;
            g_printf("TH:%d:    %s\n",i,chunk);
            i++;
    }
}


static gboolean  deal_key_press(GtkWidget * windowwidget, GdkEventKey  *event, GepubWidget *widget)
{
	switch (event->keyval) {
	case GDK_KEY_Right:
	case GDK_KEY_j:
	case GDK_KEY_space:
	{
		gepub_widget_page_next(widget);
	}
	break;

	case GDK_KEY_Left:
	case GDK_KEY_k:
	{
		gepub_widget_page_prev(widget);
	}
	break;

	case GDK_KEY_Page_Up:
	{
		GepubDoc *doc = gepub_widget_get_doc(widget);
		gepub_doc_go_prev(doc);
	}
	break;

	case GDK_KEY_Page_Down:
	{
		GepubDoc *doc = gepub_widget_get_doc(widget);
		gepub_doc_go_next(doc);
	}
	break;

	//　以下设置显示主题
	case GDK_KEY_1:
	{
		gepub_widget_set_themes(widget, 1);
	}
	break;

	case GDK_KEY_2:
	{
		gepub_widget_set_themes(widget, 2);
	}
	break;

	case GDK_KEY_3:
	{
		gepub_widget_set_themes(widget, 3);
	}
	break;
  case GDK_KEY_8:
      {

        gepub_widget_set_chapter(widget,8);
      }
      break;
    case GDK_KEY_9:
      {

        gepub_widget_set_chapter(widget,9);
      }
      break;
// 以下设置文件内容

	case GDK_KEY_F1:
	{
		GepubDoc *doc = gepub_widget_get_doc(widget);
		gint chapternum = gepub_doc_get_chapter(doc);
		gint nchapternum = gepub_doc_get_n_chapters(doc);
		g_print("第%d/%d章\n", chapternum, nchapternum);
		break;
	}

	case GDK_KEY_F2:
	{
    GtkWidget *menuwin= do_tree_store (widget);
      gtk_widget_show_all(menuwin);
	}
	break;

	case GDK_KEY_F3:
	{

	}
	break;
  case GDK_KEY_F5:
      {
              g_print("F5:\n");
              GepubDoc *doc = gepub_widget_get_doc(widget);
        gchar * txt=gepub_doc_get_current_path(doc);
        g_printf("currentPath:%s\n",txt);
           update_text(doc);
      }
                           break;
  case GDK_KEY_Escape:
      {
    gtk_widget_destroy(windowwidget);
      }
break;
	default:
		g_print("key press GDK_%x\n", event->keyval);
		break;
	}
reload_change_epub(widget);

	return TRUE;
}


static void
next_button_clicked_cb(GtkButton *source,GepubWidget *widget)
{
  gepub_widget_page_next(widget);
  reload_change_epub(widget);
}
static void
prev_button_clicked_cb(GtkButton *source,GepubWidget *widget)
{
  gepub_widget_page_prev(widget);
  reload_change_epub(widget);
}

static void
menu_button_clicked_cb(GtkButton *source,GtkWidget *widget)
{
  GtkWidget *menuwin= do_tree_store (widget);
 //  GtkWidget *menuwin= do_iconview(widget);
      gtk_widget_show_all(menuwin);

}

GtkWidget * book_open_epub(const gchar *epubfile)
{
	GtkWidget *window;

	GepubDoc *doc;

	GtkWidget *widget;
	GtkWidget *header;
	GtkWidget *button;
  GtkWidget *next_button;
  GtkWidget *prev_button;
	GtkWidget *box;

  GtkWidget *overlay;
   GIcon *icon;

EPUB_PATH=epubfile;
	widget = gepub_widget_new();

	webkit_settings_set_enable_developer_extras(webkit_web_view_get_settings(WEBKIT_WEB_VIEW(widget)), TRUE);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window, "destroy", G_CALLBACK(destroy_book_window), widget);
	  	gtk_widget_set_size_request(GTK_WIDGET(window), 700, 700);
  gtk_window_set_default_size(window, 1024, 800);
	gtk_window_set_position(window, GTK_WIN_POS_CENTER);

	header = gtk_header_bar_new();

	gtk_header_bar_set_show_close_button(header, TRUE);
	gtk_window_set_titlebar(GTK_WINDOW(window), header);
	button = gtk_button_new();
    icon = g_themed_icon_new ("mail-send-receive-symbolic");

      gtk_container_add (GTK_CONTAINER (button),  gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_BUTTON));
    g_object_unref (icon);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(header), button);
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_style_context_add_class(gtk_widget_get_style_context(box), "linked");
	prev_button = gtk_button_new();
   gtk_container_add (GTK_CONTAINER (prev_button), gtk_image_new_from_icon_name ("pan-start-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_container_add(GTK_CONTAINER(box), prev_button);
	next_button = gtk_button_new();
 gtk_container_add (GTK_CONTAINER (next_button), gtk_image_new_from_icon_name ("pan-end-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_container_add(GTK_CONTAINER(box), next_button);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), box);

  g_signal_connect (next_button, "clicked", G_CALLBACK (next_button_clicked_cb), GEPUB_WIDGET(widget));
  g_signal_connect (prev_button, "clicked", G_CALLBACK (prev_button_clicked_cb), GEPUB_WIDGET(widget));
  g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked_cb), GEPUB_WIDGET(widget));

	//键盘事件
	g_signal_connect(window, "key-press-event", G_CALLBACK(deal_key_press), widget);
	//设置窗口最小值



	// gepub widget
	doc = gepub_doc_new(epubfile, NULL);
  g_printf("epub文件解析完成\n");
	if (!doc) {
		perror("BAD epub FILE");
		return NULL;
	}

	gchar *title = gepub_doc_get_metadata(doc, GEPUB_META_TITLE);
	gtk_window_set_title(window, title);
	gtk_header_bar_set_subtitle(header, gepub_doc_get_metadata(doc, GEPUB_META_AUTHOR));
	gepub_widget_set_doc(GEPUB_WIDGET(widget), doc);
	//gtk_container_add(GTK_CONTAINER(window), widget);
	gepub_widget_set_paginate(GEPUB_WIDGET(widget), TRUE);


  // overlay;
overlay = gtk_overlay_new ();
EPUB_INFO = gtk_label_new ("");
      gtk_overlay_add_overlay (GTK_OVERLAY (overlay), EPUB_INFO);
      gtk_overlay_set_overlay_pass_through (GTK_OVERLAY (overlay), EPUB_INFO, TRUE);
      gtk_widget_set_halign (EPUB_INFO, GTK_ALIGN_CENTER);
      gtk_widget_set_valign (EPUB_INFO, GTK_ALIGN_END);



 gtk_container_add (GTK_CONTAINER (window), overlay);
   gtk_container_add (GTK_CONTAINER (overlay), widget);
 gtk_widget_show_all (overlay);




	return window;
}

 GtkWidget * book_widget( const gchar *epubfile)
{

	GepubDoc *doc;

	GtkWidget *widget;
	widget = gepub_widget_new();

	webkit_settings_set_enable_developer_extras(webkit_web_view_get_settings(WEBKIT_WEB_VIEW(widget)), TRUE);
	doc = gepub_doc_new(epubfile, NULL);
	if (!doc) {
		perror("BAD epub FILE");
		return widget;
	}
	gepub_widget_set_paginate(GEPUB_WIDGET(widget), TRUE);
	gepub_widget_set_doc(GEPUB_WIDGET(widget), doc);
	return widget;
}

int
main (int argc, char *argv[])
{
gtk_init (&argc, &argv);
g_printf("加载文件:%s\n",argv[1]);
   GtkWidget *window;
  //window= book_open(argv[1]);
   window =book_open_epub (argv[1]);
    gtk_widget_show_all (window);
    gtk_main ();

  return 0;
}
