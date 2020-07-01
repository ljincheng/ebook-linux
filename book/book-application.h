#ifndef __BOOK_APPLICATION_H
#define __BOOK_APPLICATION_H

#include <gtk/gtk.h>


typedef struct _BookApplication          BookApplication;
#define BOOK_TYPE_APPLICATION (book_application_get_type())

G_DECLARE_FINAL_TYPE (BookApplication, book_application, BOOK, APPLICATION, GtkApplication)

 BookApplication*  book_application_new(void);



#endif /* __BOOKAPPLICATION_H */
