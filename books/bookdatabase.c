
#include "bookdatabase.h"
#include <libgepub/gepub.h>
#include "ephy-sqlite-connection.h"
#include "ephy-sqlite-statement.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <libmobi/mobi.h>
#include "book-widget.h"

#define BOOKDATABASEFILEPATH "/workspace/ebook/mybook/"
#define BOOKDATABASENAME "bebebook.db"

static EphySQLiteConnection *
create_database_connection ()
{
GError *error=NULL;
 EphySQLiteStatement *statement;
  EphySQLiteConnection *connection = ephy_sqlite_connection_new ();
  //GError *error = NULL;

 // if (g_file_test (filename, G_FILE_TEST_IS_REGULAR))
   // g_unlink (filename);
 gchar *filename = g_build_filename (BOOKDATABASEFILEPATH, BOOKDATABASENAME, NULL);
  //g_printf("data path:%s\n",filename);
  g_assert (ephy_sqlite_connection_open (connection, filename, &error));
  g_assert (!error);
if(!ephy_sqlite_connection_table_exists(connection,"bebe_books"))
    {
  statement = ephy_sqlite_connection_create_statement (connection, "CREATE TABLE bebe_books (id varchar(50),title varchar(200), src LONGVARCHAR,author VARCHAR(100),sort varchar(100),chapter INTEGER,ext varchar(10))", &error);
  ephy_sqlite_statement_step (statement, &error);
  g_object_unref (statement);
}
if(!ephy_sqlite_connection_table_exists(connection,"bebe_books_menu"))
    {
  statement = ephy_sqlite_connection_create_statement (connection, "CREATE TABLE bebe_books_menu (id varchar(50),title varchar(200))", &error);
  ephy_sqlite_statement_step (statement, &error);
  g_object_unref (statement);
}

  return connection;
}

static void
add_row (GtkTreeView *treeview,gchar *bookid,gchar * title,gchar * src,gchar *author,gchar *ext,gchar *sort)
{
  GtkTreeModel *model;
  GtkTreeIter *iter;
  model = gtk_tree_view_get_model (treeview);
  gtk_tree_store_append (model, &iter, NULL);
  gtk_tree_store_set (model, &iter,
                          BOOK_NAME_COLUMN, title,
                          ID, bookid,
                          SRC, src,
                        AUTHOR, author,
                      SORT,sort,
                      EXT,ext,
                          -1);


}

gboolean book_dir_import(const gchar *epubfile,const gchar *menutitle,GtkWidget *treeview)
{
  BookDoc *doc;
  gchar **arr;
  gchar *ext=NULL;
	  doc = book_doc_init(epubfile);
	  if (doc==NULL) {
		  perror("添加文件无效");
      return FALSE;
      }
  gchar *book_title = doc->bookName;
  gchar *book_author=doc->author;
  gchar *book_id=doc->id;


	gint i;
	arr = g_strsplit(epubfile, ".", -1);
	for (i = 0; arr[i] != NULL; i++) {
		if (i ==1) {
			ext = arr[1];
			break;
		}
	}
	g_free(arr);
  //database
    GError *error = NULL;
  EphySQLiteStatement *statement;
  EphySQLiteConnection *connection;

   
  connection = create_database_connection ();

 
 statement = ephy_sqlite_connection_create_statement (connection, "SELECT title FROM bebe_books_menu where id=?", &error);
g_assert (ephy_sqlite_statement_bind_string (statement, 0, menutitle, &error));
  g_assert (!error);

if( !ephy_sqlite_statement_step (statement, &error))
    {
g_object_unref (statement);
 statement = ephy_sqlite_connection_create_statement (connection, "INSERT INTO bebe_books_menu (id, title) VALUES (?, ?)", &error);
  g_assert (statement);
  g_assert (!error);
  g_assert (ephy_sqlite_statement_bind_string (statement, 0, menutitle, &error));
  g_assert (!error);
  g_assert (ephy_sqlite_statement_bind_string (statement, 1, menutitle, &error));
  g_assert (!error);
 g_assert (!ephy_sqlite_statement_step (statement, &error));
}
g_object_unref (statement);


   statement = ephy_sqlite_connection_create_statement (connection, "SELECT chapter FROM bebe_books where id=?", &error);
g_assert (ephy_sqlite_statement_bind_string (statement, 0, book_id, &error));
  g_assert (!error);


if( !ephy_sqlite_statement_step (statement, &error))
    {
g_object_unref (statement);
       statement = ephy_sqlite_connection_create_statement (connection, "INSERT INTO bebe_books (id, title,src,author,ext,sort) VALUES (?, ?,?,?,?,? )", &error);
  g_assert (statement);
  g_assert (!error);
//g_printf("保存文件:id=%s,titile=%s,src=%s,author=%s\n",book_id,book_title,epubfile,book_author);
  g_assert (ephy_sqlite_statement_bind_string (statement, 0, book_id, &error));
  g_assert (!error);
  g_assert (ephy_sqlite_statement_bind_string (statement, 1, book_title, &error));
  g_assert (!error);
    g_assert (ephy_sqlite_statement_bind_string (statement, 2, epubfile, &error));
  g_assert (!error);
    g_assert (ephy_sqlite_statement_bind_string (statement, 3, book_author, &error));
  g_assert(!error);
  g_assert(ephy_sqlite_statement_bind_string (statement,4,ext,&error));
  g_assert (!error);
 g_assert(ephy_sqlite_statement_bind_string (statement,5,menutitle,&error));
  g_assert (!error);
 g_assert (!ephy_sqlite_statement_step (statement, &error));
  if (error)
{
printf ("保存数据库失败: %s\n", error->message);
}
  g_object_unref (statement);
printf("保存成功\n");
      add_row(treeview,book_id,book_title,epubfile,book_author,ext,menutitle);
  
    }else{
printf("找到bookid=%s,不保存\n",book_id);
g_object_unref (statement);
}

  /* Will return false since there are no resulting rows. */



  ephy_sqlite_connection_close (connection);
  g_printf("保存文件完成:%s\n",epubfile);
  book_doc_free(doc);
return TRUE;
}

gboolean book_database_add_file (const gchar *epubfile,GtkWidget *treeview)
{
  BookDoc *doc;
  gchar **arr;
  gchar *ext=NULL;
	  doc = book_doc_init(epubfile);
	  if (doc==NULL) {
		  perror("添加文件无效");
      return FALSE;
      }
  gchar *book_title = doc->bookName;
  gchar *book_author=doc->author;
  gchar *book_id=doc->id;


	gint i;
	arr = g_strsplit(epubfile, ".", -1);
	for (i = 0; arr[i] != NULL; i++) {
		if (i ==1) {
			ext = arr[1];
			break;
		}
	}
	g_free(arr);
  //database
    GError *error = NULL;
  EphySQLiteStatement *statement;
  EphySQLiteConnection *connection;

  // gchar *temporary_file = g_build_filename (BOOKDATABASEFILEPATH, BOOKDATABASENAME, NULL);
  //g_printf("sqlitedata file path:%s\n",temporary_file);
  connection = create_database_connection ();
 

   statement = ephy_sqlite_connection_create_statement (connection, "SELECT chapter FROM bebe_books where id=?", &error);
g_assert (ephy_sqlite_statement_bind_string (statement, 0, book_id, &error));
  g_assert (!error);


if( !ephy_sqlite_statement_step (statement, &error))
    {
       statement = ephy_sqlite_connection_create_statement (connection, "INSERT INTO bebe_books (id, title,src,author,ext) VALUES (?, ?,?,?,?)", &error);
  g_assert (statement);
  g_assert (!error);
//g_printf("保存文件:id=%s,titile=%s,src=%s,author=%s\n",book_id,book_title,epubfile,book_author);
  g_assert (ephy_sqlite_statement_bind_string (statement, 0, book_id, &error));
  g_assert (!error);
  g_assert (ephy_sqlite_statement_bind_string (statement, 1, book_title, &error));
  g_assert (!error);
    g_assert (ephy_sqlite_statement_bind_string (statement, 2, epubfile, &error));
  g_assert (!error);
    g_assert (ephy_sqlite_statement_bind_string (statement, 3, book_author, &error));
  g_assert(!error);
  g_assert(ephy_sqlite_statement_bind_string (statement,4,ext,&error));
  g_assert (!error);

      add_row(treeview,book_id,book_title,epubfile,book_author,ext,NULL);
    }


  /* Will return false since there are no resulting rows. */
  g_assert (!ephy_sqlite_statement_step (statement, &error));
  g_assert (!error);
  g_object_unref (statement);


  ephy_sqlite_connection_close (connection);
  g_printf("保存文件完成:%s\n",epubfile);
  book_doc_free(doc);
return TRUE;
}

gboolean book_menu_list_menuid(const gchar *menuid,GtkWidget *treeview)
{
  GtkTreeIter iter;
   GError *error = NULL;
  EphySQLiteStatement *statement;
  EphySQLiteConnection *connection;

 
/*
  gchar *temporary_file = g_build_filename (BOOKDATABASEFILEPATH, BOOKDATABASENAME, NULL);
  g_printf("sqlitedata file path:%s\n",temporary_file);
  connection = ephy_sqlite_connection_new ();

 
  ephy_sqlite_connection_open (connection, temporary_file, &error);
*/
connection=create_database_connection ();
 

  statement = ephy_sqlite_connection_create_statement (connection, "SELECT id,title,src,author,sort,ext FROM bebe_books where sort=?", &error);
g_assert (ephy_sqlite_statement_bind_string (statement, 0, menuid, &error));
  g_assert (!error);

while( ephy_sqlite_statement_step (statement, &error))
    {
 // g_printf("查询结果：text=%s\n",ephy_sqlite_statement_get_column_as_string (statement, 1));
        
   add_row(treeview,ephy_sqlite_statement_get_column_as_string (statement, 0),
ephy_sqlite_statement_get_column_as_string (statement, 1),
ephy_sqlite_statement_get_column_as_string (statement, 2),
ephy_sqlite_statement_get_column_as_string (statement, 3),
ephy_sqlite_statement_get_column_as_string (statement,5),
ephy_sqlite_statement_get_column_as_string (statement,4));
    }
ephy_sqlite_statement_step (statement, &error);
  g_object_unref (statement);
  ephy_sqlite_connection_close (connection);


}

gboolean book_menu_list_query_title(const gchar *title,GtkWidget *treeview)
{
   GtkTreeIter iter;
   GError *error = NULL;
  EphySQLiteStatement *statement;
  EphySQLiteConnection *connection;

  if(title!=NULL && strlen(title)>0)
    {
connection=create_database_connection ();


gchar *likeStr=g_strdup_printf("%%%s%%",title);
  statement = ephy_sqlite_connection_create_statement (connection, "SELECT id,title,src,author,sort,ext FROM bebe_books where title like ?", &error);
g_assert (ephy_sqlite_statement_bind_string (statement, 0, likeStr, &error));
  g_free(likeStr);
  g_assert (!error);

while( ephy_sqlite_statement_step (statement, &error))
    {
 // g_printf("查询结果：text=%s\n",ephy_sqlite_statement_get_column_as_string (statement, 1));

   add_row(treeview,ephy_sqlite_statement_get_column_as_string (statement, 0),
ephy_sqlite_statement_get_column_as_string (statement, 1),
ephy_sqlite_statement_get_column_as_string (statement, 2),
ephy_sqlite_statement_get_column_as_string (statement, 3),
ephy_sqlite_statement_get_column_as_string (statement,5),
ephy_sqlite_statement_get_column_as_string (statement,4));
    }
ephy_sqlite_statement_step (statement, &error);
  g_object_unref (statement);
  ephy_sqlite_connection_close (connection);
}


}

GtkTreeModel *create_book_menu_all_model (void)
{
GtkTreeStore *model;
  GtkTreeIter iter;
   GError *error = NULL;
  EphySQLiteStatement *statement;
  EphySQLiteConnection *connection;


  /* create tree store */
  model = gtk_tree_store_new (NUM_COLUMNS,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING);

  /* add data to the tree store */


/*
  gchar *temporary_file = g_build_filename (BOOKDATABASEFILEPATH, BOOKDATABASENAME, NULL);
  g_printf("sqlitedata file path:%s\n",temporary_file);
  connection = ephy_sqlite_connection_new ();
  ephy_sqlite_connection_open (connection, temporary_file, &error);
*/
connection=create_database_connection ();
 

  statement = ephy_sqlite_connection_create_statement (connection, "SELECT id,title,src,author,sort,ext FROM bebe_books", &error);


while( ephy_sqlite_statement_step (statement, &error))
    {
 // g_printf("查询结果：text=%s\n",ephy_sqlite_statement_get_column_as_string (statement, 1));

       gtk_tree_store_append (model, &iter, NULL);
  gtk_tree_store_set (model, &iter,
                          BOOK_NAME_COLUMN, ephy_sqlite_statement_get_column_as_string (statement, 1),
                          ID, ephy_sqlite_statement_get_column_as_string (statement, 0),
                          SRC, ephy_sqlite_statement_get_column_as_string (statement, 2),
                        AUTHOR, ephy_sqlite_statement_get_column_as_string (statement, 3),
                      SORT,ephy_sqlite_statement_get_column_as_string (statement, 4),
                      EXT,ephy_sqlite_statement_get_column_as_string (statement,5),

                          -1);
    }
ephy_sqlite_statement_step (statement, &error);
  g_object_unref (statement);
  ephy_sqlite_connection_close (connection);




  return GTK_TREE_MODEL (model);

}

GtkTreeModel *create_book_menu_model (void)
{
GtkTreeStore *model;
  GtkTreeIter iter;
   GError *error = NULL;
  EphySQLiteStatement *statement;
  EphySQLiteConnection *connection;


  /* create tree store */
  model = gtk_tree_store_new (MENU_COLUMNS,
                              G_TYPE_STRING,
                              G_TYPE_STRING);

  /* add data to the tree store */

/*

  gchar *temporary_file = g_build_filename (BOOKDATABASEFILEPATH, BOOKDATABASENAME, NULL);
  g_printf("sqlitedata file path:%s\n",temporary_file);
  connection = ephy_sqlite_connection_new ();
  ephy_sqlite_connection_open (connection, temporary_file, &error);
*/
connection=create_database_connection ();

  statement = ephy_sqlite_connection_create_statement (connection, "SELECT id,title FROM bebe_books_menu", &error);


while( ephy_sqlite_statement_step (statement, &error))
    {
  //g_printf("查询结果：text=%s\n",ephy_sqlite_statement_get_column_as_string (statement, 1));
gtk_tree_store_append (model, &iter, NULL);
gtk_tree_store_set (model, &iter,
                          MENUTITLE, ephy_sqlite_statement_get_column_as_string (statement, 1),
                          MENUID, ephy_sqlite_statement_get_column_as_string (statement, 0),

                          -1);
    }
ephy_sqlite_statement_step (statement, &error);
  g_object_unref (statement);
  ephy_sqlite_connection_close (connection);

  return GTK_TREE_MODEL (model);
}

gint book_get_history_chapter(const gchar *bookid){

  int resChapter=0;
  EphySQLiteStatement *statement;
  EphySQLiteConnection *connection;
GError *error = NULL;

 
connection=create_database_connection ();
 

  statement = ephy_sqlite_connection_create_statement (connection, "SELECT chapter FROM bebe_books where id=?", &error);
g_assert (ephy_sqlite_statement_bind_string (statement, 0, bookid, &error));
  g_assert (!error);


while( ephy_sqlite_statement_step (statement, &error))
    {
      resChapter=ephy_sqlite_statement_get_column_as_int (statement, 0);
  //g_printf("查询结果：chapter=%d \n",resChapter);

    }
ephy_sqlite_statement_step (statement, &error);
  g_object_unref (statement);
  ephy_sqlite_connection_close (connection);

  return resChapter;
}

gboolean book_set_history_chapter(gchar * bookid,gint chapter){

  EphySQLiteStatement *statement;
  EphySQLiteConnection *connection;
  GError *error = NULL;
 
connection=create_database_connection ();
 

   statement = ephy_sqlite_connection_create_statement (connection, "update bebe_books set chapter=? where id=?", &error);
  g_assert (statement);
  g_assert (!error);
  g_assert (ephy_sqlite_statement_bind_int (statement, 0, chapter, &error));
  g_assert (!error);
  g_assert (ephy_sqlite_statement_bind_string (statement, 1, bookid, &error));
  g_assert (!error);

  /* Will return false since there are no resulting rows. */
  g_assert (!ephy_sqlite_statement_step (statement, &error));
  g_assert (!error);
  g_object_unref (statement);


  ephy_sqlite_connection_close (connection);
return TRUE;
}
