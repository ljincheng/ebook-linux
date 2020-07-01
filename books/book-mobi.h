
#ifndef __BOOK_MOBI_H__
#define __BOOK_MOBI_H__

#include <glib-object.h>
#include <glib.h>
#include "gepub-nav.h"
#include "book-doc.h"


BookDoc *book_mobi_init(const gchar *path);

gchar * book_mobi_get_current_path(BookDoc *bookDoc);

BookData *book_mobi_get_chapter(BookDoc *bookDoc, gint chapterIndex);
gint    book_mobi_get_n_chapters(BookDoc     *bookDoc);
BookData *book_mobi_get_chapter_by_uri(BookDoc *bookDoc,const gchar * url);
BookData *book_mobi_get_resource(BookDoc *bookDoc, const gchar * url);

GepubNavPoint * book_mobi_nav_list(BookDoc *bookDoc);

void    book_mobi_free(BookDoc *bookDoc);

#endif /* __BOOK_MOBI_H__ */
