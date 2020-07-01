#include "book-epub.h"
#include <string.h>
#include <libgepub/gepub.h>
#include "index.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "book-mng.h"

gchar  *book_epub_get_current_path(BookDoc *bookDoc)
{
	return gepub_doc_get_current_path(bookDoc->entry);
}

GBytes *
book_epub_html_with_uris(BookDoc *bookDoc,GBytes * doc)
{

	GBytes *content, *replaced;
	gchar *path, *base;

	g_return_val_if_fail(bookDoc, NULL);

	path = book_epub_get_current_path(bookDoc);

	base = g_path_get_dirname(path);

	replaced = book_data_replace_resources(doc, base);
	return replaced;
  
}

BookDoc *book_epub_init(const gchar *path)
{
	GepubDoc *doc;
	gchar *title, *author, *bookid;
	GList *chapter;
	BookDoc  *bookDoc = NULL;
	doc = gepub_doc_new(path, NULL);
	if (!doc) {
		return NULL;
	}

	title = gepub_doc_get_metadata(doc, GEPUB_META_TITLE);
	author = gepub_doc_get_metadata(doc, GEPUB_META_AUTHOR);
	bookid = gepub_doc_get_metadata(doc, GEPUB_META_ID);
	chapter = gepub_doc_get_chapter_all(doc);

	bookDoc = g_malloc(sizeof(BookDoc));
	bookDoc->path = path;
	bookDoc->bookName = title;
	bookDoc->author = author;
	bookDoc->id = bookid;
	bookDoc->type = 1;
	bookDoc->chapterNum = g_list_length(chapter);
	bookDoc->chapterIndex = 0;
	bookDoc->entry = doc;
	return bookDoc;
}

gint  book_epub_get_n_chapters(BookDoc     *bookDoc){
return gepub_doc_get_n_chapters (bookDoc->entry);
 }


BookData *book_epub_get_chapter(BookDoc *bookDoc, gint chapterIndex)
{
  printf("准备获取第%d个文件\n",chapterIndex);
 GepubDoc *doc;
 BookData *bookData=NULL;
 GBytes * html = NULL;
 GBytes *tempContent=NULL;
 gchar *path=NULL;
 gchar *mime=NULL;
 doc = bookDoc->entry;
 gepub_doc_set_chapter(doc, chapterIndex);
 //tempContent= gepub_doc_get_current_with_epub_uris(doc);
 tempContent=gepub_doc_get_current(doc);
 html=book_epub_html_with_uris(bookDoc,tempContent);
 //path= gepub_doc_get_current_path(doc);
 mime=gepub_doc_get_current_mime(doc);
 //html=book_data_set_headnode(tempContent);
 bookData=book_data_new (mime,path,chapterIndex,html);
	return bookData;
}


BookData *book_epub_get_resource(BookDoc *bookDoc, const gchar * url)
{
  BookData *bookData;
  GBytes * html = NULL;
  gchar *mime=NULL;
  gint chapterIndex=-1;
  GBytes *content=NULL;
  GBytes *tempContent=NULL;
  gchar *path, *base;
  GepubDoc *doc;

  doc=bookDoc->entry;
  mime=gepub_doc_get_resource_mime(doc,url);
  if(mime && !strcmp("application/xhtml+xml",mime))
    {
    // content= gepub_doc_get_resource(doc, url);
	 //   path = gepub_doc_get_current_path(doc);
	 //   base = g_path_get_dirname(path);
	 //  html=gepub_utils_replace_resources(content, base);
	chapterIndex=gepub_doc_get_chapter_by_path(doc,url);
	 gepub_doc_set_chapter(doc, chapterIndex);
	tempContent=gepub_doc_get_current(doc);
	 html=book_epub_html_with_uris(bookDoc,tempContent);
	// html=book_data_set_headnode(tempContent);
  }else{
        html= gepub_doc_get_resource(doc, url);
      }

	//g_free(path);
	//g_bytes_unref(content);

	bookData=book_data_new(mime,url,chapterIndex,html);
  return bookData;
}

BookData *book_epub_get_chapter_by_uri(BookDoc *bookDoc,const gchar * url)
{
 BookData *bookData;
  GBytes * html = NULL;
  gchar *mime=NULL;
  gint chapterIndex=-1;
  GBytes *content=NULL;
  GBytes *tempContent=NULL;
  gchar *path, *base;
  GepubDoc *doc;

  doc=bookDoc->entry;
  mime=gepub_doc_get_resource_mime(doc,url);
  if(mime  )
    {
	chapterIndex=gepub_doc_get_chapter_by_path(doc,url);
	 gepub_doc_set_chapter(doc, chapterIndex);
	tempContent=gepub_doc_get_current(doc);
	 html=book_epub_html_with_uris(bookDoc,tempContent);
  } 
  bookData=book_data_new(mime,url,chapterIndex,html);
  return bookData;
}


GepubNavPoint * book_epub_nav_list(BookDoc *bookDoc)
{
printf("准备加载菜单:epub\n");
  return gepub_nav_get_list(bookDoc->entry);
}

void    book_epub_free(BookDoc *bookDoc)
{
		g_free(bookDoc->entry);
}

