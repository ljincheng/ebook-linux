
#include "book-pdf-window.h"
 #include "book-pdf.h"
#include "book-doc.h"
#include "book-doc-nav.h"
#include <gdk/gdkkeysyms.h>
#include "book-manage.h"
#include "book-dialogs.h"
#include <math.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

struct _BookPdfWindow {
	GtkApplicationWindow parent;
  gchar *bookPath;
  BookPdf *bookPdf;
  GtkWidget *bookPdfView;
  gboolean splitPage;
  bool   isCtrlMask;
  char *btnColor;
  GtkWidget *colorBtn0;
  GtkWidget *colorBtn1;
  GtkWidget *colorBtn2;
  GtkWidget *colorBtn3;
  GList *pageViewList;
  GList *pageViewSurface;
  GThread *threads;
  double scale;
};

G_DEFINE_TYPE (BookPdfWindow, book_pdf_window, GTK_TYPE_APPLICATION_WINDOW)

cairo_surface_t *_pdfSurface;


static void
book_pdf_window_cmd_edit_copy (GSimpleAction *action,
			 GVariant      *parameter,
			 gpointer       user_data)
{
	//BookPdfWindow *ev_window = user_data;
 book_log_debug("=======================copy============================\n");
  printf("-------------------------Copy-------------------------\n");
}

static const GActionEntry winPdfActions[] = {
	{ "copy", book_pdf_window_cmd_edit_copy, NULL, NULL, NULL  }
};


//=======================test=====================

cairo_surface_t * book_pdf_window_chapter_get_surface(BookPdfWindow *bookPdfWindow,gpointer drawWidget)
{

  BookPdf *bookPdf;
  GList *pageViewList,*iterator;
  cairo_surface_t * chapterSurface;
  double chapterWidth,chapterHeight,width,height;
  gint index=0;

  bookPdf=bookPdfWindow->bookPdf;
  gdouble scale=bookPdfWindow->scale;

   pageViewList=bookPdfWindow->pageViewList;
  for (iterator = pageViewList; iterator; iterator = iterator->next) {
     if(drawWidget== iterator->data)
break;
     index++;
  }
book_log_debug("book_pdf_window_chapter_get_surface:index=%d\n ",index);

  book_pdf_chapter_get_size(bookPdf,index,&chapterWidth,&chapterHeight);
  width=scale * chapterWidth;
  height=scale * chapterHeight;
  chapterSurface=book_pdf_chapter_get_surface (bookPdf, index, (gint)width, (gint)height);
  _pdfSurface=chapterSurface;

return chapterSurface;
}


void book_pdf_window_chapter_export_image(BookPdfWindow *bookPdfWindow,gchar *filePath)
{
 BookPdf *bookPdf;
  GdkPixbuf *pixbuf;
  cairo_surface_t * chapterSurface;
  double chapterWidth,chapterHeight,width,height;
  gint index=0;
gint pageNum=0;
  bookPdf=bookPdfWindow->bookPdf;
  gdouble scale=bookPdfWindow->scale;
pageNum=book_pdf_get_chapter_num (bookPdf);
  for (;index<pageNum;index++) {
 book_pdf_chapter_get_size(bookPdf,index,&chapterWidth,&chapterHeight);
  width=scale * chapterWidth;
  height=scale * chapterHeight;
  chapterSurface=book_pdf_chapter_get_surface (bookPdf, index, (gint)width, (gint)height);
  pixbuf= gdk_pixbuf_get_from_surface (chapterSurface,  0, 0, cairo_image_surface_get_width (chapterSurface),  cairo_image_surface_get_height (chapterSurface));
    gchar *filename=g_strdup_printf("%s-%d.png",filePath,index);
    book_log_debug("导出第%d张：filename=%s\n",index,filename);
  gdk_pixbuf_save(pixbuf,filename,"png",NULL);
    g_free(filename);
  }



}

cairo_surface_t * book_pdf_window_load_surface(BookPdfWindow *bookPdfWindow,gint indexs){


      BookPdf *bookPdf;
  GList *pageViewList,*iterator;
  double chapterWidth,chapterHeight,width,height;
 gdouble scale;
      gint index=0;

  bookPdf=bookPdfWindow->bookPdf;
  scale=bookPdfWindow->scale;
  pageViewList=bookPdfWindow->pageViewList;
  for (iterator = pageViewList; iterator; iterator = iterator->next) {

     GtkWidget *drawWidget=(GtkWidget *)iterator->data;
    book_pdf_render_set_scale(drawWidget,scale);
    gtk_widget_queue_draw(drawWidget);
    /*
     book_pdf_chapter_get_size(bookPdf,index,&chapterWidth,&chapterHeight);
  width=scale * chapterWidth;
  height=scale * chapterHeight;
     gtk_widget_set_size_request (drawWidget, (gint)width,(gint)height);
 g_signal_emit_by_name(drawWidget,"direction-changed");
    index++;
     */
  }
return NULL;


}

static void
book_pdf_window_init(BookPdfWindow *window)
{
  GAction *action;
  GtkBuilder *builder;
  GtkWidget *view_popup;
  GMenuModel   *view_popup_menu;
   GSimpleActionGroup *actions;

  gtk_widget_init_template (GTK_WIDGET (window));
  window->bookPath=NULL;
  window->bookPdf=NULL;
  window->isCtrlMask=false;
  window->scale=1.0;
  window->splitPage=TRUE;
  window->pageViewList=NULL;
  window->threads=NULL;


  //actions = g_simple_action_group_new ();
  g_action_map_add_action_entries (G_ACTION_MAP (window), winPdfActions, G_N_ELEMENTS (winPdfActions), NULL);

 // gtk_widget_insert_action_group (window, "top", G_ACTION_GROUP (actions));



  builder = gtk_builder_new ();
  gtk_builder_add_from_string (builder,
                               "<interface>"
                               "  <menu id='view-pdf-menu'>"
                               "    <section>"
                               "      <item>"
                               "        <attribute name='label' translatable='yes'>_Copy</attribute>"
                               "        <attribute name='action'>win.copy</attribute>"
 "<attribute name='accel'>&lt;Primary&gt;c</attribute>"
                               "      </item>"
                               "    </section>"
                               "  </menu>"
                               "</interface>", -1, NULL);
view_popup_menu=g_object_ref (G_MENU_MODEL (gtk_builder_get_object (builder, "view-pdf-menu")));
g_object_unref (builder);
 view_popup= gtk_menu_new_from_model (view_popup_menu);
  gtk_menu_attach_to_widget (GTK_MENU (view_popup),  GTK_WIDGET (window), NULL);
//gtk_menu_popup_at_pointer (GTK_MENU (view_popup), NULL);

  action = g_action_map_lookup_action (G_ACTION_MAP (window), "copy");
	g_simple_action_set_enabled (G_SIMPLE_ACTION (action), true);
book_log_debug("----------------init copy--------------------------\n");
}

static void
book_pdf_window_constructed(GObject *object)
{
	BookPdfWindow *widget = BOOK_PDF_WINDOW (object);
 G_OBJECT_CLASS (book_pdf_window_parent_class)->constructed (object);
	}

static void book_pdf_window_finalize(GObject *object)
{
  book_log_debug("[BookPdfWindow]内存清理\n");
	BookPdfWindow *widget = BOOK_PDF_WINDOW(object);

	G_OBJECT_CLASS(book_pdf_window_parent_class)->finalize(object);
  if(widget->bookPath!=NULL)
    {
   g_free(widget->bookPath);
    }

  if(widget->bookPdf!=NULL)
    {
      	g_clear_object(&widget->bookPdf);
    }
if(widget->pageViewList!=NULL)
    {
      	g_clear_pointer(&widget->pageViewList, g_list_free);
    }
}


static void
book_pdf_window_class_init(BookPdfWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->constructed = book_pdf_window_constructed;
	object_class->finalize = book_pdf_window_finalize;
	//object_class->set_property = book_widget_set_property;
	//object_class->get_property = book_widget_get_property;

}



/***
 * 事件定义
 * =================================================
 */
gboolean  book_pdf_window_key_release(BookPdfWindow * window, GdkEventKey  *event, 	BookPdfWindow *bookwindow);
gboolean  book_pdf_window_key_press(BookPdfWindow * window, GdkEventKey  *event, 	BookPdfWindow *bookwindow);

static gint book_pdf_window_bookPdfView_drawcb(GtkWidget *widget,
         cairo_t   *cr,
         gpointer   data)
{
  cairo_surface_t *surface=book_pdf_window_chapter_get_surface(data,widget);
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

 // cairo_set_source_surface (cr, _pdfSurface, 0, 0);
 // cairo_paint (cr);

  return TRUE;
}

static gboolean
book_pdf_window_header_colorBtn_draw_color (GtkWidget  *drawingarea,
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
  book_pdf_window_header_colorBtn_draw_selectcolor (GtkWidget  *drawingarea, cairo_t *cr,
                gpointer data)
  {
    GdkRGBA rgba;
    BookPdfWindow *window=(BookPdfWindow *)data;
    char * color_name= window->btnColor;

  if (gdk_rgba_parse (&rgba, color_name))
    {
      gdk_cairo_set_source_rgba (cr, &rgba);
      cairo_paint (cr);
    }
     return FALSE;
 }

void book_pdf_window_event_book_next(GtkWidget  *btn, BookPdfWindow *bookPdfWindow)
{
  BookPdf *bookPdf=bookPdfWindow->bookPdf;
  g_printf("isCtrlMask=%d\n",bookPdfWindow->isCtrlMask);

      book_pdf_chapter_next (bookPdf);
  book_log_debug("pdf显示:%d／%d页\n" ,book_pdf_get_chapterIndex(bookPdf), book_pdf_get_chapter_num (bookPdf));
 // book_pdf_get_surface (bookPdf);
   // _pdfSurface=chapterSurface;
  book_pdf_window_load_surface(bookPdfWindow,book_pdf_get_chapterIndex(bookPdf));
      // g_signal_emit_by_name(bookPdfWindow->bookPdfView,"direction-changed");
}
void book_pdf_window_event_book_prev(GtkWidget *btn,BookPdfWindow *bookPdfWindow)
{
   BookPdf *bookPdf=bookPdfWindow->bookPdf;
      book_pdf_chapter_prev (bookPdf);
   book_log_debug("pdf显示:%d／%d页\n" ,book_pdf_get_chapterIndex(bookPdf), book_pdf_get_chapter_num (bookPdf));
 // book_pdf_get_surface (bookPdf);
     // g_signal_emit_by_name(bookPdfWindow->bookPdfView,"direction-changed");
     book_pdf_window_load_surface(bookPdfWindow,book_pdf_get_chapterIndex(bookPdf));
}

static void book_pdf_window_event_destroy(BookPdfWindow *window, GtkWidget *parentWindow)
{
  book_log_debug("关闭(book_pdf_window)窗口\n");
  if(parentWindow!=NULL)
    gtk_widget_set_visible(parentWindow,true);
  //if(window->threads!=NULL)
 //   g_thread_join(window->threads);
  book_log_debug("正常关闭(book_pdf_window)窗口\n");
 // gtk_widget_destroy(window);
//	gtk_main_quit();
}


static void book_pdf_window_menuBtn_action_click(GtkWidget * btn,GtkWidget *testGtkpopover)
{
 // g_printf("点击显示testGtkpopover\n");
  gtk_popover_popup(testGtkpopover);
}
static void book_pdf_window_saveBtn_action_click(GtkWidget *btn,BookPdfWindow *window)
{
    cairo_surface_t *surface;
  GdkPixbuf *pixbuf;

  GtkFileChooserNative *native;
 GtkFileChooser *chooser;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint res;

 native = gtk_file_chooser_native_new ("Save File",
                                       window,
                                       action,
                                        "_Save",
                                        "_Cancel");
  chooser = GTK_FILE_CHOOSER (native);

  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);



  res = gtk_native_dialog_run (GTK_NATIVE_DIALOG (native));
  if (res == GTK_RESPONSE_ACCEPT)
    {
      char *filename;
      filename = gtk_file_chooser_get_filename (chooser);

      book_pdf_window_chapter_export_image(window,filename);

      g_free (filename);
    }

  g_object_unref (native);


}

void book_pdf_window_checkSplitPageBtn_click(GtkWidget * btn,BookPdfWindow *window){
 gboolean active;

  g_object_get (btn, "active", &active, NULL);
  window->splitPage=active;
book_pdf_window_load_surface(window,book_pdf_get_chapterIndex(window->bookPdf));
}
static void book_pdf_window_colorBtn_action_bg0_click(GtkWidget * btn,BookPdfWindow *window)
{
  if(btn == window->colorBtn0)
    {
      window->btnColor="white";
   GtkSettings *settings = gtk_settings_get_default ();
    g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                false,
                NULL);

     // g_signal_connect (window->colorBtn, "draw", G_CALLBACK (book_pdf_window_header_colorBtn_draw_color), "white");
    }else if(btn == window->colorBtn1){
        window->btnColor="AntiqueWhite";
   GtkSettings *settings = gtk_settings_get_default ();
    g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                false,
                NULL);
       }else if(btn == window->colorBtn2){
          window->btnColor="gray";
   GtkSettings *settings = gtk_settings_get_default ();
    g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                true,
                NULL);
          }else if(btn == window->colorBtn3){
            window->btnColor="black";
   GtkSettings *settings = gtk_settings_get_default ();
    g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                true,
                NULL);
    }

}


void book_pdf_window_header_item_create(GtkAccelGroup *accelGroup,GtkWidget * header,BookPdfWindow *window)
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

  g_signal_connect(nextBtn, "clicked", G_CALLBACK(book_pdf_window_event_book_next), window);
  g_signal_connect(prevBtn, "clicked", G_CALLBACK(book_pdf_window_event_book_prev), window);

  gtk_widget_add_accelerator(nextBtn,"clicked",accelGroup,GDK_KEY_j,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(nextBtn,"clicked",accelGroup,GDK_KEY_j,0,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(nextBtn,"clicked",accelGroup,GDK_KEY_space,0,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(prevBtn,"clicked",accelGroup,GDK_KEY_k,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(prevBtn,"clicked",accelGroup,GDK_KEY_k,0,GTK_ACCEL_VISIBLE);
  //gtk_widget_add_accelerator(nextBtn,"clicked",accelGroup,GDK_KEY_j,GDK_CONTROL_MASK | GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);
}

static void book_pdf_window_set_scale(BookPdfWindow *window,gdouble scale)
{
    BookPdf *bookPdf=window->bookPdf;

 // gint pageWidth=ceil(book_pdf_get_width (window->bookPdf)*scale);
 // gint  pageHeight=ceil(book_pdf_get_height (window->bookPdf)*scale);
  //book_log_debug("pdf scale=%f,pageWidth=%d,pageHeight=%d \n",scale,pageWidth,pageHeight);
  //  	gtk_window_set_default_size(GTK_WINDOW(window->bookPdfView),pageWidth,pageHeight);
  //book_pdf_window_fullPdf_surface(window);
  window->scale=scale;
  book_pdf_set_scale(bookPdf,scale);
  book_log_debug("pdf scale=%f \n",scale);
       book_pdf_window_load_surface(window,book_pdf_get_chapterIndex(window->bookPdf));
       // g_signal_emit_by_name(window->bookPdfView,"direction-changed");
}
static gchar*
book_pdf_format_value_callback (GtkAdjustment *adj, BookPdfWindow *window)
{
   double fraction;
  fraction = gtk_adjustment_get_value (adj);
book_log_debug("book_pdf_format_value_callback:value=%f \n",fraction);
  if(fraction==74.0)
    {
      gtk_adjustment_set_value(adj,50.0);
      book_pdf_window_set_scale(window,0.5);
    }else if(fraction==51.0 || fraction ==99.0){
      gtk_adjustment_set_value(adj,75.0);
      book_pdf_window_set_scale(window,0.75);
       }else if(fraction==76.0 || fraction==119.0){
      gtk_adjustment_set_value(adj,100.0);
        book_pdf_window_set_scale(window,1.0);
         }else if(fraction==101.0 || fraction==149.0){
      gtk_adjustment_set_value(adj,120.0);
          book_pdf_window_set_scale(window,1.2);
            }else if(fraction==121.0 || fraction==199.0){
      gtk_adjustment_set_value(adj,150.0);
              book_pdf_window_set_scale(window,1.50);
              }else if(fraction==151.0 || fraction==299.0){
      gtk_adjustment_set_value(adj,200.0);
                 book_pdf_window_set_scale(window,2.00);
                   }else if(fraction==201.0 || fraction==399.0){
      gtk_adjustment_set_value(adj,300.0);
                     book_pdf_window_set_scale(window,3.00);
                         }else if(fraction==301.0 || fraction>400.0){
      gtk_adjustment_set_value(adj,400.0);
                           book_pdf_window_set_scale(window,4.00);
    }
 // return g_strdup_printf ("-->\%0.f<--", fraction);
 }
void book_pdf_window_header_menuBtn_create(GtkAccelGroup *accelGroup,GtkWidget *header,BookPdfWindow *window)
{

   GtkWidget *bookMenuPopover,*bookMenuBtn,*bookMenuTreeView;
  // GtkWidget *scrolled;
  GtkWidget *saveBtn ,*checkSplitPageBtn,*spinner;
GtkWidget *table,*label,*scaleWidget;
    GtkAdjustment *adj1;
    GIcon *icon;
  //scrolled = gtk_scrolled_window_new (NULL, NULL);
//  gtk_widget_set_hexpand (scrolled, TRUE);
 // gtk_widget_set_vexpand (scrolled, TRUE);
 // gtk_widget_set_size_request (scrolled, 300, 460);


  bookMenuBtn=gtk_button_new();
    icon = g_themed_icon_new ("open-menu-symbolic");

      gtk_container_add (GTK_CONTAINER (bookMenuBtn),  gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_BUTTON));
    g_object_unref (icon);
  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), bookMenuBtn);


  bookMenuPopover=gtk_popover_new(bookMenuBtn);

  // gtk_container_add (GTK_CONTAINER (bookMenuPopover), scrolled);
   //   gtk_widget_show (bookMenuTreeView);
  //gtk_widget_show_all (scrolled);
  gtk_container_set_border_width (GTK_CONTAINER (bookMenuPopover), 5);

   table = gtk_grid_new ();
   gtk_grid_set_row_spacing (GTK_GRID (table), 4);
      gtk_grid_set_column_spacing (GTK_GRID (table), 4);
  gtk_container_add (GTK_CONTAINER (bookMenuPopover), table);
  label = gtk_label_new_with_mnemonic ("缩放");
  gtk_grid_attach (GTK_GRID (table), label, 0, 0, 1, 1);


 g_signal_connect (bookMenuBtn, "clicked",  G_CALLBACK (book_pdf_window_menuBtn_action_click), bookMenuPopover);
 gtk_widget_add_accelerator(bookMenuBtn,"clicked",accelGroup,GDK_KEY_F9,0,GTK_ACCEL_VISIBLE);


  //添加缩放比例
  adj1 = gtk_adjustment_new(50,50,401.0,1 ,1,0.0);
 gtk_adjustment_set_value(adj1,100.0);
  gtk_adjustment_set_page_size(adj1,1);
  scaleWidget=gtk_spin_button_new(GTK_ADJUSTMENT(adj1),1,0);
 // gtk_container_add (GTK_CONTAINER (bookMenuPopover), scaleWidget);
  gtk_grid_attach (GTK_GRID (table), scaleWidget, 1, 0, 1, 1);

//单页显示按键

   spinner= gtk_spinner_new ();
 // gtk_widget_set_size_request(spinner,200,10);
  gtk_grid_attach (GTK_GRID (table), spinner, 0, 1, 2, 1);
      //保存按键
  saveBtn=gtk_model_button_new();
  g_object_set(G_OBJECT(saveBtn),"text","导出每页",NULL);
  gtk_grid_attach (GTK_GRID (table), saveBtn, 0, 2, 2, 1);

  gtk_widget_show_all (bookMenuPopover);
  g_signal_connect (checkSplitPageBtn, "clicked",  G_CALLBACK (book_pdf_window_checkSplitPageBtn_click), window);
  g_signal_connect (adj1, "value-changed", G_CALLBACK (book_pdf_format_value_callback), window);
   g_signal_connect (saveBtn, "clicked",  G_CALLBACK (book_pdf_window_saveBtn_action_click), window);
  gtk_widget_add_accelerator(saveBtn,"activate",accelGroup,GDK_KEY_s,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE | GTK_ACCEL_LOCKED);



}

void book_pdf_window_header_colorBtn_create(GtkAccelGroup *accelGroup,GtkWidget *header,BookPdfWindow *window)
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
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_pdf_window_header_colorBtn_draw_color), "White");
  gtk_box_pack_start (GTK_BOX (vbox), window->colorBtn0, FALSE, TRUE, 0);
  g_signal_connect (window->colorBtn0, "clicked",  G_CALLBACK (book_pdf_window_colorBtn_action_bg0_click), window);
  gtk_widget_add_accelerator(window->colorBtn0,"clicked",accelGroup,GDK_KEY_1,GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);


  window->colorBtn1=gtk_button_new();
  colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (window->colorBtn1), colorBtn0);
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_pdf_window_header_colorBtn_draw_color), "AntiqueWhite");
  gtk_box_pack_start (GTK_BOX (vbox), window->colorBtn1, FALSE, TRUE, 0);
  g_signal_connect (window->colorBtn1, "clicked",  G_CALLBACK (book_pdf_window_colorBtn_action_bg0_click), window);
  gtk_widget_add_accelerator(window->colorBtn1,"clicked",accelGroup,GDK_KEY_2,GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

 window->colorBtn2=gtk_button_new();
     colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (window->colorBtn2), colorBtn0);
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_pdf_window_header_colorBtn_draw_color), "gray");
  gtk_box_pack_start (GTK_BOX (vbox), window->colorBtn2, FALSE, TRUE, 0);
  g_signal_connect (window->colorBtn2, "clicked",  G_CALLBACK (book_pdf_window_colorBtn_action_bg0_click), window);
  gtk_widget_add_accelerator(window->colorBtn2,"clicked",accelGroup,GDK_KEY_3,GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);


 window->colorBtn3=gtk_button_new();
    colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (window->colorBtn3), colorBtn0);
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_pdf_window_header_colorBtn_draw_color), "black");
  gtk_box_pack_start (GTK_BOX (vbox), window->colorBtn3, FALSE, TRUE, 0);
  g_signal_connect (window->colorBtn3, "clicked",  G_CALLBACK (book_pdf_window_colorBtn_action_bg0_click), window);
  gtk_widget_add_accelerator(window->colorBtn3,"clicked",accelGroup,GDK_KEY_4,GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);



  bookMenuBtn=gtk_button_new();
   colorBtn0 = gtk_drawing_area_new ();
  gtk_widget_set_size_request (colorBtn0, 24, 24);
   gtk_container_add (GTK_CONTAINER (bookMenuBtn), colorBtn0);
    window->btnColor="white";
  g_signal_connect (colorBtn0, "draw", G_CALLBACK (book_pdf_window_header_colorBtn_draw_selectcolor),  window);
  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), bookMenuBtn);



  bookMenuPopover=gtk_popover_new(bookMenuBtn);

   gtk_container_add (GTK_CONTAINER (bookMenuPopover), vbox);
   //   gtk_widget_show (bookMenuTreeView);
  gtk_widget_show_all (vbox);
  gtk_container_set_border_width (GTK_CONTAINER (bookMenuPopover), 1);

 g_signal_connect (bookMenuBtn, "clicked",  G_CALLBACK (book_pdf_window_menuBtn_action_click), bookMenuPopover);
 gtk_widget_add_accelerator(bookMenuBtn,"activate",accelGroup,GDK_KEY_F10,0,GTK_ACCEL_VISIBLE);

}


GtkWidget*
book_pdf_window_new (GtkApplication *application)
{
  	GtkWidget *window;
  window= g_object_new (BOOK_TYPE_PDF_WINDOW,
                         "type", GTK_WINDOW_TOPLEVEL,
                       "application", g_application_get_default (),
                         "show-menubar", FALSE,
                       NULL);


  return window;
}

static void book_pdf_window_default(BookPdfWindow *window)
{
  	GtkWidget *pageView,*table;
 GtkWidget * scrolled = gtk_scrolled_window_new (NULL, NULL);

    gtk_widget_set_hexpand (scrolled, TRUE);
    gtk_widget_set_vexpand (scrolled, TRUE);
	gtk_container_add(GTK_CONTAINER(window), scrolled);

//pdfWidget= gtk_window_new (GTK_WINDOW_TOPLEVEL);
//pdfWidget=book_pdf_create_widget (window->bookPdf);
  //  	gtk_container_add(GTK_CONTAINER(pdfWidget), book_pdf_create_widget (window->bookPdf,0));
 //gtk_widget_show_all(pdfWidget);


  table = gtk_grid_new ();
    gtk_grid_set_row_spacing(table,5);
  	gtk_container_add(GTK_CONTAINER(scrolled), table);
   gtk_widget_set_halign (table, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (table, GTK_ALIGN_CENTER);
 gint pageNum=book_pdf_get_chapter_num (window->bookPdf);
    gint i=0;
    for(i=0;i<pageNum;i++)
      {
        /*
        pageView= gtk_drawing_area_new ();
        gtk_widget_set_size_request(pageView,pageWidth,pageHeight);
        gtk_grid_attach (GTK_GRID (table), pageView, 0, i, 1, 1);
       window->pageViewList = g_list_append(window->pageViewList, pageView);
         */
       //  g_signal_connect (pageView, "draw",  G_CALLBACK (book_pdf_window_bookPdfView_drawcb), window);
pageView=book_pdf_create_widget (window->bookPdf,i);
        gtk_grid_attach (GTK_GRID (table), pageView , 0, i, 1, 1);
         window->pageViewList = g_list_append(window->pageViewList, pageView);
        gtk_widget_queue_draw (pageView);
      }

 // gtk_widget_show(table);
  //	gtk_widget_queue_draw (table);
  /*
    book_pdf_window_load_surface(window,0);

  for (iterator = window->pageViewList; iterator; iterator = iterator->next) {
    g_signal_connect (iterator->data, "draw",  G_CALLBACK (book_pdf_window_bookPdfView_drawcb), window);
  }
   */
}

GtkWidget * book_pdf_window_create(const gchar *bookFile,GtkWidget * parentWindow,const gchar * initInfo)
{
	BookPdfWindow *window;
	GtkWidget *header;
  	GtkWidget *pdfWidget;
	gchar *bookTitle;
	gchar *bookAuthor;
	GtkWidget * bookPdfView,*pageView;
  GList *iterator;
  GtkWidget *frame;
	BookDoc *bookDoc;
  GError *error;
error=NULL;
  gchar *inputPwd;
  gint pageWidth,pageHeight;
	if (!window) {
    	bookAuthor = NULL;
		window =book_pdf_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_screen(GTK_WINDOW(window), gtk_widget_get_screen(parentWindow));
	//	g_signal_connect (window, "destroy",   G_CALLBACK (book_pdf_window_event_destroy), window);



    window->bookPath=g_strdup(bookFile);
     window->bookPdf=book_pdf_new(bookFile,NULL,&error);
    if(error!=NULL)
      {
          if(error->code ==1 && strcmp("Document is encrypted",error->message)==0)
                             {
            book_log_error ("文件需要密码才能打开:%s\n",error->message);
             inputPwd=book_dialogs_return_password(parentWindow,NULL);
            if(inputPwd!=NULL)
                                  {
                g_clear_object( &window->bookPdf);
                 window->bookPdf=book_pdf_new(bookFile,inputPwd,&error);
                g_free(inputPwd);
               if(error!=NULL)
                                            {
                 return NULL;
                                            }
                                  }
                           }
      }
    pageWidth=book_pdf_get_width (window->bookPdf);
    pageHeight=book_pdf_get_height (window->bookPdf);
   // book_pdf_chapter_get_size (window->bookPdf, 0, &pageWidth, &pageHeight);
    bookTitle=book_pdf_get_metadata(window->bookPdf,BOOK_PDF_META_TITLE);
    bookAuthor=book_pdf_get_metadata(window->bookPdf,BOOK_PDF_META_AUTHOR);
  //_pdfSurface=book_pdf_get_surface (window->bookPdf);


   /*
    bookPdfView = gtk_drawing_area_new ();
  window->bookPdfView=bookPdfView;
		gtk_container_add(GTK_CONTAINER(scrolled), bookPdfView);
      gtk_widget_set_halign (bookPdfView, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (bookPdfView, GTK_ALIGN_CENTER);
    book_pdf_window_load_surface(window,0);
    */
   //gtk_widget_set_size_request (bookPdfView, book_pdf_get_width(window->bookPdf),book_pdf_get_height (window->bookPdf));




		header = gtk_header_bar_new();
		gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
		gtk_header_bar_set_title(GTK_HEADER_BAR(header), bookTitle);
		gtk_window_set_titlebar(GTK_WINDOW(window), header);




		gtk_header_bar_set_title(header, bookTitle);
		gtk_header_bar_set_subtitle(header, bookAuthor);
    	gtk_window_set_default_size(GTK_WINDOW(window),MAX(pageWidth+50,900),MAX(pageHeight+50,500));

     book_pdf_window_default(window);
   //g_thread_create(book_pdf_window_default,window,FALSE,NULL);
    //g_idle_add(book_pdf_window_default,window);
  //g_thread_join(window->threads);
   // gtk_widget_set_size_request (bookPdfView, (gint)600, (gint)500);

	g_signal_connect(window, "key-release-event", G_CALLBACK(book_pdf_window_key_release), window);
g_signal_connect(window, "key-press-event", G_CALLBACK(book_pdf_window_key_press), window);
//    g_signal_connect (window->bookPdfView, "draw",  G_CALLBACK (book_pdf_window_bookPdfView_drawcb), window);
  GtkAccelGroup *accelGroup = gtk_accel_group_new();
 gtk_window_add_accel_group(GTK_WINDOW(window),accelGroup);
 g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(book_pdf_window_event_destroy), G_OBJECT(parentWindow));
gtk_widget_add_accelerator(window,"destroy",accelGroup,GDK_KEY_w,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);


		book_pdf_window_header_menuBtn_create(accelGroup,header, window);
book_pdf_window_header_item_create(accelGroup,header,window);
    book_pdf_window_header_colorBtn_create(accelGroup,header, window);

    //添加
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


gboolean  book_pdf_window_key_release(BookPdfWindow * window, GdkEventKey  *event, 	BookPdfWindow *bookwindow)
{
 // g_printf("key:state=%d,GDK_CONTROL_MASK=%d\n",event->state,GDK_CONTROL_MASK);
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


gboolean  book_pdf_window_key_press(BookPdfWindow * window, GdkEventKey  *event,BookPdfWindow *bookwindow)
{
 if(event->state & GDK_CONTROL_MASK)
      {
         window->isCtrlMask=true;
      }else{
         window->isCtrlMask=false;
      }

	return FALSE;
}

