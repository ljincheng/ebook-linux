#include <gtk/gtk.h>
#include <libgepub/gepub.h>
#include "bookmenu.h"
#include "bookwindow.h"

/* TreeItem structure */


/* columns */
enum
{
  HOLIDAY_NAME_COLUMN = 0,
  ID,
  SRC,
  NUM_COLUMNS
};



static gboolean  deal_key_press(GtkWidget * windowwidget, GdkEventKey  *event, GepubWidget *widget)
{
	switch (event->keyval) {
		case GDK_KEY_F2:
case GDK_KEY_Escape:
		{
		 gtk_widget_destroy(windowwidget);
		}
		break;
		default:
			break;
	}

	return FALSE;
}


void add_navPoint_model(GtkTreeStore *model,GtkTreeIter *parentIter,GepubNavPoint *navPoint)
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
        add_navPoint_model(model,&iter,navPoint->children);
      if(navPoint->hasNext)
        {
          add_navPoint_model(model,parentIter,navPoint->next);
        }


}

static GtkTreeModel *
create_epub_nav_model (GepubDoc *doc)
{
  GtkTreeStore *model;
  GtkTreeIter iter;

  /* create tree store */
  model = gtk_tree_store_new (NUM_COLUMNS,
                              G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);

  /* add data to the tree store */
GepubNavPoint  *navPoint=gepub_nav_get_list (doc);
if(navPoint && navPoint != NULL )
    {
      GepubNavPoint  *bkNavPoint=navPoint;
      if(navPoint->hasChildren)
      {
       add_navPoint_model(model,NULL,navPoint->children);
      }
       g_free(bkNavPoint);
    }
  //   g_free(navPoint);


  return GTK_TREE_MODEL (model);
}



static GtkTreeModel *
create_book_nav_model (BookDoc *doc)
{
  GtkTreeStore *model;
  GtkTreeIter iter;

  /* create tree store */
  model = gtk_tree_store_new (NUM_COLUMNS,
                              G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);

  /* add data to the tree store */
  printf("加载菜单\n");
GepubNavPoint  *navPoint=book_get_nav_list(doc);
  printf("这里出问题了\n");
if(navPoint && navPoint != NULL )
    {
     printf("有菜单数据哦：\n");
      GepubNavPoint  *bkNavPoint=navPoint;
      if(navPoint->hasChildren)
      {
       add_navPoint_model(model,NULL,navPoint->children);
      }
       g_free(bkNavPoint);
    }
  //   g_free(navPoint);

printf("加载菜单数据完成\n");
  return GTK_TREE_MODEL (model);
}


static void
item_toggled (GtkCellRendererToggle *cell,
              gchar                 *path_str,
              gpointer               data)
{
  g_printf("path=%s\n",path_str);
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  GtkTreeIter iter;
  gboolean toggle_item;

  gint *column;
  gchar *value;

  column = g_object_get_data (G_OBJECT (cell), "column");

  /* get toggled iter */
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, column, &toggle_item, -1);
  gtk_tree_model_get(model, &iter, SRC, &value,  -1);
  g_printf("当前的节点SRC=%s\n",value);

  /* do something with the value */
  toggle_item ^= 1;

  /* set new value */
  gtk_tree_store_set (GTK_TREE_STORE (model), &iter, column,
                      toggle_item, -1);

  /* clean up */
  gtk_tree_path_free (path);
}

  static void
        menu_row_activated (GtkTreeView *treeview,
                                GtkTreePath *path,
                                GtkTreeViewColumn *column,
                               BookWidget *epub_widget)
         {
          // g_printf("树节点被选中\n");
             GtkTreeModel *model;
             GtkTreeIter iter;
             model = gtk_tree_view_get_model (treeview);
             if (gtk_tree_model_get_iter (model, &iter, path))
             {
                 /* Handle the selection ... */
               gchar *value;

               gtk_tree_model_get(model, &iter, SRC, &value,  -1);
 // g_printf("当前的节点SRC=%s\n",value);
               book_widget_load_uri(epub_widget,value);
                 g_free(value);
		book_window_page_change(epub_widget);

             }
         }

gboolean selection_changed(GtkTreeSelection *selection, GtkLabel *label) {
 GtkTreeView *treeView;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *active;
  gchar *value;

    treeView = gtk_tree_selection_get_tree_view(selection);
    model = gtk_tree_view_get_model(treeView);
    gtk_tree_selection_get_selected(selection, &model, &iter);
 gtk_tree_model_get(model, &iter, SRC, &value,  -1);
 // g_printf("当前的节点SRC=%s\n",value);
}

static void
editing_started (GtkCellRenderer *cell,
                 GtkCellEditable *editable,
                 const gchar     *path,
                 gpointer         data)
{

  g_print("editing_started:OK\n");

}
static void
add_columns (GtkTreeView *treeview)
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

GtkWidget *
do_tree_store (GtkWidget *epub_widget)
{
  static GtkWidget *window = NULL;

  	GepubDoc *doc = gepub_widget_get_doc(epub_widget);
		gint chapternum = gepub_doc_get_chapter(doc);
		gint nchapternum = gepub_doc_get_n_chapters(doc);
  	gchar *title = gepub_doc_get_metadata(doc, GEPUB_META_TITLE);
	gchar *author=gepub_doc_get_metadata(doc, GEPUB_META_AUTHOR);

  if (!window)
    {
      GtkWidget *vbox;
      GtkWidget *sw;
      GtkWidget *treeview;
      GtkTreeModel *model;

      /* create window, etc */
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_screen (GTK_WINDOW (window),
                             gtk_widget_get_screen (epub_widget));
      gtk_window_set_title (GTK_WINDOW (window), author);
      g_signal_connect (window, "destroy",
                        G_CALLBACK (gtk_widget_destroyed), &window);
	g_signal_connect(window, "key-press-event", G_CALLBACK(deal_key_press), doc);

      vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
      gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
      gtk_container_add (GTK_CONTAINER (window), vbox);

      gtk_box_pack_start (GTK_BOX (vbox),    gtk_label_new (title),
                          FALSE, FALSE, 0);

      sw = gtk_scrolled_window_new (NULL, NULL);
      gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                           GTK_SHADOW_ETCHED_IN);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);
      gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);

      /* create model */
      model = create_epub_nav_model (doc);

      /* create tree view */
      treeview = gtk_tree_view_new_with_model (model);
      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (treeview), FALSE);
      g_object_unref (model);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                                   GTK_SELECTION_SINGLE);

      add_columns (GTK_TREE_VIEW (treeview));

      gtk_container_add (GTK_CONTAINER (sw), treeview);

      /* expand all rows after the treeview widget has been realized */
      g_signal_connect (treeview, "realize",
                        G_CALLBACK (gtk_tree_view_expand_all), NULL);

    g_signal_connect(GTK_TREE_VIEW(treeview), "row-activated", (GCallback)  menu_row_activated, epub_widget);
      gtk_window_set_default_size (GTK_WINDOW (window), 650, 400);
    }

  if (!gtk_widget_get_visible (window))
    gtk_widget_show_all (window);
  else
    gtk_widget_destroy (window);

  return window;
}

/*

GtkWidget * book_get_tree_widget (GtkWidget *epub_widget)
{


  	GepubDoc *doc = gepub_widget_get_doc(epub_widget);
		gint chapternum = gepub_doc_get_chapter(doc);
		gint nchapternum = gepub_doc_get_n_chapters(doc);
  	gchar *title = gepub_doc_get_metadata(doc, GEPUB_META_TITLE);
	gchar *author=gepub_doc_get_metadata(doc, GEPUB_META_AUTHOR);


      GtkWidget *treeview;
      GtkTreeModel *model;

      model = create_epub_nav_model (doc);

      treeview = gtk_tree_view_new_with_model (model);
      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (treeview), FALSE);
      g_object_unref (model);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                                   GTK_SELECTION_SINGLE);

      add_columns (GTK_TREE_VIEW (treeview));


      g_signal_connect (treeview, "realize",
                        G_CALLBACK (gtk_tree_view_expand_all), NULL);

    g_signal_connect(GTK_TREE_VIEW(treeview), "row-activated", (GCallback)  menu_row_activated, epub_widget);


  return treeview;
}
*/

GtkWidget * book_get_tree_widget (BookWidget *epub_widget)
{


  	BookDoc *doc = book_widget_get_doc(epub_widget);

     // GtkWidget *vbox;
    //  GtkWidget *sw;
      GtkWidget *treeview;
      GtkTreeModel *model;

      /* create window, etc */


     // vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
     // gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
     // gtk_container_add (GTK_CONTAINER (window), vbox);

     // gtk_box_pack_start (GTK_BOX (vbox),    gtk_label_new (title),
      //                    FALSE, FALSE, 0);

     // sw = gtk_scrolled_window_new (NULL, NULL);
    //  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
    //                                       GTK_SHADOW_ETCHED_IN);
     // gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
     //                                 GTK_POLICY_AUTOMATIC,
    //                                  GTK_POLICY_AUTOMATIC);
     // gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);

      /* create model */
      model = create_book_nav_model (doc);

      /* create tree view */
      treeview = gtk_tree_view_new_with_model (model);
      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (treeview), FALSE);
      g_object_unref (model);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                                   GTK_SELECTION_SINGLE);

      add_columns (GTK_TREE_VIEW (treeview));

     // gtk_container_add (GTK_CONTAINER (sw), treeview);

      /* expand all rows after the treeview widget has been realized */
      g_signal_connect (treeview, "realize",
                        G_CALLBACK (gtk_tree_view_expand_all), NULL);

    g_signal_connect(GTK_TREE_VIEW(treeview), "row-activated", (GCallback)  menu_row_activated, epub_widget);
    //gtk_tree_view_set_enable_tree_lines(treeview,TRUE);

  return treeview;
}
