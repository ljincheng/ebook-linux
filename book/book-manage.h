
#ifndef __BOOK_MANAGE_H__
#define __BOOK_MANAGE_H__

#include "book-common.h"

G_BEGIN_DECLS

#define BOOK_TYPE_MANAGE           (book_manage_get_type ())
G_DECLARE_FINAL_TYPE (BookManage, book_manage, BOOK, MANAGE, GObject)


BookManage* book_manage_new(void);

void book_manage_check_database(void);

gchar * book_manage_query_param(const gchar *queryKey);

gboolean book_manage_updateMark_bookmeta(const gchar* bookId,const gchar* mark,GError **error);

gboolean book_manage_insert_bookmeta(const gchar *bookId,const  gchar *bookTitle,const  gchar * bookAuthor, const gchar *bookSrc,const gchar *bookFlag,GError **error);

/**
 * 获取书籍基本要素
 */
BookMeta* book_manage_get_bookmeta(const gchar * metaId);

GSList * book_manage_list_bookmeta(gint flag,const gchar* orderBy);

GSList * book_manage_all_bookmeta(const gchar* orderBy);

GSList * book_manage_lastRead_bookmeta();

GSList * book_manage_nullFlag_bookmeta();

GSList * book_manage_query_bookmeta(const gchar* titleKey);


G_END_DECLS


#endif
