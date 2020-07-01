#include "book-doc.h"
#include "mobi.h"
#include <string.h>
#include <libgepub/gepub.h>
#include "index.h"
#include "parse_rawml.h"
#include "book-epub.h"
#include "book-mobi.h"

BookData *book_data_new(const gchar *mime,const gchar *url,const gint spineIndex,GBytes * data)
{
  BookData  *bookData = g_malloc(sizeof(BookData));
  bookData->mime=mime;
  bookData->url=url;
  bookData->spineIndex=spineIndex;
  bookData->data=data;
  return bookData;
}

void book_data_free(BookData * bookData)
{

  //g_free(bookData->mime);
 // g_free(bookData->url);
 // g_bytes_unref(bookData->data);
  //bookData->mime=NULL;
  //bookData->url=NULL;
 // bookData->data=NULL;
 //g_free(bookData);

}

BookDoc *book_doc_init(const gchar *path)
{
	if (strstr(path, ".epub") != NULL) {
		return book_epub_init(path);
	}else if (strstr(path, ".mobi") != NULL) {
		return book_mobi_init(path);
	}else if (strstr(path, ".azw3") != NULL) {
		return book_mobi_init(path);
	}

	return NULL;
}


BookData *book_doc_get_n_chapter(BookDoc *bookDoc, gint chapterIndex)
{
//  printf("读取第%d章内容\n",chapterIndex);
	if (chapterIndex > bookDoc->chapterNum || chapterIndex < 0) {
		return NULL;
	}
	if (bookDoc->type == 2 || bookDoc->type == 3) {
		return book_mobi_get_chapter(bookDoc, chapterIndex);
	}else if (bookDoc->type == 1) {
		return book_epub_get_chapter(bookDoc, chapterIndex);
	}

	return NULL;
}




gchar  *book_doc_get_current_path(BookDoc *bookDoc)
{
	if (bookDoc->type == 1) {
		return book_epub_get_current_path (bookDoc);
	}else{
		return book_mobi_get_current_path(bookDoc);
	}
}



BookData *book_doc_get_resource(BookDoc *bookDoc, const gchar * url)
{
	if (bookDoc->type == 2 || bookDoc->type == 3) {
   // printf("mobi：页面寻找url=%s\n",url);
		return book_mobi_get_resource(bookDoc, url);
	}else if (bookDoc->type == 1) {
	//	printf("获取资源:url=%s\n ", url);
		return book_epub_get_resource(bookDoc, url);
	}
	return NULL;
}

BookData *book_doc_get_chapter_by_uri(BookDoc *bookDoc,const gchar * url)
{
	if (bookDoc->type == 2 || bookDoc->type == 3) {
		return book_mobi_get_chapter_by_uri(bookDoc, url);
	}else if (bookDoc->type == 1) {
		return book_epub_get_chapter_by_uri(bookDoc, url);
	}
	return NULL;
}

void    book_doc_free(BookDoc *bookDoc)
{
	if (bookDoc->type == 2 || bookDoc->type == 3) {
		book_mobi_free (bookDoc);
	}else if (bookDoc->type == 1) {
	  book_epub_free (bookDoc);
	}
	//g_free(bookDoc);
//	g_printf(" free bookDoc oK\n");
}


GepubNavPoint    *book_get_nav_list(BookDoc *bookDoc)
{
	if (bookDoc->type == 2 || bookDoc->type == 3) {
		GepubNavPoint  *ncx = book_mobi_nav_list(bookDoc);
		return ncx;
	}else if (bookDoc->type == 1) {
		return  book_epub_nav_list(bookDoc);
	}
}
