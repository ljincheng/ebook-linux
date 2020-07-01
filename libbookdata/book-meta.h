
#ifndef __BOOK_META_H__
#define __BOOK_META_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define BOOK_TYPE_META           (book_meta_get_type ())
G_DECLARE_FINAL_TYPE (BookMeta, book_meta, BOOK, META, GObject)

BookMeta* book_meta_new(void);

/**
 * 要素初始化
 */
BookMeta* book_meta_initWithMeta(const gchar  *metaId,const gchar     *title,const gchar    *author,const gchar    *src,const gchar    *openTime,const gchar   *createTime,const gchar *flag,const gchar *mark);

void book_meta_set_metaId(BookMeta* bookMeta,const gchar *metaId);
void book_meta_set_title(BookMeta* bookMeta,const gchar *title);
void book_meta_set_author(BookMeta* bookMeta,const gchar *author);
void book_meta_set_src(BookMeta* bookMeta,const gchar *src);
void book_meta_set_openTime(BookMeta* bookMeta,const gchar *openTime);
void book_meta_set_createTime(BookMeta* bookMeta,const gchar *createTime);
void book_meta_set_flag(BookMeta* bookMeta,const gchar *flag);
void book_meta_set_mark(BookMeta* bookMeta,const gchar *mark);

gchar* book_meta_toString(BookMeta *bookMeta);

G_END_DECLS

#endif
