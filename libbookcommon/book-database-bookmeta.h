
#ifndef __BOOK_DATABASE_BOOKMETA_H__
#define __BOOK_DATABASE_BOOKMETA_H__

#include "book-meta.h"
#include "book-database.h"

/**
 * 创建数据表book_info
 */
gint book_database_create_bookmeta(BookDatabase* database,GError **error);

/**
 * 保存书籍基本要素
 */
gint book_database_insert_bookmeta(BookDatabase* database,BookMeta* bookMeta,GError **error);

/**
 * 获取书籍基本要素
 */
BookMeta* book_database_get_bookmeta(BookDatabase* db,const gchar * metaId);

/**
 *  获取书籍基本要素列表
 */
GSList * book_database_list_bookmeta(BookDatabase* db, gint flag,const gchar* orderBy);

/**
 * 根据标题或作者关键词查询
 */
GSList * book_database_query_bookmeta(BookDatabase* db,const gchar* titleKey);

/**
 *获取全部数据
 */
GSList * book_database_all_bookmeta(BookDatabase* db,const gchar* orderBy);

/**
 * 上次阅读的数据
 */
GSList * book_database_lastRead_bookmeta(BookDatabase* db);

/**
 * 未分类的数据
 */
GSList * book_database_nullFlag_bookmeta(BookDatabase* db);

gint book_database_update_bookmeta(BookDatabase* db,BookMeta* bookMeta,GError **error);

gint book_database_updateMark_bookmeta(BookDatabase* db,BookMeta* bookMeta,GError **error);

gint book_database_delete_bookmeta(BookDatabase* db,const gchar* metaId,GError **error);

#endif
