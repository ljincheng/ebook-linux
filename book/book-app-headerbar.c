
#include "book-app-headerbar.h"
#include "book-httpclient.h"
#include "book-test-window.h"
/**
 *工具栏二选一按键事件
 */
static void book_app_headerbar_titlebar_action_selectedSegment(GtkWidget * btn,GtkWidget *btn2)
{
  gtk_widget_set_sensitive (btn, FALSE);
  gtk_widget_set_sensitive (btn2, TRUE);

//book_http_client_test();
//book_pdf_window_test ();
 book_test_window_create ();
  }

//定义BookAppHeaderBar
struct _BookAppHeaderbar
{
  GtkHeaderBar       parent;
  GtkWidget *titleBtn_first;
  GtkWidget *titleBtn_second;
  GtkWidget *searchEntry;
};

G_DEFINE_TYPE (BookAppHeaderbar, book_app_headerbar, GTK_TYPE_HEADER_BAR)

static void
book_app_headerbar_class_init (BookAppHeaderbarClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

}

static void book_app_headerbar_init (BookAppHeaderbar *self)
{
  GtkWidget *headerTitleBox;
  GtkWidget *searchEntry,*settingBtn;
GtkWidget *stackSwitcher;
  GtkWidget *stack;

   gtk_header_bar_set_show_close_button(self, TRUE);
   headerTitleBox= gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_style_context_add_class(gtk_widget_get_style_context(headerTitleBox), "linked");
  gtk_header_bar_set_custom_title(self,headerTitleBox);

stackSwitcher=gtk_stack_switcher_new();
  gtk_container_add (GTK_CONTAINER (headerTitleBox), stackSwitcher);

   stack = gtk_stack_new ();
  gtk_stack_set_transition_type (GTK_STACK (stack), GTK_STACK_TRANSITION_TYPE_NONE);
gtk_stack_switcher_set_stack(stackSwitcher,stack);

   	//本地按扭
  self->titleBtn_first =gtk_button_new_with_label("本地");
  //gtk_container_add (GTK_CONTAINER (headerTitleBox), self->titleBtn_first);
 //gtk_widget_set_sensitive (self->titleBtn_first, FALSE);
  gtk_stack_add_named (GTK_STACK (stack), self->titleBtn_first, "本地");
  gtk_container_child_set (GTK_CONTAINER (stack), self->titleBtn_first, "title", "本地", NULL);

  //网络按键
  self->titleBtn_second=gtk_button_new_with_label("微信");
  //gtk_container_add (GTK_CONTAINER (headerTitleBox), self->titleBtn_second);
    gtk_stack_add_named (GTK_STACK (stack), self->titleBtn_second, "本地");
  gtk_container_child_set (GTK_CONTAINER (stack), self->titleBtn_second, "title", "微信", NULL);

  self->searchEntry= gtk_search_entry_new ();
  gtk_header_bar_pack_end(GTK_HEADER_BAR(self), self->searchEntry);


 g_signal_connect (self->titleBtn_first, "clicked",  G_CALLBACK (book_app_headerbar_titlebar_action_selectedSegment), self->titleBtn_second);
 g_signal_connect (self->titleBtn_second, "clicked",  G_CALLBACK (book_app_headerbar_titlebar_action_selectedSegment), self->titleBtn_first);
}


GtkWidget * book_app_header_bar_new(){
  GtkWidget *headerBar;
  headerBar= g_object_new (BOOK_TYPE_APP_HEADERBAR, NULL);
  return headerBar;
}

GtkWidget * book_app_header_bar_get_titleBtn(BookAppHeaderbar *headerBar,gint index){
  GtkWidget *titleBtn;
    switch (index)
      {
      case 0:
      titleBtn=headerBar->titleBtn_first;
        break;
      case 1:
        titleBtn=headerBar->titleBtn_second;
        break;
      }

  return titleBtn;
}

GtkWidget * book_app_header_bar_get_searchEntry(BookAppHeaderbar *headerBar){
  return headerBar->searchEntry;
}
 
