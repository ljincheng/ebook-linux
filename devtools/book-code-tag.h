
#ifndef __BOOK_CODE_TAG_H__
#define __BOOK_CODE_TAG_H__

#include <gtk/gtk.h>



G_BEGIN_DECLS

#define BOOK_TYPE_CODE_TAG (book_code_tag_get_type())
G_DECLARE_FINAL_TYPE (BookCodeTag, book_code_tag, BOOK, CODE_TAG, GObject)

BookCodeTag*  book_code_tag_new(void);

G_END_DECLS

#endif
