


#ifndef _BOOK_PDF_RENDER_H_
#define _BOOK_PDF_RENDER_H_


#include <gtk/gtk.h>
#include <poppler.h>
#include "book-pdf.h"
G_BEGIN_DECLS

#define BOOK_TYPE_PDF_RENDER          (book_pdf_render_get_type ())
G_DECLARE_FINAL_TYPE (BookPdfRender, book_pdf_render, BOOK, PDF_RENDER, GtkDrawingArea)

//GtkWidget *book_pdf_render_new(PopplerDocument *document,gint pageIndex);
GtkWidget *book_pdf_render_new(BookPdf *bookPdf,gint pageIndex);

void book_pdf_render_set_scale(BookPdfRender *render,gdouble scale);

G_END_DECLS

#endif /* _BOOK_PDF_RENDER_H_ */
