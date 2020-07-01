
#ifndef __BOOK_DOC_H__
#define __BOOK_DOC_H__

#include <glib-object.h>
#include <glib.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BOOK_TYPE_DOC           (book_doc_get_type ())
G_DECLARE_FINAL_TYPE (BookDoc, book_doc, BOOK, DOC, GObject)


struct _BookResource {
    gchar *mime;
    gchar *uri;
  GBytes * data;
};

typedef struct _BookResource BookResource;

typedef struct _BookNavPoint  BookNavPoint;
struct _BookNavPoint
{
   gchar    *text;
   gchar       * id;
  gchar        *playOrder;
   gchar       *src;
  gboolean    hasNext;
  gboolean    hasChildren;
  BookNavPoint *children;
  BookNavPoint *next;
};



//GType             book_doc_get_type                        (void) G_GNUC_CONST;

void book_doc_resource_free(BookResource *res);

/**
 * 根据文件路径生成 BookDoc
 */
BookDoc* book_doc_new                             (const gchar *path, GError **error);
/**
 * 获取基本信息数据
 */
gchar*  book_doc_get_metadata                    (BookDoc *doc, const gchar *mdata);

/**
 * 根据资源路径获取资源数据
 */
BookResource* book_doc_get_resource                    (BookDoc *doc, const gchar *path);

/**
 * 获取章节
 */
BookResource* book_doc_get_chapter                         (BookDoc *doc, gint index);
/**
 * 获取章节总数量
 */
gint              book_doc_get_chapter_num                  (BookDoc *doc);

/**
 * 路径转为章节序数
 */
gint book_doc_path_to_chapter_index(BookDoc *doc,const gchar *path);

/**
 * 导航菜单
 */
BookNavPoint* book_doc_get_nav(BookDoc *doc);



/**
 * BOOK_META_TITLE:
 * The book title.
 */
#define BOOK_META_TITLE "title"

/**
 * BOOK_META_LANG:
 * The book lang.
 */
#define BOOK_META_LANG "language"

/**
 * BOOK_META_ID:
 * The book id.
 */
#define BOOK_META_ID "identifier"

/**
 * BOOK_META_AUTHOR:
 * The book author.
 */
#define BOOK_META_AUTHOR "creator"

/**
 * BOOK_META_DESC:
 * The book description.
 */
#define BOOK_META_DESC "description"

G_END_DECLS
#endif /* __BOOK_DOC_H__ */

