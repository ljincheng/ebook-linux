
#ifndef __BOOK_DATABASE_BOOKPARAM_H__
#define __BOOK_DATABASE_BOOKPARAM_H__

#include "book-database.h"

/**
 * 创建数据表book_info
 */
gint book_database_create_bookparam(BookDatabase* database,GError **error);

gchar* book_database_get_bookparam(BookDatabase* database,const gchar* paramId);

gint book_database_update_bookparam(BookDatabase* db,const gchar* paramId,const gchar* paramValue,GError **error);

gint book_database_insert_bookparam(BookDatabase* db,const gchar* paramId,const gchar* paramValue,GError **error);

gint book_database_delete_bookparam(BookDatabase* db,const gchar* paramId,GError **error);

#endif
