
#ifndef __BOOK_DATABASE_H__
#define __BOOK_DATABASE_H__

#include <gtk/gtk.h>
#include <glib.h>

G_BEGIN_DECLS

#define BOOK_DATABASE_CODE_OK         0
#define BOOK_DATABASE_CODE_ROW         100
#define BOOK_DATABASE_CODE_DONE        101

#define BOOK_TYPE_DATABASE           (book_database_get_type ())
G_DECLARE_FINAL_TYPE (BookDatabase, book_database, BOOK, DATABASE, GObject)

/**
 * 数据库初始化
 */
BookDatabase* book_database_new(const gchar* dbfile);

/**
 * 打开数据库
 */
gboolean book_database_open(BookDatabase* db,GError **error);

/**
 * 关闭数据库
 */
void book_database_close(BookDatabase* db);

gboolean book_database_statement(BookDatabase* db,const gchar *sql);

int book_database_statement_step(BookDatabase* db);

void book_database_statement_finalize(BookDatabase* db);

gboolean book_database_statement_bind_string(BookDatabase* db,int column, const char *value);
gboolean book_database_statement_bind_null(BookDatabase* db,int column);
gboolean book_database_statement_bind_int(BookDatabase* db,int column ,gint value);

gchar* book_database_column_string(BookDatabase* db,int column);

void book_database_get_error (BookDatabase* db, GError **error);


/**
 * 判断表是否存在
 */
gboolean book_database_exit_table(BookDatabase* db,const gchar   *tableName);



G_END_DECLS
#endif
