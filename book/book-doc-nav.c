#include "book-doc-nav.h"

/* columns */
enum
{
  HOLIDAY_NAME_COLUMN = 0,
  ID,
  SRC,
  NUM_COLUMNS
};

static void book_doc_nav_treeview_event_rowActivated(GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,BookWebview *webview)
{
   GtkTreeModel *model,*booklistmodel;
             GtkTreeIter iter;
           GtkTreeSelection *select;
             gchar *value;

  select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
            if (gtk_tree_selection_get_selected (select, &model, &iter))
        {
                gtk_tree_model_get (model, &iter, SRC, &value, -1);
            book_webview_nav_go(webview,value);
                //g_print ("菜单地址=%s\n", value);

        }
}
void book_doc_nav_treeview_add_navPoint_model(GtkTreeStore *model,GtkTreeIter *parentIter,BookNavPoint *navPoint)
{
  GtkTreeIter iter;
      //g_printf("标题：%s(id:%s,playOrder:%d,src:%s)\n", navPoint->text, navPoint->id, navPoint->playOrder, navPoint->src);

      gtk_tree_store_append (model, &iter, parentIter);
      //g_printf("树节点：%s \n",navPoint->text);
      gtk_tree_store_set (model, &iter,
                          HOLIDAY_NAME_COLUMN, navPoint->text,
                          ID,navPoint->id,
                          SRC,navPoint->src,
                          -1);
     if(navPoint->hasChildren)
        book_doc_nav_treeview_add_navPoint_model(model,&iter,navPoint->children);
      if(navPoint->hasNext)
        {
          book_doc_nav_treeview_add_navPoint_model(model,parentIter,navPoint->next);
        }


}

static GtkTreeModel *
book_doc_nav_treeview_model (BookDoc *doc)
{
  GtkTreeStore *model;
  GtkTreeIter iter;

  /* create tree store */
  model = gtk_tree_store_new (NUM_COLUMNS,
                              G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);

  /* add data to the tree store */
  printf("加载菜单\n");
BookNavPoint  *navPoint=book_doc_get_nav (doc);
  printf("这里出问题了\n");
if(navPoint && navPoint != NULL )
    {
     printf("有菜单数据哦：\n");
      BookNavPoint  *bkNavPoint=navPoint;
      if(navPoint->hasChildren)
      {
       book_doc_nav_treeview_add_navPoint_model(model,NULL,navPoint->children);
      }
       g_free(bkNavPoint);
    }
  //   g_free(navPoint);

printf("加载菜单数据完成\n");
  return GTK_TREE_MODEL (model);
}

static void
book_doc_nav_treeview_add_columns (GtkTreeView *treeview)
{
  gint col_offset;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeModel *model = gtk_tree_view_get_model (treeview);

  /* column for holiday names */
  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "xalign", 0.0, NULL);

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, "目录",
                                                            renderer, "text",
                                                            HOLIDAY_NAME_COLUMN,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);

}

GtkWidget  *book_doc_nav_treeview_new(BookWebview *webview){


      GtkWidget *treeview;
      GtkTreeModel *model;
    BookDoc *doc=book_webview_get_doc(webview);
      model = book_doc_nav_treeview_model (doc);

      /* create tree view */
      treeview = gtk_tree_view_new_with_model (model);
      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (treeview), FALSE);
      g_object_unref (model);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                                   GTK_SELECTION_SINGLE);

      book_doc_nav_treeview_add_columns (GTK_TREE_VIEW (treeview));

      g_signal_connect (treeview, "realize",  G_CALLBACK (gtk_tree_view_expand_all), NULL);

    g_signal_connect(GTK_TREE_VIEW(treeview), "row-activated", (GCallback)  book_doc_nav_treeview_event_rowActivated, webview);


  return treeview;
}


