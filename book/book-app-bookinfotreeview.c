
#include "book-app-bookinfotreeview.h"
#include <glib-object.h>
#include <json-glib/json-glib.h>
#include "book-manage.h"
#include "book-window.h"
enum
{
  APPBOOKID=0,
  APPBOOKTITLE,
  APPBOOKAUTHOR,
  APPBOOKSRC,
  APPBOOKMARK,
  APPBOOKOPENTIME,
  APPBOOKCREATETIME,
  APPBOOK_COLUMNS
};

//定义BookAppBookinfoTreeview
struct _BookAppBookinfoTreeview
{
  GtkTreeView       parent;
  GtkWidget *window;

};

G_DEFINE_TYPE (BookAppBookinfoTreeview, book_app_bookinfo_treeview, GTK_TYPE_TREE_VIEW)

//定义方法
GtkTreeModel *book_app_bookinfo_treeview_model (GSList *bookInfoList);
void book_app_bookinfo_treeview_columns (GtkTreeView *treeview,GtkTreeModel *model);
void book_app_bookinfo_treeview_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                               GtkWidget *window);

static void
book_app_bookinfo_treeview_class_init (BookAppBookinfoTreeviewClass *klass)
{
 // GtkTreeViewClass *object_class = GTK_TREE_VIEW_CLASS (klass);
//	object_class->constructed = book_app_mark_treeview_constructed;
	//object_class->finalize = book_app_mark_treeview_finalize;
}

static void book_app_bookinfo_treeview_init (BookAppBookinfoTreeview *self)
{

}


GtkWidget * book_app_bookinfo_treeview_new(GtkWidget *window,GSList *bookInfoList){
  GtkWidget *treeview;
  GtkTreeModel *model;

  //treeview= g_object_new (BOOK_TYPE_APP_BOOKINFO_TREEVIEW, NULL);

 // treeview->window=window;
  model = book_app_bookinfo_treeview_model(bookInfoList);
 //treeview = gtk_tree_view_new_with_model (model);
   treeview= g_object_new (BOOK_TYPE_APP_BOOKINFO_TREEVIEW, "model", model, NULL);

     gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (treeview), TRUE);
      g_object_unref (model);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                                   GTK_SELECTION_SINGLE);

      book_app_bookinfo_treeview_columns (GTK_TREE_VIEW (treeview),model);

  g_signal_connect (treeview, "realize",G_CALLBACK (gtk_tree_view_expand_all), NULL);
 g_signal_connect(GTK_TREE_VIEW(treeview), "row-activated", (GCallback)  book_app_bookinfo_treeview_row_activated, window);


  return treeview;
}



GtkTreeModel *book_app_bookinfo_treeview_model (GSList *bookInfoList)
{
 GtkTreeStore *model;
  GtkTreeIter iter;
gchar *bookId,*bookTitle,*bookAuthor,*bookSrc,*bookFlag,*bookMark,*bookOpenTime,*bookCreateTime;
   GSList *iterator = NULL;
   GError *error = NULL;
  model = gtk_tree_store_new (APPBOOK_COLUMNS,  G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);

     for (iterator = bookInfoList; iterator; iterator = iterator->next) {
       BookMeta *bookMeta=(BookMeta *)iterator->data;
      g_object_get(G_OBJECT(bookMeta),"metaId",&bookId,"title",&bookTitle,"author",&bookAuthor,"src",&bookSrc,"flag",&bookFlag,"mark",&bookMark,"openTime",&bookOpenTime,"createTime",&bookCreateTime,NULL);
       gtk_tree_store_append (model, &iter, NULL);
      gtk_tree_store_set (model, &iter,APPBOOKID, bookId,APPBOOKTITLE, bookTitle,  APPBOOKAUTHOR,bookAuthor,APPBOOKSRC,bookSrc,APPBOOKMARK,bookMark,APPBOOKOPENTIME,bookOpenTime,APPBOOKCREATETIME,bookCreateTime,-1);
      g_clear_object(&bookMeta);
    }
  g_slist_free(bookInfoList);

  return GTK_TREE_MODEL (model);
}

void book_app_bookinfo_treeview_columns (GtkTreeView *treeview,GtkTreeModel *model)
{
   gint col_offset;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
 // GtkTreeModel *model = gtk_tree_view_get_model (treeview);



  renderer = gtk_cell_renderer_text_new ();
 g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (APPBOOKTITLE));
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1, "书名",renderer, "text",APPBOOKTITLE,NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_resizable (column, TRUE);
  gtk_tree_view_column_set_sort_column_id(column,1);
   gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column), 200);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),  GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);


   renderer = gtk_cell_renderer_text_new ();
 g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (APPBOOKAUTHOR));
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1, "作者",renderer, "text",APPBOOKAUTHOR,NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_sort_column_id(column,2);
  gtk_tree_view_column_set_resizable (column, TRUE);
  gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column), 100);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);

  renderer = gtk_cell_renderer_text_new ();
 g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (APPBOOKOPENTIME));
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1, "上次阅读",renderer, "text",APPBOOKOPENTIME,NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_sort_column_id(column,2);
  gtk_tree_view_column_set_resizable (column, TRUE);
  gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column), 100);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);

  renderer = gtk_cell_renderer_text_new ();
 g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (APPBOOKCREATETIME));
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1, "创建时间",renderer, "text",APPBOOKCREATETIME,NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_sort_column_id(column,2);
  gtk_tree_view_column_set_resizable (column, TRUE);
  gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column), 100);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);

}


 void book_app_bookinfo_treeview_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                               GtkWidget *window)
         {
  printf("book_application_panedLeft_treeview_row_activated\n");
             GtkTreeModel *model;
             GtkTreeIter iter;
           GtkWidget * win;
             model = gtk_tree_view_get_model (treeview);
           gchar *ext;
             if (gtk_tree_model_get_iter (model, &iter, path))
             {

               gchar *bookId,*bookPath,*bookMark;
               gtk_tree_model_get(model, &iter, APPBOOKID, &bookId, APPBOOKSRC,&bookPath,-1);
                g_printf("当前的节点BookId=%s\n",bookId);

             //  BookInfo *bookInfo= book_database_get_bookInfo (bookId);
              // bookMark=bookInfo->bookMark;
               BookMeta* bookMeta= book_manage_get_bookmeta (bookId);
               g_object_get(G_OBJECT(bookMeta),"mark",&bookMark,NULL);
               //gtk_tree_model_get(model, &iter, BOOKMARK,&bookMark, -1);
              g_printf("当前的节点SRC=%s,mark=%s\n",bookPath,bookMark);

               	ext = strrchr(bookPath, '.');
               if(strcmp(ext,".pdf") == 0)
                 {
                   win=book_pdf_window_create (bookPath, window,bookMark);
                 }else{
                  win=book_window_create (bookPath, window,bookMark);
                   }
               gtk_widget_show_all(win);
               //gtk_window_present (GTK_WINDOW(win));
               g_free(bookId);
               //g_free(bookInfo);


               /*  GtkWidget * win=book_window_create ("/boxes/bookfere/zh/国学/孟子讲读（国学名著讲读系列）- 赵杏根.azw3", window,NULL); */
               /* gtk_widget_show_all(win); */
             }
  }

GtkWidget * book_app_bookinfo_treeview_reset_list(BookAppBookinfoTreeview *treeview,GSList *bookInfoList){

  GtkTreeModel *model;
    GtkTreeIter iter;
   GSList *iterator = NULL;
  gchar *bookId,*bookTitle,*bookAuthor,*bookSrc,*bookFlag,*bookMark,*bookOpenTime,*bookCreateTime;
  model = gtk_tree_view_get_model (treeview);
gtk_tree_store_clear(model);
  for (iterator = bookInfoList; iterator; iterator = iterator->next) {
       BookMeta *bookMeta=(BookMeta *)iterator->data;
    book_meta_toString(bookMeta);
      g_object_get(G_OBJECT(bookMeta),"metaId",&bookId,"title",&bookTitle,"author",&bookAuthor,"src",&bookSrc,"flag",&bookFlag,"mark",&bookMark,"openTime",&bookOpenTime,"createTime",&bookCreateTime,NULL);
       gtk_tree_store_append (model, &iter, NULL);
      gtk_tree_store_set (model, &iter,APPBOOKID, bookId,APPBOOKTITLE, bookTitle,  APPBOOKAUTHOR,bookAuthor,APPBOOKSRC,bookSrc,APPBOOKMARK,bookMark,APPBOOKOPENTIME,bookOpenTime,APPBOOKCREATETIME,bookCreateTime,-1);
      g_clear_object(&bookMeta);
    }
  //g_slist_free(bookInfoList);
}
