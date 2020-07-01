
#include "book-window.h"
 #include "book-webview.h"
#include "book-doc.h"
#include "book-doc-nav.h"
#include <gdk/gdkkeysyms.h>
#include "book-manage.h"


struct _BookWindow {
	GtkApplicationWindow parent;
  gchar *bookPath;
  BookWebview *webview;
  bool   isCtrlMask;
  char *btnColor;
  GtkWidget *colorBtn0;
  GtkWidget *colorBtn1;
  GtkWidget *colorBtn2;
  GtkWidget *colorBtn3;
  GtkWidget *colorBtn;
};

G_DEFINE_TYPE (BookWindow, book_window, GTK_TYPE_APPLICATION_WINDOW)

static void
book_window_init(BookWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
  self->bookPath=NULL;
  self->webview=NULL;
  self->isCtrlMask=false;
}

static void
book_window_constructed(GObject *object)
{
	BookWindow *widget = BOOK_WINDOW (object);
 G_OBJECT_CLASS (book_window_parent_class)->constructed (object);
	}

static void book_window_finalize(GObject *object)
{
	BookWindow *widget = BOOK_WINDOW(object);

	G_OBJECT_CLASS(book_window_parent_class)->finalize(object);
  if(widget->bookPath!=NULL)
    {
    g_free(widget->bookPath);
    }

  if(widget->webview!=NULL)
    {
      	g_clear_object(&widget->webview);
    }

}


static void
book_window_class_init(BookWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->constructed = book_window_constructed;
	object_class->finalize = book_window_finalize;
	//object_class->set_property = book_widget_set_property;
	//object_class->get_property = book_widget_get_property;

}



/***
 * 事件定义
 * =================================================
 */
gboolean  book_window_key_release(BookWindow * window, GdkEventKey  *event, 	BookWindow *bookwindow);
gboolean  book_window_key_press(BookWindow * window, GdkEventKey  *event, 	BookWindow *bookwindow);
static gboolean
book_window_header_colorBtn_draw_color (GtkWidget  *drawingarea,
            cairo_t    *cr,
            const char *color_name)
{
  GdkRGBA rgba;

  if (gdk_rgba_parse (&rgba, color_name))
    {
      gdk_cairo_set_source_rgba (cr, &rgba);
      cairo_paint (cr);
    }

  return FALSE;
}

 static gboolean
  book_window_header_colorBtn_draw_selectcolor (GtkWidget  *drawingarea, cairo_t *cr,
                gpointer data)
  {
    GdkRGBA rgba;
    BookWindow *window=(BookWindow *)data;
    char * color_name= window->btnColor;

  if (gdk_rgba_parse (&rgba, color_name))
    {
      gdk_cairo_set_source_rgba (cr, &rgba);
      cairo_paint (cr);
    }
     return FALSE;
 }

void book_window_event_book_next(GtkWidget  *btn, BookWindow *bookWindow)
{
  BookWebview *bookWebview=bookWindow->webview;
  g_printf("isCtrlMask=%d\n",bookWindow->isCtrlMask);
  if(bookWindow->isCtrlMask)
    {
      book_webview_chapter_next (bookWebview);
    }else{
       book_webview_page_next(bookWebview);
    }

}
void book_window_event_book_prev(GtkWidget *btn,BookWindow *bookWindow)
{
   BookWebview *bookWebview=bookWindow->webview;
  if(bookWindow->isCtrlMask)
    {
      book_webview_chapter_prev (bookWebview);
    }else{
       book_webview_page_prev(bookWebview);
    }

}
static void book_window_event_destroy(BookWindow *window, GtkWidget *parentWindow)
{
  book_log_debug("关闭窗口\n");
  gtk_widget_set_visible(parentWindow,true);

  BookWebview *webview=window->webview;
  if(webview!=NULL )
    {

  gchar *bookId=book_doc_get_metadata (book_webview_get_doc(webview),BOOK_META_ID);
  gchar * bookinfo=book_webview_get_info (window->webview);
  if(bookId!=NULL && bookinfo!=NULL)
        {
          book_manage_updateMark_bookmeta (bookId, bookinfo, NULL);
          g_free(bookId);
          g_free(bookinfo);
        }

    }


  gtk_widget_destroy(window);
//	gtk_main_quit();
}


static void book_window_menuBtn_action_click(GtkWidget * btn,GtkWidget *testGtkpopover)
{
 // g_printf("点击显示testGtkpopover\n");
  gtk_popover_popup(testGtkpopover);
}

static void book_window_colorBtn_action_bg0_click(GtkWidget * btn,BookWindow *window)
{
  if(btn == window->colorBtn0)
    {
       book_webview_set_background (window->webview,0);
      window->btnColor="white";
   GtkSettings *settings = gtk_settings_get_default ();
    g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                false,
                NULL);

     // g_signal_connect (window->colorBtn, "draw", G_CALLBACK (book_window_header_colorBtn_draw_color), "white");
    }else if(btn == window->colorBtn1){
book_webview_set_background (window->webview,1);
        window->btnColor="AntiqueWhite";
   GtkSettings *settings = gtk_settings_get_default ();
    g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                false,
                NULL);
       }else if(btn == window->colorBtn2){
book_webview_set_background (window->webview,2);
          window->btnColor="gray";
   GtkSettings *settings = gtk_settings_get_default ();
    g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                false,
                NULL);
          }else if(btn == window->colorBtn3){
book_webview_set_background (window->webview,3);
            window->btnColor="black";
   GtkSettings *settings = gtk_settings_get_default ();
    g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                true,
                NULL);
    }
 g_signal_emit_by_name(window->colorBtn,"direction-changed");
}


void book_window_header_item_create(GtkAccelGroup *accelGroup,GtkWidget * header,BookWindow *window)
{
  GtkWidget *prevBtn,*nextBtn,*box;

  	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_style_context_add_class(gtk_widget_get_style_context(box), "linked");

  prevBtn = gtk_button_new();
   gtk_container_add (GTK_CONTAINER (prevBtn), gtk_image_new_from_icon_name ("pan-start-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_container_add(GTK_CONTAINER(box), prevBtn);

  nextBtn = gtk_button_new();
 gtk_container_add (GTK_CONTAINER (nextBtn), gtk_image_new_from_icon_name ("pan-end-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_container_add(GTK_CONTAINER(box), nextBtn);
  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), box);

  g_signal_connect(nextBtn, "clicked", G_CALLBACK(book_window_event_book_next), window);
  g_signal_connect(prevBtn, "clicked", G_CALLBACK(book_window_event_book_prev), window);

  gtk_widget_add_accelerator(nextBtn,"clicked",accelGroup,GDK_KEY_j,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(nextBtn,"clicked",accelGroup,GDK_KEY_j,0,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(nextBtn,"clicked",accelGroup,GDK_KEY_space,0,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(prevBtn,"clicked",accelGroup,GDK_KEY_k,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(prevBtn,"clicked",accelGroup,GDK_KEY_k,0,GTK_ACCEL_VISIBLE);
  //gtk_widget_add_accelerator(nextBtn,"clicked",accelGroup,GDK_KEY_j,GDK_CONTROL_MASK | GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);
}

void book_window_header_menuBtn_create(GtkAccelGroup *accelGroup,GtkWidget *header,BookWindow *window)
{

   GtkWidget *bookMenuPopover,*bookMenuBtn,*bookMenuTreeView;

  GtkWidget *scrolled;
    GIcon *icon;
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (scrolled, TRUE);
  gtk_widget_set_vexpand (scrolled, TRUE);
  gtk_widget_set_size_request (scrolled, 300, 460);
  BookWebview *webview=window->webview;
  bookMenuTreeView=book_doc_nav_treeview_new(webview);
  gtk_container_add (GTK_CONTAINER (scrolled), bookMenuTreeView);

  bookMenuBtn=gtk_button_new();
    icon = g_themed_icon_new ("open-menu-symbolic");

      gtk_container_add (GTK_CONTAINER (bookMenuBtn),  gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_BUTTON));
    g_object_unref (icon);
  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), bookMenuBtn);


  bookMenuPopover=gtk_popover_new(bookMenuBtn);

   gtk_container_add (GTK_CONTAINER (bookMenuPopover), scrolled);
   //   gtk_widget_show (bookMenuTreeView);
  gtk_widget_show_all (scrolled);
  gtk_container_set_border_width (GTK_CONTAINER (bookMenuPopover), 5);

 g_signal_connect (bookMenuBtn, "clicked",  G_CALLBACK (book_window_menuBtn_action_click), bookMenuPopover);
 gtk_widget_add_accelerator(bookMenuBtn,"clicked",accelGroup,GDK_KEY_F9,0,GTK_ACCEL_VISIBLE);

}

void book_window_header_colorBtn_create(GtkAccelGroup *accelGroup,GtkWidget *header,BookWindow *window)
{

   GtkWidget *bookMenuPopover,*bookMenuBtn;
GtkWidget *vbox,*colorBtn0;
   GIcon *icon;

vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
//着色按键
  window->colorBtn0=gtk_button_new();
    colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (window->colorBtn0), colorBtn0);
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_window_header_colorBtn_draw_color), "White");
  gtk_box_pack_start (GTK_BOX (vbox), window->colorBtn0, FALSE, TRUE, 0);
  g_signal_connect (window->colorBtn0, "clicked",  G_CALLBACK (book_window_colorBtn_action_bg0_click), window);
  gtk_widget_add_accelerator(window->colorBtn0,"clicked",accelGroup,GDK_KEY_1,GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);


  window->colorBtn1=gtk_button_new();
  colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (window->colorBtn1), colorBtn0);
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_window_header_colorBtn_draw_color), "AntiqueWhite");
  gtk_box_pack_start (GTK_BOX (vbox), window->colorBtn1, FALSE, TRUE, 0);
  g_signal_connect (window->colorBtn1, "clicked",  G_CALLBACK (book_window_colorBtn_action_bg0_click), window);
  gtk_widget_add_accelerator(window->colorBtn1,"clicked",accelGroup,GDK_KEY_2,GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

 window->colorBtn2=gtk_button_new();
     colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (window->colorBtn2), colorBtn0);
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_window_header_colorBtn_draw_color), "gray");
  gtk_box_pack_start (GTK_BOX (vbox), window->colorBtn2, FALSE, TRUE, 0);
  g_signal_connect (window->colorBtn2, "clicked",  G_CALLBACK (book_window_colorBtn_action_bg0_click), window);
  gtk_widget_add_accelerator(window->colorBtn2,"clicked",accelGroup,GDK_KEY_3,GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);


 window->colorBtn3=gtk_button_new();
    colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (window->colorBtn3), colorBtn0);
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_window_header_colorBtn_draw_color), "black");
  gtk_box_pack_start (GTK_BOX (vbox), window->colorBtn3, FALSE, TRUE, 0);
  g_signal_connect (window->colorBtn3, "clicked",  G_CALLBACK (book_window_colorBtn_action_bg0_click), window);
  gtk_widget_add_accelerator(window->colorBtn3,"clicked",accelGroup,GDK_KEY_4,GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);



  bookMenuBtn=gtk_button_new();
  window->colorBtn=bookMenuBtn;
   colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (bookMenuBtn), colorBtn0);
    window->btnColor="white";
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_window_header_colorBtn_draw_selectcolor),  window);
  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), bookMenuBtn);



  bookMenuPopover=gtk_popover_new(bookMenuBtn);

   gtk_container_add (GTK_CONTAINER (bookMenuPopover), vbox);
   //   gtk_widget_show (bookMenuTreeView);
  gtk_widget_show_all (vbox);
  gtk_container_set_border_width (GTK_CONTAINER (bookMenuPopover), 1);

 g_signal_connect (bookMenuBtn, "clicked",  G_CALLBACK (book_window_menuBtn_action_click), bookMenuPopover);
 gtk_widget_add_accelerator(bookMenuBtn,"activate",accelGroup,GDK_KEY_F10,0,GTK_ACCEL_VISIBLE);

}

GtkWidget*
book_window_new (GtkApplication *application)
{
  	GtkWidget *window;
  window= g_object_new (BOOK_TYPE_WINDOW,
                       "application", application,
                       NULL);
  return window;
}
GtkWidget * book_window_create(gchar *bookFile, GtkWidget * parentWindow,const gchar * initInfo)
{
	BookWindow *window;
	GtkWidget *header;
	gchar *bookTitle;
	gchar *bookAuthor;
	GtkWidget * bookWebview;
	BookDoc *bookDoc;

	if (!window) {
		window =book_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_screen(GTK_WINDOW(window), gtk_widget_get_screen(parentWindow));
		//g_signal_connect (window, "destroy",   G_CALLBACK (book_window_event_destroy), window);
 g_signal_connect(window, "destroy", G_CALLBACK(book_window_event_destroy), parentWindow);


		bookWebview = book_webview_new();
		gtk_container_add(GTK_CONTAINER(window), bookWebview);
  window->webview=bookWebview;
    window->bookPath=bookFile;

		header = gtk_header_bar_new();
		gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
		gtk_header_bar_set_title(GTK_HEADER_BAR(header), bookTitle);
		gtk_window_set_titlebar(GTK_WINDOW(window), header);


		bookDoc = book_doc_new(bookFile, NULL);
		book_webview_set_doc(bookWebview, bookDoc);
		book_webview_set_info(bookWebview, initInfo);
		bookTitle = book_doc_get_metadata(bookDoc, BOOK_META_TITLE);
		bookAuthor = book_doc_get_metadata(bookDoc, BOOK_META_AUTHOR);
		gtk_header_bar_set_title(header, bookTitle);
		gtk_header_bar_set_subtitle(header, bookAuthor);
    	gtk_window_set_default_size(GTK_WINDOW(window),book_webview_get_pageWidth ( bookWebview), book_webview_get_pageHeight ( bookWebview));

	g_signal_connect(window, "key-release-event", G_CALLBACK(book_window_key_release), bookWebview);
g_signal_connect(window, "key-press-event", G_CALLBACK(book_window_key_press), bookWebview);
  GtkAccelGroup *accelGroup = gtk_accel_group_new();
 gtk_window_add_accel_group(GTK_WINDOW(window),accelGroup);

gtk_widget_add_accelerator(window,"destroy",accelGroup,GDK_KEY_w,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

		book_window_header_menuBtn_create(accelGroup,header, window);
book_window_header_item_create(accelGroup,header,window);
    book_window_header_colorBtn_create(accelGroup,header, window);
	}

  /*
    if (!gtk_widget_get_visible(window))
		gtk_widget_show_all(window);
	else
		gtk_widget_destroy(window);
   */
  gtk_widget_set_visible(parentWindow,false);

  return window;
}


gboolean  book_window_key_release(BookWindow * window, GdkEventKey  *event, 	BookWindow *bookwindow)
{
 // g_printf("key:state=%d,GDK_CONTROL_MASK=%d\n",event->state,GDK_CONTROL_MASK);
 BookWebview *webview=bookwindow->webview;
  switch (event->keyval) {
	case GDK_KEY_Escape:
	{

 // 		 gchar *info= book_webview_get_info(webview);
   // g_printf("info:%s\n",info);
	}
	break;
case GDK_KEY_w:
	{
      if(event->state  & GDK_CONTROL_MASK)
      {
          g_signal_emit_by_name(window,"destroy");
      }

	}
	break;
	default:
		//g_print("key press GDK_%x\n", event->keyval);
		break;
	}

   if(!(event->state & GDK_CONTROL_MASK))
      {
         window->isCtrlMask=false;
      }else{
        window->isCtrlMask=true;
      }
	return FALSE;
}


gboolean  book_window_key_press(BookWindow * window, GdkEventKey  *event,BookWindow *bookwindow)
{
 if(event->state & GDK_CONTROL_MASK)
      {
         window->isCtrlMask=true;
      }else{
         window->isCtrlMask=false;
      }

	return FALSE;
}

