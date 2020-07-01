
#ifndef __BOOK_EPUB_H__
#define __BOOK_EPUB_H__

#include <glib-object.h>
#include <glib.h>
#include <libgepub/gepub.h>
#include "book-doc.h"



G_BEGIN_DECLS

#define BOOK_TYPE_EPUB           (book_epub_get_type ())
G_DECLARE_FINAL_TYPE (BookEpub, book_epub, BOOK, EPUB, GObject)

BookEpub *book_epub_new(const gchar *path);


//GepubDoc *book_epub_init(const gchar *path);

/**
 * 获取基本信息数据
 */
gchar            *book_epub_get_metadata                    (BookEpub *bookepub, const gchar *mdata);

/**
 * 根据资源路径获取资源数据
 */
BookResource           *book_epub_get_resource                    (BookEpub *bookepub, const gchar *path);

/**
 * 获取章节
 */
BookResource   *book_epub_get_chapter  (BookEpub *bookepub, gint index);

/**
 * 获取章节总数量
 */
gint              book_epub_get_chapter_num                  (BookEpub *bookepub);

gint book_epub_path_to_chapter_index(BookEpub *bookepub,const gchar *path);

/**
 * 导航菜单
 */
BookNavPoint * book_epub_get_nav(BookEpub *bookepub);


G_END_DECLS
#endif /* __BOOK_EPUB_H__ */
