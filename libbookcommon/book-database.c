#include "book-database.h"
#include <sqlite3.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <sys/stat.h>
#include <string.h>
#include <gio/gio.h>
#include "book-log.h"

static GQuark book_database_error_quark(void)
{
	static GQuark q = 0;

	if (q == 0)
		q = g_quark_from_string("book-database-quark");
	return q;
}

typedef enum {
	BOOK_DATABSE_ERROR_OPEN = 0, /*< nick=Invalid >*/
  BOOK_DATABSE_ERROR_STATEMENTSTEP,
  BOOK_DATABSE_ERROR_UNKWON,
} BookDatabaseError;

struct _BookDatabase {
	GObject parent;
	gchar *dbFile;
sqlite3 * db_sqlite3;
sqlite3_stmt *db_stmt;
};

G_DEFINE_TYPE (BookDatabase, book_database, G_TYPE_OBJECT)


static void book_database_init (BookDatabase *self)
{
self->dbFile=NULL;
  self->db_stmt=NULL;
  self->db_sqlite3=NULL;
}

static void book_database_finalize (GObject *object)
{
  book_log_debug("[book_database清理对象内存]\n");
   BookDatabase *self = BOOK_DATABASE(object);
  book_database_statement_finalize (self);
  book_database_close(self);
  g_free(self->dbFile);
  G_OBJECT_CLASS (book_database_parent_class)->finalize (object);
}

static void
book_database_class_init (BookDatabaseClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_database_finalize;
}



BookDatabase* book_database_new(const gchar* dbfile){
  BookDatabase *db;
  db=g_object_new(BOOK_TYPE_DATABASE,NULL);
  db->dbFile=g_strdup( dbfile);
  return db;
}


gboolean book_database_open(BookDatabase* database,GError **error)
{

	sqlite3 *db;
	int nResult = sqlite3_open(database->dbFile, &db);
	if (nResult != SQLITE_OK) {
  if (error != NULL) {
    book_log_debug ("打开数据库失败：%s",database->dbFile);
				g_set_error(error, book_database_error_quark(), BOOK_DATABSE_ERROR_OPEN,  "打开数据库文件失败");
			}
		return FALSE;
	}
  database->db_sqlite3=db;
	return TRUE;
}


void book_database_close(BookDatabase* db){

if(db->db_sqlite3!=NULL)
    {
      sqlite3_close(db->db_sqlite3);
      db->db_sqlite3=NULL;
    }
}

gboolean book_database_exit_table(BookDatabase* database,const gchar  *tableName){
gboolean result;
  sqlite3 *db;
	sqlite3_stmt *prepared_statement;
  db=database->db_sqlite3;
  if(db!=NULL)
    {

	      if (sqlite3_prepare_v2(db, "SELECT COUNT(type) FROM sqlite_master WHERE type='table' and name=?", -1, &prepared_statement, NULL) != SQLITE_OK) {
          book_log_debug("数据库表是否存在获取失败:%s\n", sqlite3_errmsg(db));
	      return FALSE;
	      }
      if (sqlite3_bind_text(prepared_statement,  1, tableName, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
		      book_log_debug("数据库表是否存在设置参数失败:%s\n", sqlite3_errmsg(db));
		      sqlite3_finalize(prepared_statement);
		      return FALSE;
	      }
	      int error_code = sqlite3_step(prepared_statement);
	      if (error_code != SQLITE_OK && error_code != SQLITE_ROW && error_code != SQLITE_DONE) {
		      book_log_debug("数据库表是否存在设置参数失败:%s\n", sqlite3_errmsg(db));
		      sqlite3_finalize(prepared_statement);
	      return FALSE;
	      }

	  int exitTbl = sqlite3_column_int(prepared_statement, 0);
	  sqlite3_finalize(prepared_statement);
	  return  exitTbl;
          }
return FALSE;
}


gboolean book_database_statement(BookDatabase* db,const gchar *sql)
{
  gboolean res;
  if(db->db_sqlite3==NULL)
    return FALSE;

  if (sqlite3_prepare_v2(db->db_sqlite3, sql, -1, &db->db_stmt, NULL) != SQLITE_OK) {
			  book_log_debug("数据库表是否存在获取失败:%s\n", sqlite3_errmsg(db->db_sqlite3));
     return FALSE;
		}
  return TRUE;
}

int book_database_statement_step(BookDatabase* db){
  int error_code = sqlite3_step(db->db_stmt);
  book_log_debug("执行脚本返回step=%d\n",error_code);
  return error_code;
}
void book_database_statement_finalize(BookDatabase* db)
{
  if(db->db_stmt!=NULL)
    {
      sqlite3_finalize(db->db_stmt);
      db->db_stmt=NULL;
    }
}

gboolean book_database_statement_bind_string(BookDatabase* db,int column, const char *value){

  if(value==NULL)
    {
      return book_database_statement_bind_null(db,column);
    }
		if (sqlite3_bind_text(db->db_stmt,  column+1, value, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
			book_log_debug("SQL设置参数变更失败:%s\n", sqlite3_errmsg(db->db_sqlite3));
      return FALSE;
		}
  return TRUE;
}

gboolean book_database_statement_bind_int(BookDatabase* db,int column ,gint value){

		if (sqlite3_bind_int(db->db_stmt,  column+1, value) != SQLITE_OK) {
			book_log_debug("SQL设置参数变更失败:%s\n", sqlite3_errmsg(db->db_sqlite3));
      return FALSE;
		}
  return TRUE;
}

gboolean book_database_statement_bind_null(BookDatabase* db,int column){
if (sqlite3_bind_null(db->db_stmt,  column+1) != SQLITE_OK) {
			book_log_debug("SQL设置参数变更失败:%s\n", sqlite3_errmsg(db->db_sqlite3));
      return FALSE;
		}
  return TRUE;
}

void book_database_get_error (BookDatabase* db, GError **error)
{
  if (error && db!=NULL && db->db_sqlite3!=NULL)
    *error = g_error_new_literal (book_database_error_quark (), BOOK_DATABSE_ERROR_UNKWON, sqlite3_errmsg (db->db_sqlite3));
}

gchar* book_database_column_string(BookDatabase* db,int column){
  return sqlite3_column_text(db->db_stmt, column);

}
