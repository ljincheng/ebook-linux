#include "book-pdf.h"

#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include <glob.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "book-common.h"
#include "book-pdf-render.h"

struct _BookPdf {
	GObject parent;
  gchar *path;
 gchar *password;
  PopplerDocument *document;
  gint chapterNum;
  gint chapterIndex;
  gint width;
  gint height;
   gdouble scale;
  BookPdfRender *render;
};

G_DEFINE_TYPE (BookPdf, book_pdf, G_TYPE_OBJECT)

static cairo_surface_t * book_pdf_page_render (PopplerPage *page,
                 gint         width,
                 gint         height,gdouble scale)
{
	cairo_surface_t *surface;
	cairo_t *cr;

	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					      width, height);
	cr = cairo_create (surface);
gdouble xscale,yscale;
  xscale=1.0 * scale;
  yscale=1.0 * scale;
   book_log_debug("book_pdf_page_render: width=%d,height=%d,scale=%f,yscale=%f \n",width,height,scale,yscale);
cairo_scale (cr, xscale,yscale);
	poppler_page_render (page, cr);

	cairo_set_operator (cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_set_source_rgb (cr, 1., 1., 1.);
	cairo_paint (cr);

	cairo_destroy (cr);

	return surface;
}


static void book_pdf_init (BookPdf *self)
{
  self->path=NULL;
  self->password=NULL;
  self->document=NULL;
  self->chapterNum=0;
  self->chapterIndex=0;
  self->width=0;
  self->height=0;
  self->scale=1.0;
  self->render=NULL;
}

static void book_pdf_finalize (GObject *object)
{
    book_log_debug("[BookPdf]内存清理\n");
   BookPdf *self = BOOK_PDF(object);
if(self->path!=NULL)
   g_clear_pointer (&self->path, g_free);
  if(self->password!=NULL)
   g_clear_pointer (&self->password, g_free);
if(self->render!=NULL)
    {
       g_clear_object (&self->render);
    }

  G_OBJECT_CLASS (book_pdf_parent_class)->finalize (object);
}

static void
book_pdf_class_init (BookPdfClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_pdf_finalize;
}

gchar *
book_pdf_format_date (GTime utime)
{
#pragma  message("=========== ljc =====_X86  macro  activated!")

	time_t time = (time_t) utime;
	char s[256];
	//const char fmt_hack[] = "%c";
  const char fmt_hack[] = "%Y-%m-%d %I:%M:%S";
	size_t len;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-y2k"
#ifdef HAVE_LOCALTIME_R
	struct tm t;
	if (time == 0 || !localtime_r (&time, &t)) return NULL;
	len = strftime (s, sizeof (s), fmt_hack, &t);
#else
	struct tm *t;
	if (time == 0 || !(t = localtime (&time)) ) return NULL;
	len = strftime (s, sizeof (s), fmt_hack, t);
#endif
#pragma GCC diagnostic pop

	if (len == 0 || s[0] == '\0') return NULL;

	return g_locale_to_utf8 (s, -1, NULL, NULL, NULL);
}

BookPdf *book_pdf_new(const gchar *path,const gchar *password,GError **error){


  BookPdf *bookPdf;
 GError *pdferror = NULL;
  bookPdf=g_object_new(BOOK_TYPE_PDF,NULL);
   if(path!=NULL)
    {
  bookPdf->path=g_strdup_printf("file://%s", path);
  if(password!=NULL)
  bookPdf->password=g_strdup(password);
  bookPdf->document = poppler_document_new_from_file (bookPdf->path, bookPdf->password, &pdferror);

	  if (bookPdf->document == NULL) {
    if(pdferror != NULL)
        {
              if(  pdferror->code == POPPLER_ERROR_ENCRYPTED)
            {
               book_log_error("打开PDF文件失败:%s,code=%d \n",pdferror->message,pdferror->code);
            }
            g_propagate_error (error, pdferror);
        }

	  }else{
      bookPdf->chapterNum=poppler_document_get_n_pages(bookPdf->document);
      }
    }
  //g_error_free(error);
  return bookPdf;
}

GtkWidget *book_pdf_create_widget (BookPdf *bookPdf,gint chapterIndex){

return book_pdf_render_new (bookPdf,chapterIndex);
}

GdkPixbuf * book_pdf_get_pixbuf(BookPdf *bookPdf){

  cairo_surface_t *surface;
  GdkPixbuf *pixbuf;
  surface=book_pdf_get_surface (bookPdf);
   pixbuf= gdk_pixbuf_get_from_surface (surface,  0, 0, cairo_image_surface_get_width (surface),  cairo_image_surface_get_height (surface));
  book_log_debug("book_pdf_get_pixbuf:完成\n");
  return pixbuf;
}

cairo_surface_t * book_pdf_get_chapter_surface(BookPdf *bookPdf,gint chapterIndex){
   cairo_surface_t *surface;
   PopplerPage *page;
    double width, height;
    gint pageWidth, pageHeight;
  gdouble scale=bookPdf->scale;
   page = poppler_document_get_page (bookPdf->document,chapterIndex);
      poppler_page_get_size (page, &width, &height);
  pageWidth=ceil(width*scale);
  pageHeight=ceil(height*scale);
  book_log_debug("book_pdf_get_chapter_surface:index=%d,width=%d,height=%d,scale=%f\n",chapterIndex,pageWidth,pageHeight,scale);
  surface=book_pdf_page_render(page, pageWidth, pageHeight,scale);
  //bookPdf->width=(gint)width;
  //bookPdf->height=(gint)height;
  g_object_unref (page);
  return surface;
}

cairo_surface_t * book_pdf_get_surface(BookPdf *bookPdf){
   cairo_surface_t *surface;
   PopplerPage *page;
    gdouble width, height;
   page = poppler_document_get_page (bookPdf->document, bookPdf->chapterIndex);
      poppler_page_get_size (page, &width, &height);
   width=width*bookPdf->scale;
  height=height*bookPdf->scale;
  surface=book_pdf_page_render(page,ceil(width),ceil(height),bookPdf->scale);
  //bookPdf->width=(gint)width;
 // bookPdf->height=(gint)height;
  g_object_unref (page);
  return surface;
}

cairo_region_t       *book_pdf_get_selected_region(BookPdf *bookPdf,gint chapterIndex,gdouble scale,BookPdfSelectionStyle style,BookPdfRectangle *selection){

    PopplerPage *page;
   page = poppler_document_get_page (bookPdf->document,chapterIndex);
  return  poppler_page_get_selected_region (page, scale, style,selection);
}

gchar   *book_pdf_get_selected_text(BookPdf *bookPdf,gint chapterIndex, BookPdfSelectionStyle style,BookPdfRectangle *selection){
                                             PopplerPage *page;
   page = poppler_document_get_page (bookPdf->document, chapterIndex);
     return   poppler_page_get_selected_text (page,
					       style,
					       selection);
}

cairo_surface_t * book_pdf_render_selection(BookPdf *bookPdf,gint chapterIndex,gint width,gint height,
							  BookPdfRectangle   *selection,
							  BookPdfRectangle   *old_selection,
							  BookPdfSelectionStyle style,
							  BookPdfColor       *glyph_color,
							  BookPdfColor       *background_color){

cairo_surface_t *surface;
  	cairo_t *cr;
   PopplerPage *page;
    double pageWidth, pageHeight;
  double xscale,yscale;
  int surface_width,surface_height;

   page = poppler_document_get_page (bookPdf->document,chapterIndex);
      poppler_page_get_size (page, &pageWidth, &pageHeight);
  xscale=width * 100 / pageWidth;
  yscale=height * 100 / pageHeight;
xscale=ceil(xscale)/100;
  yscale=ceil(yscale)/100;
  surface_width=xscale*pageWidth;
  surface_height=yscale*pageHeight;

  	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,surface_width , surface_height);
	cr = cairo_create (surface);
 cairo_scale (cr, xscale,yscale);
//	poppler_page_render (page, cr);
poppler_page_render_selection (page, cr,
				      selection, old_selection,
				      style,
				       glyph_color,
				       background_color);
	cairo_destroy (cr);
  g_object_unref (page);
  return surface;
                }

gint book_pdf_get_chapter_num (BookPdf *bookPdf){
  return bookPdf->chapterNum;
}

gint book_pdf_get_chapterIndex(BookPdf *bookPdf){
  return bookPdf->chapterIndex;
}

gchar* book_pdf_get_chapter_text(BookPdf *bookPdf,gint chapterIndex){

   PopplerPage *page;
  gchar *text;
   page = poppler_document_get_page (bookPdf->document, chapterIndex);
  text=poppler_page_get_text(page);
  book_log_debug ("获取第%d页的内容：%s\n",chapterIndex,text);
  return text;
}

void book_pdf_chapter_get_size(BookPdf *bookPdf,gint index,double *width,double *height){
  double page_width, page_height;
   PopplerPage *page;
   page = poppler_document_get_page (bookPdf->document,index);
  poppler_page_get_size (page, &page_width, &page_height);
  *width=page_width;
  *height=page_height;
}

cairo_surface_t * book_pdf_chapter_get_surface(BookPdf *bookPdf,gint chapterIndex,gint width,gint height){
  cairo_surface_t *surface;
  	cairo_t *cr;
   PopplerPage *page;
    double pageWidth, pageHeight;
  double xscale,yscale;
  int surface_width,surface_height;

   page = poppler_document_get_page (bookPdf->document,chapterIndex);
      poppler_page_get_size (page, &pageWidth, &pageHeight);
  xscale=width * 100 / pageWidth;
  yscale=height * 100 / pageHeight;
xscale=ceil(xscale)/100;
  yscale=ceil(yscale)/100;
  surface_width=xscale*pageWidth;
  surface_height=yscale*pageHeight;

  	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,surface_width , surface_height);
	cr = cairo_create (surface);
 cairo_scale (cr, xscale,yscale);
	poppler_page_render (page, cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_set_source_rgb (cr, 1., 1., 1.);
	cairo_paint (cr);
	cairo_destroy (cr);
gint swidth=cairo_image_surface_get_width(surface);
  gint sheight=cairo_image_surface_get_height(surface);
 book_log_debug("生成surface:  width=%d,swidth=%d,height=%d,sheight=%d,xscale=%f,yscale=%f\n",surface_width,swidth,surface_height,sheight,xscale,yscale);
  g_object_unref (page);
  return surface;

}

gint book_pdf_get_width(BookPdf *bookPdf){

 PopplerPage *page;

      gdouble width, height;
       page = poppler_document_get_page (bookPdf->document, bookPdf->chapterIndex);
      poppler_page_get_size (page, &width, &height);
       //width=width*bookPdf->scale;
//  height=height*bookPdf->scale;
      bookPdf->width=(gint)width;
      bookPdf->height=(gint)height;
       g_object_unref (page);

  return bookPdf->width;

}
gint book_pdf_get_height(BookPdf *bookPdf)
{

 PopplerPage *page;

      gdouble width, height;
       page = poppler_document_get_page (bookPdf->document, bookPdf->chapterIndex);
      poppler_page_get_size (page, &width, &height);
      // width=width*bookPdf->scale;
 // height=height*bookPdf->scale;
      bookPdf->width=(gint)width;
      bookPdf->height=(gint)height;
       g_object_unref (page);
book_pdf_get_chapter_text (bookPdf, bookPdf->chapterIndex);
  return bookPdf->height;
}

void book_pdf_chapter_next(BookPdf *bookPdf){
  if(bookPdf->chapterIndex< (bookPdf->chapterNum -1))
    {
      bookPdf->chapterIndex+=1;
    }
}
/**
 * 上一页
 */
void book_pdf_chapter_prev(BookPdf *bookPdf){
    if(bookPdf->chapterIndex>0)
    {
      bookPdf->chapterIndex-=1;
    }
}

gchar *book_pdf_get_metadata(BookPdf *bookPdf, const gchar *mdata){
  gchar *mvalue;
if(bookPdf==NULL || bookPdf->document ==NULL)
    {
      return NULL;
    }
    g_object_get(G_OBJECT(bookPdf->document),mdata,&mvalue,NULL);
  book_log_debug("mdata=%s,mvalue=%s\n",mdata,mvalue);
if(mvalue==NULL)
  return NULL;

  return g_strdup(mvalue);
}

static GdkPixbuf *pixbuf;
static cairo_surface_t *surface = NULL;
static gint book_pdf_drawcb(GtkWidget *widget,
         cairo_t   *cr,
         gpointer   data)
{
  //gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
/*
  GdkRGBA rgba;

  if (gdk_rgba_parse (&rgba, "white"))
    {
      gdk_cairo_set_source_rgba (cr, &rgba);
      poppler_page_render (data, cr);
      cairo_paint (cr);
    }
*/
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

  return TRUE;
}

gdouble book_pdf_get_scale(BookPdf *bookPdf){
  return bookPdf->scale;
}
void book_pdf_set_scale(BookPdf *bookPdf,gdouble scale){
  bookPdf->scale=scale;


}


typedef struct _BookSaveToBufferData      BookSaveToBufferData;
struct _BookSaveToBufferData {
	gchar *buffer;
	gsize len, max;
};

static gboolean
attachment_save_to_buffer_callback (const gchar  *buf,
				    gsize         count,
				    gpointer      user_data,
				    GError      **error)
{
	BookSaveToBufferData *sdata =(BookSaveToBufferData *)user_data;
	gchar *new_buffer;
	gsize new_max;

	if (sdata->len + count > sdata->max) {
		new_max = MAX (sdata->max * 2, sdata->len + count);
		new_buffer = (gchar *)g_realloc (sdata->buffer, new_max);

		sdata->buffer = new_buffer;
		sdata->max = new_max;
	}

	memcpy (sdata->buffer + sdata->len, buf, count);
	sdata->len += count;

	return TRUE;
}

static gboolean
attachment_save_to_buffer (PopplerAttachment  *attachment,
			   gchar             **buffer,
			   gsize              *buffer_size,
			   GError            **error)
{
	static const gint initial_max = 1024;
	BookSaveToBufferData sdata;

	*buffer = NULL;
	*buffer_size = 0;

	sdata.buffer = (gchar *) g_malloc (initial_max);
	sdata.max = initial_max;
	sdata.len = 0;

	if (! poppler_attachment_save_to_callback (attachment,
						   attachment_save_to_buffer_callback,
						   &sdata,
						   error)) {
		g_free (sdata.buffer);
		return FALSE;
	}

	*buffer = sdata.buffer;
	*buffer_size = sdata.len;

	return TRUE;
}

static GList *
pdf_document_attachments_get_attachments (PopplerDocument   *document)
{
	GList *attachments;
	GList *list;
	GList *retval = NULL;

	attachments = poppler_document_get_attachments ( document);
book_log_info("attachments len=%d\n",g_list_length(attachments));
	for (list = attachments; list; list = list->next) {
		PopplerAttachment *attachment;
		gchar *data = NULL;
		gsize size;
		GError *error = NULL;

		attachment = (PopplerAttachment *) list->data;
book_log_info("name:%s,description=%s,mtime=%s,ctime=%s\n",attachment->name,
							   attachment->description,
							   attachment->mtime,
							   attachment->ctime);
		if (attachment_save_to_buffer (attachment, &data, &size, &error)) {
      book_log_info("name:%s,description=%s,mtime=%s,ctime=%s,size=%zu\n",attachment->name,
							   attachment->description,
							   attachment->mtime,
							   attachment->ctime,size);


		} else {
			if (error) {
				g_warning ("%s", error->message);
				g_error_free (error);

				g_free (data);
			}
		}

		g_object_unref (attachment);
	}

	return g_list_reverse (retval);
}


void book_pdf_window_test(void){
   GError *error;
  /* demo window */
 GtkWidget *window = NULL;
 cairo_t     *cr;
   GtkAllocation allocation;
/* Current frame */

/* Background image */
/* Widgets */
 GtkWidget *da;
    PopplerPage *page;
   PopplerDocument *document;
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_screen (GTK_WINDOW (window),
                             gtk_widget_get_screen (NULL));
      gtk_window_set_title (GTK_WINDOW (window), "Pixbufs");
      gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

      g_signal_connect (window, "destroy",
                        G_CALLBACK (gtk_widget_destroyed), &window);

      error = NULL;
da = gtk_drawing_area_new ();

//=============pdf=====================
 book_log_debug("=============================测试PDF==============================\n");

  //gchar *uri="file:///home/ljc/Downloads/彩呗-解决方案--.pdf";
  gchar *uri="file:///workspace/docs/jcpt/接口文档/xd/小贷还款计划通知接口-v1.2.pdf";
  document = poppler_document_new_from_file (uri, NULL, &error);
  book_log_debug("测试PDF：%s\n",uri);
      gtk_widget_set_size_request (window, 500, 500);
pdf_document_attachments_get_attachments(document);

if(document!=NULL)
    {
      int n_pages = poppler_document_get_n_pages(document);
      book_log_debug ("PDF文档共%d页\n",n_pages);

      page = poppler_document_get_page (document, 2);
       if (page) {
    double width, height;

    poppler_page_get_size (page, &width, &height);
    gtk_window_set_default_size (GTK_WINDOW (window), (gint)width, (gint)height);
         gtk_widget_set_size_request (da, (gint)width, (gint)height);
          gtk_container_add (GTK_CONTAINER (window), da);
          book_log_debug ("PDF文档:width=%f,height=%f\n",width,height);

 width=width*1.2;
  height=height*1.2;

         surface = book_pdf_page_render(page,(gint)width, (gint)height,1.0);
   // g_object_unref (page);
          g_signal_connect (da, "draw",  G_CALLBACK (book_pdf_drawcb), page);
  }




//      pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, width, height);
//  poppler_page_render_to_pixbuf (page, 0, 0, width, height, 1.0, 0, pixbuf);
    }


//===================================






gtk_widget_show_all (window);

}
 
