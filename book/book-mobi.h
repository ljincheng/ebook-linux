
#ifndef __BOOK_MOBI_H__
#define __BOOK_MOBI_H__

#include <glib-object.h>
#include <glib.h>
#include  "mobi.h"
#include "book-doc.h"



G_BEGIN_DECLS

#define BOOK_TYPE_MOBI           (book_mobi_get_type ())
G_DECLARE_FINAL_TYPE (BookMobi, book_mobi, BOOK, MOBI, GObject)


BookMobi* book_mobi_new(const gchar *path);


/**
 * 获取基本信息数据
 */
gchar* book_mobi_get_metadata (	BookMobi *bookmobi, const gchar *mdata);

/**
 * 根据资源路径获取资源数据
 */
BookResource* book_mobi_get_resource (	BookMobi *bookmobi, const gchar *path);

/**
 * 获取章节
 */
BookResource* book_mobi_get_chapter  (	BookMobi *bookmobi, gint index);

/**
 * 获取章节总数量
 */
gint              book_mobi_get_chapter_num(	BookMobi *bookmobi);

/**
 * 导航菜单
 */
BookNavPoint* book_mobi_get_nav(	BookMobi *bookmobi);

//void    book_mobi_free(MOBIData *m);
//void    book_mobi_rawml_free(MOBIRawml *rawml );


G_END_DECLS
#endif /* __BOOK_EPUB_H__ */
