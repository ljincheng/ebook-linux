
#ifndef __BOOK_PDF_H__
#define __BOOK_PDF_H__

#include <glib-object.h>
#include <glib.h>

#include <gtk/gtk.h>
#include <poppler.h>

G_BEGIN_DECLS

typedef enum
{
  BOOK_PDF_ERROR_INVALID=0,
  BOOK_PDF_ERROR_ENCRYPTED,
  BOOK_PDF_ERROR_OPEN_FILE,
  BOOK_PDF_ERROR_BAD_CATALOG,
  BOOK_PDF_ERROR_DAMAGED
} BOOKPDFError;



#define BookPdfSelectionStyle PopplerSelectionStyle
#define BookPdfRectangle PopplerRectangle
#define BookPdfColor PopplerColor

#define BOOK_TYPE_PDF          (book_pdf_get_type ())
G_DECLARE_FINAL_TYPE (BookPdf, book_pdf, BOOK, PDF, GObject)

BookPdf *book_pdf_new(const gchar *path,const gchar *password,GError **error);

GtkWidget *book_pdf_create_widget (BookPdf *bookPdf,gint chapterIndex);

GdkPixbuf * book_pdf_get_pixbuf(BookPdf *bookPdf);

cairo_surface_t * book_pdf_get_surface(BookPdf *bookPdf);

cairo_surface_t * book_pdf_get_chapter_surface(BookPdf *bookPdf,gint chapterIndex);
cairo_region_t       *book_pdf_get_selected_region(BookPdf *bookPdf,gint chapterIndex, gdouble scale,BookPdfSelectionStyle style,BookPdfRectangle *selection);
gchar   *book_pdf_get_selected_text(BookPdf *bookPdf,gint chapterIndex, BookPdfSelectionStyle style,BookPdfRectangle *selection);
cairo_surface_t * book_pdf_render_selection(BookPdf *bookPdf,gint chapterIndex,gint width,gint height,
							  BookPdfRectangle   *selection,
							  BookPdfRectangle   *old_selection,
							  BookPdfSelectionStyle style,
							  BookPdfColor       *glyph_color,
							  BookPdfColor       *background_color);
/**
 * 获取页数
 */
gint book_pdf_get_chapter_num (BookPdf *bookPdf);

/**
 * 获取当前的第几页
 */
gint book_pdf_get_chapterIndex(BookPdf *bookPdf);

/**
 * 获取字符内容
 */
gchar* book_pdf_get_chapter_text(BookPdf *bookPdf,gint chapterIndex);

void book_pdf_chapter_get_size(BookPdf *bookPdf,gint index, double *width,double *height);

cairo_surface_t * book_pdf_chapter_get_surface(BookPdf *bookPdf,gint pageIndex,gint width,gint height);

gint book_pdf_get_width(BookPdf *bookPdf);
gint book_pdf_get_height(BookPdf *bookPdf);

gchar *book_pdf_get_metadata(BookPdf *bookPdf, const gchar *mdata);

gdouble book_pdf_get_scale(BookPdf *bookPdf);
void book_pdf_set_scale(BookPdf *bookPdf,gdouble scale);


/**
 *下一页
 */
void book_pdf_chapter_next(BookPdf *bookPdf);
/**
 * 上一页
 */
void book_pdf_chapter_prev(BookPdf *bookPdf);


void book_pdf_window_test(void);



/**
 * BOOK_PDF_META_TITLE:
 * The book title.
 */
#define BOOK_PDF_META_TITLE "title"

/**
 * BOOK_PDF_META_LANG:
 * The book author.
 */
#define BOOK_PDF_META_AUTHOR "author"

/**
 * BOOK_PDF_META_FORMAT:
 * The book format.
 */
#define BOOK_PDF_META_FORMAT "format"

/**
 * BOOK_PDF_META_CREATOR:
 * The book creator.
 */
#define BOOK_PDF_META_CREATOR "creator"

/**
 * BOOK_PDF_META_CREATEDATE:
 * The book creation-date.
 */
#define BOOK_PDF_META_CREATEDATE "creation-date"

G_END_DECLS
#endif /* __BOOK_PDF_H__ */
