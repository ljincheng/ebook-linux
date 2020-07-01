
#include "book-app-marktreeview.h"
#include <glib-object.h>
#include <json-glib/json-glib.h>

enum
{
  APPMARKTITLE = 0,
  APPMARKFLAG,
  APPMARKSRC,
  APPMARK_COLUMNS
};

//定义BookAppMarkTreeview
struct _BookAppMarkTreeview
{
  GtkTreeView       parent;

};

G_DEFINE_TYPE (BookAppMarkTreeview, book_app_mark_treeview, GTK_TYPE_TREE_VIEW)

//定义方法
void book_app_mark_treeview_loop_json_data(JsonNode *jsonNode,GtkTreeIter *parentIter,GtkTreeModel *model);
GtkTreeModel *book_app_mark_treeview_model (gchar *data);
void book_app_mark_treeview_columns (GtkTreeView *treeview,GtkTreeModel *model);

static void
book_app_mark_treeview_class_init (BookAppMarkTreeviewClass *klass)
{
 // GtkTreeViewClass *object_class = GTK_TREE_VIEW_CLASS (klass);
//	object_class->constructed = book_app_mark_treeview_constructed;
	//object_class->finalize = book_app_mark_treeview_finalize;
}

static void book_app_mark_treeview_init (BookAppMarkTreeview *self)
{

}


GtkWidget * book_app_mark_treeview_new(gchar *data){
  GtkWidget *treeview;
  GtkTreeModel *model;

  model = book_app_mark_treeview_model(data);
  treeview= g_object_new (BOOK_TYPE_APP_MARK_TREEVIEW, "model", model, NULL);

     gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (treeview), FALSE);
      g_object_unref (model);
      gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                                   GTK_SELECTION_SINGLE);

      book_app_mark_treeview_columns (GTK_TREE_VIEW (treeview),model);

  g_signal_connect (treeview, "realize",G_CALLBACK (gtk_tree_view_expand_all), NULL);
 // g_signal_connect(GTK_TREE_VIEW(treeview), "row-activated", (GCallback)  book_application_panedLeft_treeview_row_activated, booklistTreeview);


  return treeview;
}



GtkTreeModel *book_app_mark_treeview_model (gchar *data)
{
 GtkTreeStore *model;
  GtkTreeIter iter;
   GError *error = NULL;
  model = gtk_tree_store_new (APPMARK_COLUMNS,  G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);

gtk_tree_store_append (model, &iter, NULL);
gtk_tree_store_set (model, &iter,  APPMARKTITLE, "上次阅读",  APPMARKFLAG,"-2",APPMARKSRC,"view-dual-symbolic",-1);
gtk_tree_store_append (model, &iter, NULL);
gtk_tree_store_set (model, &iter,  APPMARKTITLE, "书库", APPMARKFLAG,"-1",APPMARKSRC,"view-dual-symbolic",-1);
gtk_tree_store_append (model, &iter, NULL);
 gtk_tree_store_set (model, &iter,  APPMARKTITLE, "PDF", APPMARKFLAG,"-4",APPMARKSRC,"view-dual-symbolic",-1);
  gtk_tree_store_append (model, &iter, NULL);
gtk_tree_store_set (model, &iter,  APPMARKTITLE, "未分类", APPMARKFLAG,"-3",APPMARKSRC,"view-dual-symbolic",-1);

gtk_tree_store_append (model, &iter, NULL);
gtk_tree_store_set (model, &iter,  APPMARKTITLE, "书库分类",  APPMARKFLAG,"0",APPMARKSRC,NULL,-1);

//gchar *data=book_database_query_param ("bookdirectory");
  g_printf("db:data=%s\n",data);
    //解析JSON
  JsonNode *root_node;
  	JsonParser *parser = json_parser_new();
	json_parser_load_from_data(parser, data, -1, NULL);
	root_node = json_parser_get_root (parser);
  if (json_node_get_node_type (root_node) == JSON_NODE_ARRAY) {
    JsonArray * rootArr;
			   rootArr= json_node_get_array(root_node);
    gint len= json_array_get_length (rootArr);
    int index=0;
      for(index=0;index<len;index++)
            {
         JsonNode * node=json_array_get_element(rootArr,index);
       book_app_mark_treeview_loop_json_data(node,&iter,model);
      }
	}

  return GTK_TREE_MODEL (model);
}

void book_app_mark_treeview_columns (GtkTreeView *treeview,GtkTreeModel *model)
{
  gint col_offset;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
 // GtkTreeModel *model = gtk_tree_view_get_model (treeview);


  /** 图标 **/
/*
   renderer = gtk_cell_renderer_pixbuf_new ();
  column = gtk_tree_view_column_new_with_attributes ("Symbolic icon",
                                                     renderer,
                                                     "icon-name",
                                                     MARKSRC,
                                                     NULL);
  gtk_tree_view_column_set_max_width (GTK_TREE_VIEW_COLUMN (column), 50);
  gtk_tree_view_column_set_sort_column_id (column, MARKSRC);
  gtk_tree_view_append_column (treeview, column);
*/

  /* column for holiday names */
  renderer = gtk_cell_renderer_text_new ();
 //g_object_set (renderer,  "editable", TRUE, NULL);
 g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (APPMARKTITLE));
  //g_signal_connect (renderer, "edited", G_CALLBACK (cell_edited), model);
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, "专栏",
                                                            renderer, "text",
                                                            APPMARKTITLE,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  //gtk_tree_view_column_set_resizable (column, TRUE);
  // gtk_tree_view_column_set_min_width (GTK_TREE_VIEW_COLUMN (column), 100);
 // gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),  GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);


}



void book_app_mark_treeview_loop_json_data(JsonNode *jsonNode,GtkTreeIter *parentIter,GtkTreeModel *model)
{
   GtkTreeIter myIter;
      JsonObject *nodeObj= json_node_get_object (jsonNode);
      gchar * nodeTitle= json_object_get_string_member (nodeObj, "text");
      gint64 * nodeFlag= json_object_get_null_member(nodeObj,"mark")?0:json_object_get_int_member (nodeObj, "mark");

      gchar * markFlag=g_strdup_printf("%d",nodeFlag);

      //treeview 添加项
    gtk_tree_store_append (model, &myIter, (parentIter==NULL)?NULL:parentIter);
      gtk_tree_store_set (model, &myIter,  APPMARKTITLE, g_strdup( nodeTitle),  APPMARKFLAG, markFlag,-1);

      if(json_object_has_member(nodeObj,"children"))
                {
       JsonNode * nodeData= json_object_get_member (nodeObj, "children");
                       JsonArray * rootArr;
			           rootArr= json_node_get_array(nodeData);
            gint len= json_array_get_length (rootArr);
            int index=0;
            for(index=0;index<len;index++)
                                    {
                JsonNode * node=json_array_get_element(rootArr,index);
                book_app_mark_treeview_loop_json_data(node,&myIter,model);
                                  }
               }

}


void book_app_mark_treeview_get_row_flag(GtkTreeModel *model, GtkTreeIter *iter,char *value){
 gtk_tree_model_get(model, iter, APPMARKFLAG, value,  -1);
}
