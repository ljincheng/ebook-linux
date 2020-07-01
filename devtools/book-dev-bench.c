#include "book-dev-bench.h"

#include <webkit2/webkit2.h>
#include "book-common.h"
#include "book-dev-toollist.h"
#include "book-dev-table.h"

struct _BookDevBench {
GtkBox parent;
GtkWidget *window;
GtkAccelGroup *accelGroup;
WebKitWebView *webview;
};


G_DEFINE_TYPE (BookDevBench, book_dev_bench, GTK_TYPE_BOX)

static WebKitSettings *webkit_settings = NULL;
static gpointer
book_dev_bench_webkit_initsettings (gpointer user_data)
{
  guint i;

  webkit_settings = webkit_settings_new_with_settings ("enable-developer-extras", TRUE,
                                                       "enable-fullscreen", TRUE,
                                                       "enable-javascript", TRUE,
                                                       "enable-dns-prefetching", TRUE,
                                                       "enable-mediasource", TRUE,
                                                       "javascript-can-open-windows-automatically", TRUE,
                                                       NULL);

  webkit_settings_set_hardware_acceleration_policy (webkit_settings, WEBKIT_HARDWARE_ACCELERATION_POLICY_NEVER);

  return webkit_settings;
}


static void book_dev_bench_init (BookDevBench *self)
{
self->accelGroup=NULL;
}

static void book_dev_bench_finalize (GObject *object)
{
  book_log_debug("[BookBench对象退出清理内存]\n");
   //BookBench *self = BOOK_BENCH(object);

  G_OBJECT_CLASS (book_dev_bench_parent_class)->finalize (object);
}


static void
book_dev_bench_class_init (BookDevBenchClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_dev_bench_finalize;

}

GtkWidget* book_dev_bench_new(GtkWidget* window){
  BookDevBench *obj;
   GtkWidget * scrolled,*hpaned;
  GtkWidget *devtoollist;
    GtkWidget *devTable;
  obj=g_object_new(BOOK_TYPE_DEV_BENCH,NULL);
  obj->window=window;


  hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start (GTK_BOX (obj), hpaned, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER(hpaned), 0);


  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (scrolled, TRUE);
gtk_widget_set_vexpand (scrolled, TRUE);

 gtk_widget_set_size_request (scrolled, 200, 60);
 gtk_paned_add1(GTK_PANED (hpaned), scrolled);

  devtoollist=book_dev_toollist_new();

   gtk_widget_set_size_request (devtoollist, 200, 60);
  gtk_container_add (GTK_CONTAINER (scrolled), devtoollist);

   scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (scrolled, TRUE);
gtk_widget_set_vexpand (scrolled, TRUE);

 gtk_widget_set_size_request (scrolled, 200, 60);
 gtk_paned_add2(GTK_PANED (hpaned), scrolled);

  devTable=book_dev_table_new(window);
  gtk_container_add (GTK_CONTAINER (scrolled), devTable);

  return obj;
}


 
