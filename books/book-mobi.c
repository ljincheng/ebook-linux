#include "book-mobi.h"
#include "mobi.h"
#include <string.h>
#include "index.h"
#include "parse_rawml.h"
#include <libgepub/gepub-utils.h>
#include "book-mng.h"

typedef struct _BookMobiDoc BookMobiDoc;
struct _BookMobiDoc {
	MOBIData *data;
	MOBIRawml *rawml;
};


gchar  *book_mobi_get_current_path(BookDoc *bookDoc)
{
		return "test.hmtl";
}

BookDoc *book_mobi_init(const gchar *path)
{
	BookMobiDoc *bookMobiDoc = NULL;
	MOBIData *m = mobi_init();
	gint filetype = 0;

	if (strstr(path, ".mobi") != NULL) {
		filetype = 2;
	}else if (strstr(path, ".azw3") != NULL) {
		filetype = 3;
	}else{
		return NULL;
	}
	if (m == NULL) {
		return NULL;
	}
 //mobi_parse_kf7(m);
 g_printf("加载文件：%s\n",path);
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		mobi_free(m);
		return NULL;
	}

	MOBI_RET mobi_ret = mobi_load_file(m, file);
	fclose(file);
	if (mobi_ret != MOBI_SUCCESS) {
    g_printf("加载文件失败：%s\n",path);
		mobi_free(m);
		return NULL;
	}

	//bool iskf8=mobi_is_kf8(m);
//printf("==============iskf8: %d==============\n",iskf8);
//print_meta(m);

	MOBIRawml *rawml = mobi_init_rawml(m);
	if (rawml == NULL) {
		mobi_free(m);
		return NULL;
	}

	mobi_ret = mobi_parse_rawml(rawml, m);
	if (mobi_ret != MOBI_SUCCESS) {
		mobi_free(m);
		mobi_free_rawml(rawml);
		return NULL;
	}

	BookDoc  *bookDoc = g_malloc(sizeof(BookDoc));
	bookDoc->path = path;
	bookDoc->bookName = mobi_meta_get_title(m);
	bookDoc->author = mobi_meta_get_author(m);
	if (m->mh) {
		bookDoc->id = g_strdup_printf("%u", *m->mh->uid);
	}else if (m->ph) {
		bookDoc->id = g_strdup_printf("%u", m->ph->uid);
	}
	bookDoc->type = filetype;


	int chapterNum = 0;
	if (rawml->markup != NULL) {
		MOBIPart *curr = rawml->markup;
		while (curr != NULL) {
			//  if (curr->size > 0) {
			chapterNum++;
			//     }
			curr = curr->next;
		}
	}
	bookDoc->chapterNum = chapterNum;
	bookDoc->chapterIndex = 0;

	bookMobiDoc = g_malloc(sizeof(BookMobiDoc));
	bookMobiDoc->data = m;
	bookMobiDoc->rawml = rawml;

	bookDoc->entry = bookMobiDoc;
	return bookDoc;
}

GBytes *
book_mobi_html_with_uris(BookDoc *bookDoc,GBytes * doc)
{

	GBytes *content, *replaced;
	gchar *path, *base;

	g_return_val_if_fail(bookDoc, NULL);

	path = book_mobi_get_current_path(bookDoc);

	base = g_path_get_dirname(path);

	replaced = book_data_replace_resources(doc, base);
	return replaced;
}

BookData *book_mobi_get_chapter(BookDoc *bookDoc, gint chapterIndex)
{
	BookMobiDoc *mobiDoc = NULL;
	MOBIRawml *rawml = NULL;
	GBytes * html = NULL;
	GBytes *tempContent;
  	BookData *bookData=NULL;
	gchar *mime=NULL;
	mobiDoc = bookDoc->entry;
	rawml = mobiDoc->rawml;
	bool iskf8=mobi_is_rawml_kf8(rawml);
 printf("mobi:iskf8=%d\n",iskf8);
	if(chapterIndex>=0 && chapterIndex < bookDoc->chapterNum)
	{
		MOBIPart *curr = mobi_get_part_by_uid(rawml, chapterIndex);
		if (curr != NULL && curr->size > 0) {
// printf("mobi:读取第%d章内容,有内容\n",chapterIndex);
			MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
			mime=g_strdup_printf("%s",file_meta.mime_type);
			tempContent= g_bytes_new_static(curr->data, curr->size);
			html=book_mobi_html_with_uris(bookDoc,tempContent);
//			html=book_data_set_headnode(tempContent);
//printf("mobi:%s \n",curr->data);
		}else{
			printf("mobi:读取不到第%d章内容\n",chapterIndex);
		}

	 	bookData=book_data_new(mime,NULL,chapterIndex,html);
	}
	return bookData;
}

gint    book_mobi_get_n_chapters(BookDoc     *bookDoc)
{

}


BookData *book_mobi_get_resource(BookDoc *bookDoc, const gchar * url)
{
	//printf("MOBI:准备获取地址：url=%s\n", url);
	BookData *bookData=NULL;
	BookMobiDoc *mobiDoc = NULL;
	MOBIRawml *rawml = NULL;
	GBytes * html = NULL;
	GBytes *tempContent=NULL;

	char *chapterUrl;
	char partname[200];
	mobiDoc = bookDoc->entry;
	rawml = mobiDoc->rawml;

	int findRes = 0;


	gchar **arr;
	gint i;

	arr = g_strsplit(url, ".", -1);
	for (i = 0; arr[i] != NULL; i++) {
		if (i == 0) {
			chapterUrl = arr[0];
			break;
		}
	}
	g_free(arr);

	if (strstr(chapterUrl, "resource") != NULL) {
		uint32_t part_id = atoi(chapterUrl + 8);
		MOBIPart *curr = mobi_get_resource_by_uid(rawml, part_id);
		if (curr != NULL && curr->size > 0) {
     			html = g_bytes_new_static(curr->data, curr->size);
       			MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
      			bookData=book_data_new (g_strdup_printf("%s",file_meta.mime_type),url,part_id,html);
		}else{
			printf("没有找到url=%s \n",url);		
		}
	}else if (strstr(chapterUrl, "flow") != NULL) {
		uint32_t part_id = atoi(chapterUrl + 4);
		MOBIPart *curr = mobi_get_flow_by_uid(rawml, part_id);
		if (curr != NULL && curr->size > 0) {
			html = g_bytes_new_static(curr->data, curr->size);
         		MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
      			bookData=book_data_new (g_strdup_printf("%s",file_meta.mime_type),url,part_id,html);
		}else{
			printf("没有找到url=%s \n",url);		
		}
	}else if (strstr(chapterUrl, "part") != NULL) {
		uint32_t part_id = atoi(chapterUrl + 4);
  //  printf("mobi:准备获取part，uri=%s,fid=%s,uid=%zu,rawml==NULL:%d \n", chapterUrl, chapterUrl + 4,part_id,rawml==NULL);
		MOBIPart *curr = mobi_get_part_by_uid(rawml, part_id);
		if (curr != NULL && curr->size>0) {
       //printf("mobi:准备获取part，uri=%s,fid=%s,uid=%zu,size=%zu,data=%d \n", chapterUrl
        //      , chapterUrl + 4,part_id,curr->size,&curr->data);
	    		tempContent = g_bytes_new_static(curr->data, curr->size);
			html=book_mobi_html_with_uris(bookDoc,tempContent);
			//html=book_data_set_headnode(tempContent);
	      		MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
			bookData=book_data_new (g_strdup_printf("%s",file_meta.mime_type),url,part_id,html);
			//printf("mobi:uri=%s 获取part成功\n",chapterUrl);
		}else{
			printf("没有找到url=%s \n",url);		
		}
	}else{
		bookData=NULL;	
	}
  	g_free(chapterUrl);

  	return bookData;
}

BookData *book_mobi_get_chapter_by_uri(BookDoc *bookDoc,const gchar * url)
{
	BookData *bookData=NULL;
	BookMobiDoc *mobiDoc = NULL;
	MOBIRawml *rawml = NULL;
	GBytes * html = NULL;
	GBytes *tempContent=NULL;

	char *chapterUrl;
	char partname[200];
	mobiDoc = bookDoc->entry;
	rawml = mobiDoc->rawml;

	int findRes = 0;


	gchar **arr;
	gint i;

	arr = g_strsplit(url, ".", -1);
	for (i = 0; arr[i] != NULL; i++) {
		if (i == 0) {
			chapterUrl = arr[0];
			break;
		}
	}
	g_free(arr);

	if (strstr(chapterUrl, "part") != NULL) {
		uint32_t part_id = atoi(chapterUrl + 4);
		MOBIPart *curr = mobi_get_part_by_uid(rawml, part_id);
		if (curr != NULL && curr->size>0) {
	    		tempContent = g_bytes_new_static(curr->data, curr->size);
			html=book_mobi_html_with_uris(bookDoc,tempContent);
	      		MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
			bookData=book_data_new (g_strdup_printf("%s",file_meta.mime_type),url,part_id,html);
			//printf("mobi:uri=%s 获取part成功\n",chapterUrl);
		}else{
			printf("没有找到url=%s \n",url);		
		}
	}else{
		bookData=NULL;	
	}
  	g_free(chapterUrl);

  	return bookData;
}


GepubNavPoint * book_nav_get_last(GepubNavPoint * nav)
{
	if (nav == NULL) {
		return nav;
	}else{
		if (nav->hasNext) {
			return book_nav_get_last(nav->next);
		}else{
			return nav;
		}
	}
}

void book_nav_add_child(GepubNavPoint * parentNav, GepubNavPoint * childNav)
{
	if (parentNav != NULL && parentNav != childNav) {
		if (parentNav->hasChildren) {
			GepubNavPoint * lastNav = book_nav_get_last(parentNav->children);
			lastNav->hasNext = TRUE;
			lastNav->next = childNav;
		}else{
			parentNav->hasChildren = TRUE;
			parentNav->children = childNav;
		}
	}
}

GepubNavPoint * book_mobi_nav_list(BookDoc *bookDoc)
{
	BookMobiDoc *mobiDoc = NULL;
	MOBIRawml *rawml = NULL;
	mobiDoc = bookDoc->entry;
	rawml = mobiDoc->rawml;
	//测试打印目录
	GepubNavPoint     *navPoint = g_malloc(sizeof(GepubNavPoint));
	navPoint->id = NULL;
	navPoint->playOrder = NULL;
	navPoint->src = NULL;
	navPoint->text = NULL;
	navPoint->hasNext = FALSE;
	navPoint->hasChildren = FALSE;
	size_t i = 0;
	MOBI_RET ret;
	const size_t count = rawml->ncx->entries_count;
	if (count < 1) {
		return navPoint;
	}
	GepubNavPoint *ncx = malloc(count * sizeof(GepubNavPoint));
	uint32_t currLevel = 0;
	while (i < count) {
		const MOBIIndexEntry *ncx_entry = &rawml->ncx->entries[i];
		const char *label = ncx_entry->label;
		const size_t id = strtoul(label, NULL, 16);
		uint32_t cncx_offset;
		ret = mobi_get_indxentry_tagvalue(&cncx_offset, ncx_entry, INDX_TAG_NCX_TEXT_CNCX);
		const MOBIPdbRecord *cncx_record = rawml->ncx->cncx_record;
		char *text = mobi_get_cncx_string_utf8(cncx_record, cncx_offset, rawml->ncx->encoding);
		char *target = malloc(MOBI_ATTRNAME_MAXSIZE + 1);
		if (mobi_is_rawml_kf8(rawml)) {
			uint32_t posfid;
			ret = mobi_get_indxentry_tagvalue(&posfid, ncx_entry, INDX_TAG_NCX_POSFID);

			uint32_t posoff;
			ret = mobi_get_indxentry_tagvalue(&posoff, ncx_entry, INDX_TAG_NCX_POSOFF);

			uint32_t filenumber;
			char targetid[MOBI_ATTRNAME_MAXSIZE + 1];
			ret = mobi_get_id_by_posoff(&filenumber, targetid, rawml, posfid, posoff);
			if (posoff) {
				snprintf(target, MOBI_ATTRNAME_MAXSIZE + 1, "part%05u.html#%s", filenumber, targetid);
			} else {
				snprintf(target, MOBI_ATTRNAME_MAXSIZE + 1, "part%05u.html", filenumber);
			}
		} else {
			uint32_t filepos;
			ret = mobi_get_indxentry_tagvalue(&filepos, ncx_entry, INDX_TAG_NCX_FILEPOS);

			snprintf(target, MOBI_ATTRNAME_MAXSIZE + 1, "part00000.html#%010u", filepos);
		}
		uint32_t level;
		ret = mobi_get_indxentry_tagvalue(&level, ncx_entry, INDX_TAG_NCX_LEVEL);
		uint32_t parent = MOBI_NOTSET;
		ret = mobi_get_indxentry_tagvalue(&parent, ncx_entry, INDX_TAG_NCX_PARENT);
		uint32_t first_child = MOBI_NOTSET;
		ret = mobi_get_indxentry_tagvalue(&first_child, ncx_entry, INDX_TAG_NCX_CHILD_START);
		uint32_t last_child = MOBI_NOTSET;
		ret = mobi_get_indxentry_tagvalue(&last_child, ncx_entry, INDX_TAG_NCX_CHILD_END);
		//printf("seq=%zu, id=%zu, text='%s', target='%s', level=%u, parent=%u, fchild=%u, lchild=%u\n", i, id, text, target, level, parent, first_child, last_child);
		char str_int[10 ];
		snprintf(str_int, 11, "%u", id);
		ncx[i] = (GepubNavPoint) {text, str_int, str_int, target, FALSE, FALSE, NULL, NULL };
		if (level == 0) {
			if (i > 0) {
				GepubNavPoint *lastNav = book_nav_get_last(&ncx[0]);
				lastNav->hasNext = TRUE;
				lastNav->next = &ncx[i];
			}
		}else if (level > 0) {
			book_nav_add_child(&ncx[parent], &ncx[i]);
		}
		i++;
	}
	navPoint->hasChildren = TRUE;
	navPoint->children = ncx;
	return navPoint;
}


void    book_mobi_free(BookDoc *bookDoc)
{
		BookMobiDoc *mobiDoc = NULL;
		mobiDoc = bookDoc->entry;
		mobi_free(mobiDoc->data);
		mobi_free_rawml(mobiDoc->rawml);
		//g_free(bookDoc->entry);

}
