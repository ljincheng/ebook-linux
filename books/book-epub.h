
#ifndef __BOOK_EPUB_H__
#define __BOOK_EPUB_H__

#include <glib-object.h>
#include <glib.h>
#include "gepub-nav.h"
#include "book-doc.h"


BookDoc *book_epub_init(const gchar *path);

BookData *book_epub_get_chapter(BookDoc *bookDoc, gint chapterIndex);
gint    book_epub_get_n_chapters(BookDoc     *bookDoc);
BookData *book_epub_get_chapter_by_uri(BookDoc *bookDoc,const gchar * url);
BookData *book_epub_get_resource(BookDoc *bookDoc, const gchar * url);
gchar  *book_epub_get_current_path(BookDoc *bookDoc);
GepubNavPoint * book_epub_nav_list(BookDoc *bookDoc);

void    book_epub_free(BookDoc *bookDoc);

#endif /* __BOOK_EPUB_H__ */
