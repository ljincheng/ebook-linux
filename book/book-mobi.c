#include "book-mobi.h"
#include <string.h>
#include "index.h"


#ifndef MOBI_ATTRNAME_MAXSIZE
#define MOBI_ATTRNAME_MAXSIZE 150
#endif

#include "config.h"
#include "mobi.h"


struct _BookMobi {
	GObject parent;
  MOBIData *m;
  	MOBIRawml *rawml;
  gchar *path;
};

G_DEFINE_TYPE (BookMobi, book_mobi, G_TYPE_OBJECT)

static void book_mobi_init (BookMobi *self)
{
	MOBIData *m = mobi_init();
self->m=m;
}

static void book_mobi_finalize (GObject *object)
{
   g_printf("清理BookMobi对象内存\n");
   BookMobi *self = BOOK_MOBI(object);
if(self->path!=NULL)
   g_clear_pointer (&self->path, g_free);

  if(self->rawml!=NULL)
    {
       mobi_free_rawml(self->rawml);
    }
   if(self->m!=NULL){
  	mobi_free(self->m);
   }

  G_OBJECT_CLASS (book_mobi_parent_class)->finalize (object);
  g_printf("清理BookMobi对象内存完成\n");
}

static void
book_mobi_class_init (BookMobiClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_mobi_finalize;
}


BookMobi* book_mobi_new(const gchar *path){

  BookMobi *bookmobi;
  bookmobi=g_object_new(BOOK_TYPE_MOBI,NULL);
  bookmobi->path=g_strdup( path);

	FILE *file = fopen(bookmobi->path, "rb");
	if (file != NULL) {
 MOBI_RET mobi_ret = mobi_load_file(bookmobi->m, file);
	fclose(file);
	if (mobi_ret != MOBI_SUCCESS) {
    g_printf("加载文件失败,path=%s\n",path);
    g_clear_object(&bookmobi);
    bookmobi=NULL;
	}
 }
                                            g_printf("打开path=%s完成，bookmobi＝%d\n",path,bookmobi==NULL);
    return bookmobi;
}


MOBIRawml *book_mobi_get_rawml(	BookMobi *bookmobi)
{

   MOBIRawml *rawml0=bookmobi->rawml;
  if(rawml0==NULL)
    {

      MOBIData *m=bookmobi->m;
      if(m!=NULL)
        {
           MOBIRawml *rawml = mobi_init_rawml(m);
      MOBI_RET mobi_ret = mobi_parse_rawml(rawml, m);
	      if (mobi_ret != MOBI_SUCCESS) {
		       mobi_free_rawml(rawml);
          g_printf("获取到空rawml哦\n");
		       return NULL;
	      }else{
            bookmobi->rawml=rawml;
                 }
        }else{
           g_printf("获取到空MOBIData哦\n");
        }



    }

		return bookmobi->rawml;

}

/**
 * 获取基本信息数据
 */
gchar* book_mobi_get_metadata( 	BookMobi *bookmobi, const gchar *mdata)
{
    MOBIData *m=bookmobi->m;
	if (m == NULL) {
		return NULL;
	}
	if (strcmp(BOOK_META_TITLE, mdata) == 0) {
		return mobi_meta_get_title(m);
	}
	if (strcmp(BOOK_META_AUTHOR, mdata) == 0) {
		return mobi_meta_get_author(m);;
	}

	if (strcmp(BOOK_META_ID, mdata) == 0) {
  char * mybookId=mobi_meta_get_isbn(m);
            if(mybookId!=NULL)
            {
               return mybookId;
            }
      char *bookasin=mobi_meta_get_asin(m);
        if(bookasin!=NULL)
        {
            return bookasin;
        }

    if(m->mh->uid)
        {
            return g_strdup_printf("%u" ,*m->mh->uid);
        }
		return NULL;
	}
	if (strcmp(BOOK_META_DESC, mdata) == 0) {
		return mobi_meta_get_description(m);
	}
	if (strcmp(BOOK_META_LANG, mdata) == 0) {
		return mobi_meta_get_language(m);
	}
	return NULL;
}

/**
 * 根据资源路径获取资源数据
 */
BookResource           *book_mobi_get_resource(BookMobi *bookmobi,const gchar *url)
{
  if(url==NULL)
    {
      return NULL;
    }
 BookResource *bookRes = NULL;
	GBytes * html = NULL;
	GBytes *tempContent = NULL;
	char *chapterUrl;
MOBIRawml *rawml=book_mobi_get_rawml(bookmobi);
  if(rawml==NULL)
    {
      return NULL;
    }



/*
  char partname[200];
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
  */
chapterUrl=url;
	if (memcmp(chapterUrl, "resource",8) ==0) {
		uint32_t part_id = atoi(chapterUrl + 8);
		MOBIPart *curr = mobi_get_resource_by_uid(rawml, part_id);
		if (curr != NULL && curr->size > 0) {
			html = g_bytes_new_static(curr->data, curr->size);
			MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
			bookRes = g_malloc(sizeof(BookResource));
			bookRes->mime = g_strdup(file_meta.mime_type);
			bookRes->uri = url;
			bookRes->data = html;
			return bookRes;
		}
		return NULL;
	}else if (memcmp(chapterUrl, "flow",4)==0) {
		uint32_t part_id = atoi(chapterUrl + 4);
		MOBIPart *curr = mobi_get_flow_by_uid(rawml, part_id);
		if (curr != NULL && curr->size > 0) {
			html = g_bytes_new_static(curr->data, curr->size);
			MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
			//			bookData=book_data_new (g_strdup_printf("%s",file_meta.mime_type),url,part_id,html);
			bookRes = g_malloc(sizeof(BookResource));
			bookRes->mime = g_strdup(file_meta.mime_type);
			bookRes->uri = url;
			bookRes->data = html;
			return bookRes;

		}
		return NULL;
	}else if (memcmp(chapterUrl, "part",4) ==0 ) {
		uint32_t part_id = atoi(chapterUrl + 4);
		//  printf("mobi:准备获取part，uri=%s,fid=%s,uid=%zu,rawml==NULL:%d \n", chapterUrl, chapterUrl + 4,part_id,rawml==NULL);
		MOBIPart *curr = mobi_get_part_by_uid(rawml, part_id);
		if (curr != NULL && curr->size > 0) {
			html = g_bytes_new_static(curr->data, curr->size);
			MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
			bookRes = g_malloc(sizeof(BookResource));
			bookRes->mime = g_strdup(file_meta.mime_type);
			bookRes->uri = url;
			bookRes->data = html;
			return bookRes;
		}
		return NULL;
	}
	g_free(chapterUrl);

	return NULL;


}

/**
 * 获取章节
 */
BookResource   *book_mobi_get_chapter(BookMobi *bookmobi, gint index)
{
  g_printf("获取第%d章\n",index);
	BookResource *bookRes = NULL;
	GBytes * html = NULL;
	GBytes *tempContent;
	gchar *mime = NULL;
MOBIRawml *rawml=book_mobi_get_rawml(bookmobi);
if(rawml!=NULL)
    {
       g_printf("获取第%d章哦\n",index);
	MOBIPart *curr = mobi_get_part_by_uid(rawml, index);
	if (curr != NULL && curr->size > 0) {
		MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
		mime = g_strdup_printf("%s", file_meta.mime_type);
		html = g_bytes_new_static(curr->data, curr->size);

		bookRes = g_malloc(sizeof(BookResource));
		bookRes->mime = g_strdup(file_meta.mime_type);
		bookRes->uri = g_strdup_printf("part%05d.html", index);
		bookRes->data = html;
		return bookRes;

	}

          }
	return NULL;

}


gint book_mobi_get_chapter_num(BookMobi *bookmobi )
{
  gint num=1;
  MOBIRawml *rawml=book_mobi_get_rawml(bookmobi);
	if (rawml != NULL && rawml->markup != NULL) {
		if (rawml->skel != NULL) {
			num= rawml->skel->entries_count;
		}
	}
  g_printf("chapter num=%d\n",num);
  return num;
}




BookNavPoint * book_mobi_nav_get_last(BookNavPoint * nav)
{
	if (nav == NULL) {
		return nav;
	}else{
		if (nav->hasNext) {
			return book_mobi_nav_get_last(nav->next);
		}else{
			return nav;
		}
	}
}

void book_mobi_nav_add_child(BookNavPoint * parentNav, BookNavPoint * childNav)
{
	if (parentNav != NULL && parentNav != childNav) {
		if (parentNav->hasChildren) {
			BookNavPoint * lastNav = book_mobi_nav_get_last(parentNav->children);
			lastNav->hasNext = TRUE;
			lastNav->next = childNav;
		}else{
			parentNav->hasChildren = TRUE;
			parentNav->children = childNav;
		}
	}
}



BookNavPoint* book_mobi_get_nav(BookMobi *bookmobi)
{
	//测试打印目录
 MOBIRawml *rawml;
	BookNavPoint     *navPoint = g_malloc(sizeof(BookNavPoint));
	navPoint->id = NULL;
	navPoint->playOrder = NULL;
	navPoint->src = NULL;
	navPoint->text = NULL;
	navPoint->hasNext = FALSE;
	navPoint->hasChildren = FALSE;
  rawml=book_mobi_get_rawml(bookmobi);
  if(rawml==NULL)
    {
      return navPoint;
    }
	size_t i = 0;
	MOBI_RET ret;
	const size_t count = rawml->ncx->entries_count;
	if (count < 1) {
		return navPoint;
	}
	BookNavPoint *ncx = malloc(count * sizeof(BookNavPoint));
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
		ncx[i] = (BookNavPoint) {text, str_int, str_int, target, FALSE, FALSE, NULL, NULL };
		if (level == 0) {
			if (i > 0) {
				BookNavPoint *lastNav = book_mobi_nav_get_last(&ncx[0]);
				lastNav->hasNext = TRUE;
				lastNav->next = &ncx[i];
			}
		}else if (level > 0) {
			book_mobi_nav_add_child(&ncx[parent], &ncx[i]);
		}
		i++;
	}
	navPoint->hasChildren = TRUE;
	navPoint->children = ncx;
	return navPoint;
}

