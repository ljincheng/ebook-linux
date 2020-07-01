

#include "book-app-window.h"
#include "book-app-headerbar.h"
//#include "book-application-window-panedleft.h"
//#include "book-application-window-panedright.h"
#include "book-app-marktreeview.h"
#include "book-app-bookinfotreeview.h"
#include "book-manage.h"
#include "widgets/book-bench.h"
#include "widgets/book-erm.h"
#include "book-devtools.h"

/**
 * 水平布局左边面板
 */
GtkWidget *book_application_window_panedLeft_init(GtkWidget *windowPaned){
  GtkWidget * menuscrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (menuscrolled, TRUE);
gtk_widget_set_vexpand (menuscrolled, TRUE);

 gtk_widget_set_size_request (menuscrolled, 300, 60);
 gtk_paned_add1 (GTK_PANED (windowPaned), menuscrolled);
}

/**
 * 水平布局右边面板
 */
GtkWidget *book_application_window_panedRight_init(GtkWidget *windowPaned){

   GtkWidget * overlay = gtk_overlay_new ();
   GtkWidget * scrolled = gtk_scrolled_window_new (NULL, NULL);

    gtk_widget_set_hexpand (scrolled, TRUE);
    gtk_widget_set_vexpand (scrolled, TRUE);
    gtk_paned_add2 (GTK_PANED (windowPaned), overlay);
    gtk_container_add (GTK_CONTAINER (overlay), scrolled);
}


struct _BookAppWindow {
	GtkApplicationWindow parent;
  BookAppHeaderbar *headerBar;
  GtkWidget *markTreeview;
  GtkWidget *mainTreeview;
  GtkWidget *localFrame;
  GtkWidget *netFrame;
};

G_DEFINE_TYPE (BookAppWindow, book_app_window, GTK_TYPE_APPLICATION_WINDOW)

void book_appwindow_marktreeview_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                               GtkWidget *booklistTreeview);
static void book_appwindow_headerbar_action_searchChanged (GtkWidget *entry,BookAppWindow *window);

static void
book_app_window_init(BookAppWindow *self)
{
  GtkWidget *windowBoxVertical;//主窗口的box
  GtkWidget *windowPaned;//主窗口水平布局
  GtkWidget * scrolled;
  GtkWidget * searchEntry;
  GtkWidget * leftSeparator;
  GtkWidget *headerStackSwitcher,*headerTitleBox,*stack;
  GtkWidget *tab1;
  gtk_widget_init_template (GTK_WIDGET (self));
  self->headerBar=NULL;
  self->markTreeview=NULL;
  self->mainTreeview=NULL;


  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (self), FALSE);
  gtk_window_set_default_size ((GtkWindow*)self, 1000, 600);
  gtk_window_set_icon_name (GTK_WINDOW (self),"view-dual-symbolic");

  //添加header bar
  self->headerBar=gtk_header_bar_new ();
   gtk_header_bar_set_show_close_button (GTK_HEADER_BAR(self->headerBar), TRUE);
   headerTitleBox= gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_style_context_add_class(gtk_widget_get_style_context(headerTitleBox), "linked");
  gtk_header_bar_set_custom_title(self->headerBar,headerTitleBox);
   gtk_window_set_titlebar(GTK_WINDOW (self), self->headerBar);

  searchEntry= gtk_search_entry_new ();
  gtk_header_bar_pack_end(GTK_HEADER_BAR(self->headerBar), searchEntry);


  headerStackSwitcher=gtk_stack_switcher_new();
  gtk_container_add (GTK_CONTAINER (headerTitleBox), headerStackSwitcher);

  stack = gtk_stack_new ();
  gtk_stack_set_transition_type (GTK_STACK (stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
  gtk_stack_switcher_set_stack(headerStackSwitcher,stack);


  //APPAPINNER = gtk_spinner_new ();
  // gtk_header_bar_pack_end(GTK_HEADER_BAR(self->headerBar),APPAPINNER);

  self->localFrame=book_bench_new(self);
  gtk_frame_set_shadow_type (GTK_FRAME (self->localFrame), GTK_SHADOW_NONE);
   gtk_stack_add_named (GTK_STACK (stack), self->localFrame, "本地");
  gtk_container_child_set (GTK_CONTAINER (stack), self->localFrame, "title", "本地", NULL);

  //gtk_container_add (GTK_CONTAINER (self), self->localFrame);
  //self->netFrame=gtk_frame_new (NULL);
  self->netFrame=book_erm_new(self);
  //gtk_frame_set_shadow_type (GTK_FRAME (self->netFrame), GTK_SHADOW_NONE);
  //gtk_container_add (GTK_CONTAINER (self), self->netFrame);
 gtk_stack_add_named (GTK_STACK (stack), self->netFrame, "微信");
  gtk_container_child_set (GTK_CONTAINER (stack), self->netFrame, "title", "微信", NULL);
gtk_container_add (GTK_CONTAINER (self), stack);

  tab1=book_dev_bench_new(self);
 gtk_stack_add_named (GTK_STACK (stack), tab1, "开发工具");
  gtk_container_child_set (GTK_CONTAINER (stack), tab1, "title", "开发工具", NULL);
gtk_container_add (GTK_CONTAINER (self), stack);

  //添加水平布局


g_signal_connect (searchEntry, "search-changed", G_CALLBACK (book_appwindow_headerbar_action_searchChanged), self);


  //设置快捷键
  GtkAccelGroup *accelGroup = gtk_accel_group_new();
 gtk_window_add_accel_group(GTK_WINDOW(self),accelGroup);
gtk_widget_add_accelerator(stack,gtk_stack_get_child_by_name(stack,"书库"),accelGroup,GDK_KEY_1,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(stack,gtk_stack_get_child_by_name(stack,"本地"),accelGroup,GDK_KEY_2,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
  //gtk_widget_add_accelerator(self->markTreeview,"grab-focus",accelGroup,GDK_KEY_F5,0,GTK_ACCEL_VISIBLE);
  //gtk_widget_add_accelerator(self->mainTreeview,"grab-focus",accelGroup,GDK_KEY_F6,0,GTK_ACCEL_VISIBLE);
  book_bench_set_accel(self->localFrame,accelGroup);
  gtk_widget_add_accelerator(searchEntry,"grab-focus",accelGroup,GDK_KEY_f,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

}

static void
book_app_window_constructed(GObject *object)
{
	BookAppWindow *widget = BOOK_APP_WINDOW (object);
 G_OBJECT_CLASS (book_app_window_parent_class)->constructed (object);
	}

static void book_app_window_finalize(GObject *object)
{
	BookAppWindow *widget = BOOK_APP_WINDOW(object);
	G_OBJECT_CLASS(book_app_window_parent_class)->finalize(object);


}


static void
book_app_window_class_init(BookAppWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->constructed = book_app_window_constructed;
	object_class->finalize = book_app_window_finalize;
	//object_class->set_property = book_widget_set_property;
	//object_class->get_property = book_widget_get_property;

}

GtkWidget*
book_app_window_new (GtkApplication *application)
{
  	GtkWidget *window;
  window= g_object_new (BOOK_TYPE_APP_WINDOW,
                       "application", application,
                       NULL);
  return window;
}

GtkWidget * book_appwindow_get_frame(BookAppWindow *window,gint index){
  GtkWidget *frame;
  switch (index)
    {
    case 0:
     frame= window->localFrame;
      break;
      case 1:
     frame= window->netFrame;
      break;
      default:
        frame=NULL;
        break;
    }
  return frame;
}

GtkWidget * book_appwindow_get_headerBar(BookAppWindow *window){
  return window->headerBar;
}

GtkWidget * book_appwindow_get_markTreeview(BookAppWindow *window){
  return window->markTreeview;
}

GtkWidget * book_appwindow_get_mainTreeview(BookAppWindow *window){
  return window->mainTreeview;
}

void book_appwindow_marktreeview_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                               GtkWidget *booklistTreeview)
         {
  printf("book_application_panedLeft_treeview_row_activated\n");
             GtkTreeModel *model;
             GtkTreeIter iter;
           gchar *value;
            GSList *bookInfoList;
             model = gtk_tree_view_get_model (treeview);
             if (gtk_tree_model_get_iter (model, &iter, path))
             {
book_app_mark_treeview_get_row_flag(model, &iter, &value);
              g_printf("当前的节点SRC=%s\n",value);
if(strcmp("-1",value)==0)
                 {
                   bookInfoList=book_manage_all_bookmeta (NULL);
                 }else if(strcmp("-2",value)==0)
                 {
                   bookInfoList=book_manage_lastRead_bookmeta ();
                   }else if(strcmp("-3",value)==0)
                 {
                   bookInfoList=book_manage_nullFlag_bookmeta ();
                 }else{
          bookInfoList=book_manage_list_bookmeta (atoi(value),NULL);
                 }
               book_log_debug("获取flag=%s\n",value);

book_app_bookinfo_treeview_reset_list(booklistTreeview,bookInfoList);
               g_slist_free(bookInfoList);

                 g_free(value);

             }
  }

// 搜索框事件
static void book_appwindow_headerbar_action_searchChanged (GtkWidget *entry,BookAppWindow *window)
{
  const char *text;
   GSList *bookInfoList;
  text = gtk_entry_get_text (GTK_ENTRY (entry));
     g_message ("search changed: %s", text);
 //bookInfoList=book_manage_query_bookmeta (text);
//book_app_bookinfo_treeview_reset_list(window->netFrame,bookInfoList);
  book_bench_query_key(window->localFrame,text);
             //  g_slist_free(bookInfoList);
}
