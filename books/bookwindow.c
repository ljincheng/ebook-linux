
#include "bookwindow.h"
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <libgepub/gepub.h>
#include <gdk/gdkkeysyms.h>
#include "bookmenu.h"
#include "bookstorewin.h"
#include "bookdatabase.h"
#include <libmobi/mobi.h>
#include "book-widget.h"


static gchar * EPUB_PATH=NULL;
static GtkWidget *EPUB_INFO=NULL;
static  GtkWidget *sidebar_content;
static  GtkWidget *sidebar_header;

static void book_window_page_next(BookWidget *widget)
{
	book_widget_page_next(widget);
	gint currIndex= book_widget_get_chapter(widget);
 	gchar *bookid = book_widget_get_doc(widget)->id;
	book_set_history_chapter (bookid,currIndex);
}

static void book_window_page_prev(BookWidget *widget)
{
	book_widget_page_prev(widget);
	gint currIndex= book_widget_get_chapter(widget);
	gchar *bookid = book_widget_get_doc(widget)->id;
	book_set_history_chapter (bookid,currIndex);
}

static void destroy_book_window(GtkWidget *window, GtkWidget *view)
{
	BookDoc *doc = book_widget_get_doc(view);

 // book_doc_free(doc);
	//g_object_unref(doc);
	gtk_main_quit();
}

void book_window_page_change (GtkWidget *widget)
{
  BookDoc *doc = book_widget_get_doc(BOOK_WIDGET(widget));
  gint themes=book_widget_get_themes(BOOK_WIDGET(widget));

  gint chapternum = doc->chapterIndex;
  gint nchapternum = doc->chapterNum;
 
  gchar *docinfo;
  if(themes==1)
{
     g_type_class_unref (g_type_class_ref (GTK_TYPE_IMAGE_MENU_ITEM));
g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", FALSE, NULL);
  docinfo= g_strdup_printf (
        "<span foreground='#000000' background='#e4e4e4'  font='10'>%d/%d</span>"
        , chapternum, nchapternum);
}else if(themes==2)
    {
      g_type_class_unref (g_type_class_ref (GTK_TYPE_IMAGE_MENU_ITEM));
g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", TRUE, NULL);
       docinfo= g_strdup_printf (
        "<span foreground='#888A85' background='#e4e4e4'  font='10'>%d/%d</span>"
        , chapternum, nchapternum);
    }else if(themes==3)
    {
        docinfo= g_strdup_printf (
        "<span foreground='#333'   font='10'>%d/%d</span>"
        , chapternum, nchapternum);
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

 
 

static gboolean  book_key_press(GtkWidget * windowwidget, GdkEventKey  *event, BookWidget *widget)
{
  switch (event->keyval) {
	case GDK_KEY_Right:
	case GDK_KEY_j:
	case GDK_KEY_space:
	{
  		book_window_page_next(widget);
		book_window_page_change(widget);
	}
	break;

	case GDK_KEY_Left:
	case GDK_KEY_k:
	{
    		book_window_page_prev(widget);
		book_window_page_change(widget);
	}
	break;

	case GDK_KEY_Page_Up:
	{
	}
	break;

	case GDK_KEY_Page_Down:
	{
	}
	break;
    //　以下设置显示主题
	case GDK_KEY_1:
	{
     g_type_class_unref (g_type_class_ref (GTK_TYPE_IMAGE_MENU_ITEM));
g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", FALSE, NULL);
		book_widget_set_themes(widget, 1);
book_window_page_change(widget);
    gtk_widget_show_all(widget);
    return TRUE;
	}
	break;

	case GDK_KEY_2:
	{
     g_type_class_unref (g_type_class_ref (GTK_TYPE_IMAGE_MENU_ITEM));
g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", TRUE, NULL);
		book_widget_set_themes(widget, 2);
book_window_page_change(widget);
    gtk_widget_show_all(widget);
    return TRUE;
	}
	break;

	case GDK_KEY_3:
	{
		book_widget_set_themes(widget, 3);
book_window_page_change(widget);
    gtk_widget_show_all(widget);
    return TRUE;
	}
	break;
// 以下设置文件内容
	case GDK_KEY_F2:
	{
   if(gtk_widget_get_visible(sidebar_header))
    {
       gtk_widget_set_visible(sidebar_header,FALSE);
       gtk_widget_set_visible(sidebar_content,FALSE);
    }else{
       gtk_widget_set_visible(sidebar_header,TRUE);
       gtk_widget_set_visible(sidebar_content,TRUE);
    }
    return TRUE;
	}
	break;

	case GDK_KEY_F3:
	{
book_widget_splitPage(widget,false);
	}
	break;
    	case GDK_KEY_F4:
	{
book_widget_splitPage(widget,true);
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

	return FALSE;
}

static void
next_button_clicked_cb(GtkButton *source,GtkWidget *widget)
{
 book_window_page_next(BOOK_WIDGET(widget));
}
static void
prev_button_clicked_cb(GtkButton *source,GepubWidget *widget)
{
 book_window_page_prev(BOOK_WIDGET(widget));
}

static void
menu_button_clicked_cb(GtkButton *source,GtkWidget *widget)
{
  if(gtk_widget_get_visible(sidebar_header))
    {
       gtk_widget_set_visible(sidebar_header,FALSE);
       gtk_widget_set_visible(sidebar_content,FALSE);
    }else{
       gtk_widget_set_visible(sidebar_header,TRUE);
       gtk_widget_set_visible(sidebar_content,TRUE);
    }
}

static void
split_decorations (GtkSettings *settings,
                   GParamSpec  *pspec,
                   GtkBuilder  *builder)
{

}
 

GtkWidget * book_open_epub(const gchar *epubfile)
{
  GtkBuilder *builder;
  GtkSettings *settings;
  GtkWidget *header;
   GtkWidget *main_header;
  GtkWidget *main_content;

  GtkWidget *window;
	BookDoc *doc;
	GtkWidget *widget;
	GtkWidget *button;
  GtkWidget *prev_button;
  GtkWidget *next_button;
	GtkWidget *box;

  GtkWidget *overlay;
   GIcon *icon,*winIcon;

EPUB_PATH=epubfile;

 	widget = book_widget_new();
  	webkit_settings_set_enable_developer_extras(webkit_web_view_get_settings(WEBKIT_WEB_VIEW(widget)), TRUE);


  builder = gtk_builder_new ();
  gtk_builder_add_from_resource (builder, "/bebebookapp/bookwindow.ui", NULL);

  window = (GtkWidget *)gtk_builder_get_object (builder, "window");
  settings = gtk_widget_get_settings (window);
 //gtk_widget_set_size_request(GTK_WIDGET(window), 1024, 400);

  winIcon = g_themed_icon_new ("view-dual-symbolic");
    gtk_window_set_icon(GTK_WINDOW(window), winIcon);

  main_header = (GtkWidget *)gtk_builder_get_object (builder, "main-header");
	gtk_header_bar_set_show_close_button(main_header, TRUE);

	//window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 g_signal_connect(window, "destroy", G_CALLBACK(destroy_book_window), widget);
	gtk_window_set_default_size(window, 1024, 900);
	gtk_window_set_position(window, GTK_WIN_POS_CENTER);



  // gepub widget
g_printf("epub地址:%s\n",epubfile);
	doc = book_doc_init(epubfile);
  g_printf("epub文件解析完成\n");
	if (doc==NULL) {
		perror("BAD epub FILE");
		return NULL;
	}

  book_widget_set_doc (widget,doc);
	//gtk_container_add(GTK_CONTAINER(main_content), widget);

gchar *bookid = doc->id;
  int chapterHistoryIndex= book_get_history_chapter (bookid);
  printf("历史记阅读:bookid=%s,index=%d\n",bookid,chapterHistoryIndex);
  if(chapterHistoryIndex>0)
    {
      book_widget_set_chapter (widget,chapterHistoryIndex);
    }
gtk_header_bar_set_title(main_header,doc->bookName);
gtk_header_bar_set_subtitle(main_header,doc->author);
 // g_signal_connect (settings, "notify::gtk-decoration-layout", G_CALLBACK (split_decorations), builder);
 // split_decorations (settings, NULL, builder);


  sidebar_header= (GtkWidget *)gtk_builder_get_object (builder, "sidebar-header");
	gtk_widget_set_size_request(GTK_WIDGET(sidebar_header), 300, 40);
  //header = (GtkWidget *)gtk_builder_get_object (builder, "main-header");


 // next_button=(GtkWidget *)gtk_builder_get_object (builder, "next_button");
 // gtk_container_add (GTK_CONTAINER (next_button), gtk_image_new_from_icon_name ("pan-end-symbolic", GTK_ICON_SIZE_BUTTON));
  // g_signal_connect (next_button, "clicked", G_CALLBACK (next_button_clicked_cb), sidebar_header);


  	button = gtk_button_new();
    icon = g_themed_icon_new ("open-menu-symbolic");

      gtk_container_add (GTK_CONTAINER (button),  gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_BUTTON));
    g_object_unref (icon);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(main_header), button);
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_style_context_add_class(gtk_widget_get_style_context(box), "linked");
	prev_button = gtk_button_new();
   gtk_container_add (GTK_CONTAINER (prev_button), gtk_image_new_from_icon_name ("pan-start-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_container_add(GTK_CONTAINER(box), prev_button);
	next_button = gtk_button_new();
 gtk_container_add (GTK_CONTAINER (next_button), gtk_image_new_from_icon_name ("pan-end-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_container_add(GTK_CONTAINER(box), next_button);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(main_header), box);

  g_signal_connect (next_button, "clicked", G_CALLBACK (next_button_clicked_cb), widget);
  g_signal_connect (prev_button, "clicked", G_CALLBACK (prev_button_clicked_cb), widget);
  g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked_cb), sidebar_header);



overlay = gtk_overlay_new ();
  main_content=(GtkWidget *)gtk_builder_get_object (builder, "main-content");
  sidebar_content=(GtkWidget *)gtk_builder_get_object (builder, "sidebar-content");
  	gtk_widget_set_size_request(GTK_WIDGET(sidebar_content), 300, 700);
  gtk_widget_set_size_request(GTK_WIDGET(main_content), 700, 700);
//gtk_window_set_default_size(window, 1024, 900);


  EPUB_INFO = gtk_label_new ("");
      gtk_overlay_add_overlay (GTK_OVERLAY (overlay), EPUB_INFO);
      gtk_overlay_set_overlay_pass_through (GTK_OVERLAY (overlay), EPUB_INFO, TRUE);
      gtk_widget_set_halign (EPUB_INFO, GTK_ALIGN_END );
      gtk_widget_set_valign (EPUB_INFO, GTK_ALIGN_END);

    GtkWidget *menuwin= book_get_tree_widget (widget);

  gtk_container_add (GTK_CONTAINER (sidebar_content), menuwin);

      //gtk_widget_show_all(menuwin);

 gtk_container_add (GTK_CONTAINER (main_content), overlay);
   gtk_container_add (GTK_CONTAINER (overlay), widget);
 //gtk_widget_show_all (overlay);

	//键盘事件
	//g_signal_connect(window, "key-press-event", G_CALLBACK(deal_key_press), NULL);
  g_signal_connect(window, "key-press-event", G_CALLBACK(book_key_press), widget);
	//设置窗口最小值
	//gtk_widget_set_size_request(GTK_WIDGET(window), 1024, 700);




  // gtk_widget_show_all (widget);
  gtk_window_present (GTK_WINDOW (window));
  //gtk_widget_show_all (window);


  return window;
}

 GtkWidget * book_widget( const gchar *epubfile)
{

	BookDoc *doc;

	GtkWidget *widget;
	widget = book_widget_new();

	webkit_settings_set_enable_developer_extras(webkit_web_view_get_settings(WEBKIT_WEB_VIEW(widget)), TRUE);
	doc = book_doc_init (epubfile);
	if (doc==NULL) {
		perror("BAD epub FILE");
		return widget;
	}
book_widget_set_doc (widget,doc);
	//gepub_widget_set_paginate(GEPUB_WIDGET(widget), TRUE);
	//gepub_widget_set_doc(GEPUB_WIDGET(widget), doc);
	return widget;
}

/*
int
main (int argc, char *argv[])
{
gtk_init (&argc, &argv);
g_printf("加载文件:%s\n",argv[1]);
   GtkWidget *window;
  window= book_open(argv[1]);
   //window =book_open_epub (argv[1]);
    gtk_widget_show_all (window);
    gtk_main ();

  return 0;
}
 */
