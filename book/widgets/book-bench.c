#include "book-bench.h"
#include "book-common.h"
#include "book-app-marktreeview.h"
#include "book-app-bookinfotreeview.h"
#include "book-manage.h"

struct _BookBench {
GtkBox parent;
GtkWidget *markTreeview;
GtkWidget *mainTreeview;
GtkWidget *window;
  GtkAccelGroup *accelGroup;
};


G_DEFINE_TYPE (BookBench, book_bench, GTK_TYPE_BOX)

void book_bench_marktreeview_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                               BookBench *bookBench);

static void book_bench_view_create (BookBench *self){
GtkWidget * scrolled,*hpaned;

  hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start (GTK_BOX (self), hpaned, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER(hpaned), 0);


  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (scrolled, TRUE);
gtk_widget_set_vexpand (scrolled, TRUE);

 gtk_widget_set_size_request (scrolled, 200, 60);
 gtk_paned_add2(GTK_PANED (hpaned), scrolled);

  GSList *bookInfoList;
   bookInfoList=book_manage_lastRead_bookmeta ();
  self->mainTreeview=book_app_bookinfo_treeview_new(self->window,bookInfoList);
   gtk_widget_set_size_request (self->mainTreeview, 200, 60);
  gtk_container_add (GTK_CONTAINER (scrolled), self->mainTreeview);


   scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (scrolled, TRUE);
gtk_widget_set_vexpand (scrolled, TRUE);

 gtk_widget_set_size_request (scrolled, 200, 60);
 gtk_paned_add1 (GTK_PANED (hpaned), scrolled);

  gchar *data=book_manage_query_param ("bookdirectory");
  self->markTreeview=book_app_mark_treeview_new(data);
   gtk_widget_set_size_request (self->markTreeview, 200, 60);
  gtk_container_add (GTK_CONTAINER (scrolled), self->markTreeview);

   g_signal_connect(GTK_TREE_VIEW(self->markTreeview), "row-activated", (GCallback)  book_bench_marktreeview_row_activated, self);
}

static void book_bench_init (BookBench *self)
{
self->accelGroup=NULL;
  self->markTreeview=NULL;
  self->mainTreeview=NULL;
}

static void book_bench_finalize (GObject *object)
{
  book_log_debug("[BookBench对象退出清理内存]\n");
   //BookBench *self = BOOK_BENCH(object);

  G_OBJECT_CLASS (book_bench_parent_class)->finalize (object);
}


static void
book_bench_class_init (BookBenchClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_bench_finalize;

}



GtkWidget* book_bench_new(GtkWidget* window){
  BookBench *obj;
  obj=g_object_new(BOOK_TYPE_BENCH,NULL);
  obj->window=window;
  book_bench_view_create(obj);
  return obj;
}



void book_bench_marktreeview_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                                BookBench *bookBench)
         {
  printf("book_application_panedLeft_treeview_row_activated\n");
             GtkTreeModel *model;
             GtkTreeIter iter;
           gchar *value;
            GSList *bookInfoList;
           GtkWidget *booklistTreeview;
           booklistTreeview=bookBench->mainTreeview;
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


  void book_bench_query_key(BookBench *bookBench,const gchar * keyStr){

   GSList *bookInfoList;
 bookInfoList=book_manage_query_bookmeta (keyStr);
book_app_bookinfo_treeview_reset_list(bookBench->mainTreeview,bookInfoList);
  }
  void book_bench_set_accel(BookBench *bookBench,GtkAccelGroup *accelGroup ){

    if(bookBench->accelGroup==NULL)
      {
        bookBench->accelGroup=accelGroup;
        gtk_widget_add_accelerator(bookBench->markTreeview,"grab-focus",accelGroup,GDK_KEY_F5,0,GTK_ACCEL_VISIBLE);
      gtk_widget_add_accelerator(bookBench->mainTreeview,"grab-focus",accelGroup,GDK_KEY_F6,0,GTK_ACCEL_VISIBLE);
      }


  }
