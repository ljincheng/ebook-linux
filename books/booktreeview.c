#include "booktreeview.h"
#include "bookwindow.h"
#include <glib.h>
#include <glib/gstdio.h>
#include "bookdatabase.h"



static void
add_book_list_columns (GtkTreeView *treeview)
{
  gint col_offset;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeModel *model = gtk_tree_view_get_model (treeview);

  /* column for holiday names */
  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "xalign", 0.0, NULL);

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, "书名",
                                                            renderer, "text",
                                                            BOOK_NAME_COLUMN,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_resizable (column, TRUE);
   gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column),
                                 300);

  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
   gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
                                   GTK_TREE_VIEW_COLUMN_FIXED);
gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN (column),300);
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, "作者",
                                                            renderer, "text",
                                                            AUTHOR,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
  gtk_tree_view_column_set_resizable (column, TRUE);
   gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
                                   GTK_TREE_VIEW_COLUMN_FIXED);
gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN (column),120);
   col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, "类别",
                                                            renderer, "text",
                                                            SORT,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_resizable (column, TRUE);
   gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
                                   GTK_TREE_VIEW_COLUMN_FIXED);
gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN (column),80);
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, "文件类型",
                                                            renderer, "text",
                                                            EXT,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_resizable (column, TRUE);
   gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
                                   GTK_TREE_VIEW_COLUMN_FIXED);
gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN (column),40);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}

static void
add_book_menu_columns (GtkTreeView *treeview)
{
  gint col_offset;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeModel *model = gtk_tree_view_get_model (treeview);

  /* column for holiday names */
  renderer = gtk_cell_renderer_text_new ();
 // g_object_set (renderer, "bookmenus", 0.0, NULL);

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, "专栏",
                                                            renderer, "text",
                                                            MENUTITLE,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_resizable (column, TRUE);
   gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column),
                                 100);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
                                   GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);

    
}

  static void
        book_list_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                               GtkWidget *epub_widget)
         {
             GtkTreeModel *model;
             GtkTreeIter iter;
             model = gtk_tree_view_get_model (treeview);
             if (gtk_tree_model_get_iter (model, &iter, path))
             {
                 /* Handle the selection ... */
               gchar *value;

               gtk_tree_model_get(model, &iter, SRC, &value,  -1);
                 GtkWidget *epubwindow;
             epubwindow =book_open_epub (value);
           if(epubwindow!=NULL)
                                  {
             gtk_widget_show_all(epubwindow);
                                 }
                 g_free(value);

             }
         }

  static void
        book_menu_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                               GtkWidget *booklistTreeview)
         {
         //  g_printf("树节点被选中\n");
             GtkTreeModel *model,*booklistmodel;
             GtkTreeIter iter;
             model = gtk_tree_view_get_model (treeview);
             if (gtk_tree_model_get_iter (model, &iter, path))
             {
                 /* Handle the selection ... */
               gchar *value;

               gtk_tree_model_get(model, &iter, MENUID, &value,  -1);
             // g_printf("当前的节点SRC=%s\n",value);
  booklistmodel = gtk_tree_view_get_model (booklistTreeview);
gtk_tree_store_clear(booklistmodel);
               book_menu_list_menuid(value,booklistTreeview);
           // printf("epub file:%s\n",value);
             
                 g_free(value);

             }
         }


GtkWidget * book_get_book_treeview (GtkWidget *do_widget){

 GtkTreeModel *model;
  GtkWidget *treeview;
 model = create_book_menu_all_model ();

      treeview = gtk_tree_view_new_with_model (model);
     gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (treeview), TRUE);
      g_object_unref (model);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                                   GTK_SELECTION_SINGLE);

      add_book_list_columns (GTK_TREE_VIEW (treeview));

  g_signal_connect (treeview, "realize",
                        G_CALLBACK (gtk_tree_view_expand_all), NULL);
    g_signal_connect(GTK_TREE_VIEW(treeview), "row-activated", (GCallback)  book_list_row_activated, do_widget);

return treeview;

}

GtkWidget * book_get_mymenu_treeview (GtkWidget *booklistTreeview)
{
 GtkTreeModel *model;
  GtkWidget *treeview;
 model = create_book_menu_model();

      treeview = gtk_tree_view_new_with_model (model);
     gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (treeview), TRUE);
      g_object_unref (model);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                                   GTK_SELECTION_SINGLE);

      add_book_menu_columns (GTK_TREE_VIEW (treeview));

  g_signal_connect (treeview, "realize",
                        G_CALLBACK (gtk_tree_view_expand_all), NULL);
    g_signal_connect(GTK_TREE_VIEW(treeview), "row-activated", (GCallback)  book_menu_row_activated, booklistTreeview);

return treeview;
}

