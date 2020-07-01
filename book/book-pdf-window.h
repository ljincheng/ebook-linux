
#ifndef __BOOK_PDF_WINDOW_H__
#define __BOOK_PDF_WINDOW_H__

#include <gtk/gtk.h>



G_BEGIN_DECLS

#define BOOK_TYPE_PDF_WINDOW (book_pdf_window_get_type())
G_DECLARE_FINAL_TYPE (BookPdfWindow, book_pdf_window, BOOK, PDF_WINDOW, GtkApplicationWindow)

GtkWidget*                book_pdf_window_new                  (GtkApplication       *application);

GtkWidget * book_pdf_window_create(const gchar *bookFile,GtkWidget * do_widge,const gchar * initInfo);

G_END_DECLS

#endif
