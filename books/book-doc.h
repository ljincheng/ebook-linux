#ifndef __BOOKS_DOC_H__
#define __BOOKS_DOC_H__

#include <glib-object.h>
#include <glib.h>
#include "gepub-nav.h"

typedef struct _BookDoc  BookDoc;
struct _BookDoc
{
   gchar    *path;
   gchar    *id;
   gchar    *bookName;
   gchar    *author;
   gint     type;
  gint      chapterNum;
  gint      chapterIndex;
  gpointer    entry;
};

typedef struct _BookData  BookData;
struct _BookData
{
  gchar * mime;
  gchar *url;
  gint spineIndex;
  GBytes * data;
};

BookData *book_data_new(const gchar *mime,const gchar *url,const gint spineIndex,GBytes * data);


BookDoc *book_doc_init(const gchar *path);

BookData *book_doc_get_n_chapter(BookDoc *bookDoc,gint chapterIndex);
BookData *book_doc_get_chapter_by_uri(BookDoc *bookDoc,const gchar * url);

BookData *book_doc_get_resource(BookDoc *bookDoc,const gchar * url);
gchar  *book_doc_get_current_path(BookDoc *bookDoc);
GepubNavPoint    *book_get_nav_list 				(BookDoc *bookDoc);

void    book_doc_free(BookDoc *bookDoc);

#endif /* __BOOKS_DOC_H__ */


