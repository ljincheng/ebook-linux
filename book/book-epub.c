#include "book-epub.h"
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <string.h>

int TOPINDEX=0;


struct _BookEpub {
	GObject parent;
  	GepubDoc *doc;
  gchar *path;
};

G_DEFINE_TYPE (BookEpub, book_epub, G_TYPE_OBJECT)


static void book_epub_init (BookEpub *self)
{
self->doc=NULL;
  self->path=NULL;
}

static void book_epub_finalize (GObject *object)
{
   BookEpub *self = BOOK_EPUB(object);
if(self->path!=NULL)
   g_clear_pointer (&self->path, g_free);

  if(self->doc!=NULL)
    {
      g_clear_object(&self->doc);
    }

  G_OBJECT_CLASS (book_epub_parent_class)->finalize (object);
}

static void
book_epub_class_init (BookEpubClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_epub_finalize;
}


BookEpub *book_epub_new(const gchar *path){


  BookEpub *bookepub;
   GepubDoc *doc;

  bookepub=g_object_new(BOOK_TYPE_EPUB,NULL);
  bookepub->path=g_strdup( path);
  	doc = gepub_doc_new(bookepub->path, NULL);
  bookepub->doc=doc;
  return bookepub;
}


/**
 * 获取基本信息数据
 */
gchar            *book_epub_get_metadata                    (BookEpub *bookepub, const gchar *mdata){
  if(bookepub==NULL || mdata==NULL)
    {
      return NULL;
    }
  GepubDoc *doc=bookepub->doc;
  if(doc==NULL)
    {
  return NULL;
    }
  return  gepub_doc_get_metadata(doc,mdata);
}

/**
 * 根据资源路径获取资源数据
 */
BookResource           *book_epub_get_resource                    (BookEpub *bookepub, const gchar *path){
  GepubDoc *doc;
if(bookepub==NULL || path==NULL)
    {
      return NULL;
    }
 doc =bookepub->doc;
  if(doc==NULL)
    {
  return NULL;
    }
    BookResource *bookRes;
  GBytes * html = NULL;
  gchar *mime=NULL;
  gint chapterIndex=-1;
  GBytes *content=NULL;
  gchar *basePath=NULL;
  gchar *currPath;
  gchar *filePath=NULL;
 // GBytes *tempContent=NULL;
  currPath=gepub_doc_get_current_path(doc);
  basePath=g_path_get_dirname(currPath);
  if(basePath!=NULL && strlen (basePath)>0)
    {
       filePath=g_strdup_printf("%s/%s",basePath,path);
    }else{
       filePath=g_strdup(path);
    }

  mime=gepub_doc_get_resource_mime(doc,path);
  //g_printf("path=%s,basePath=%s,filePath=%s\n",path,basePath,filePath);
  if(mime && !strcmp("application/xhtml+xml",mime))
    {
	  chapterIndex=gepub_doc_get_chapter_by_path(doc,filePath);
	   gepub_doc_set_chapter(doc, chapterIndex);
	  html=gepub_doc_get_current(doc);
	 // html=book_epub_html_with_uris(bookDoc,tempContent);
  }else{
        html= gepub_doc_get_resource(doc, filePath);
   }
//g_free(currPath);
 // g_free(basePath);

  if(html!=NULL){
                    bookRes= g_malloc(sizeof(BookResource));
  bookRes->mime=mime;
  bookRes->uri=path;
  bookRes->data=html;
         }

  return bookRes;
}

/**
 * 获取章节
 */
BookResource   *book_epub_get_chapter  (BookEpub *bookepub, gint index){
   BookResource *bookRes;
 //GBytes * html = NULL;
 GBytes *tempContent=NULL;
 gchar *path=NULL;
 gchar *mime=NULL;
  GepubDoc *doc;
 // g_printf("获取EPUB,index=%d\n",index);
  if(bookepub==NULL || index<0)
    {
      return NULL;
    }
   doc=bookepub->doc;
  if(doc==NULL)
    {
  return NULL;
    }

 gepub_doc_set_chapter(doc, index);

 tempContent=gepub_doc_get_current(doc);
 //html=book_epub_html_with_uris(bookDoc,tempContent);
path=gepub_doc_get_current_path(doc);
 mime=gepub_doc_get_current_mime(doc);

bookRes= g_malloc(sizeof(BookResource));
  bookRes->mime=mime;
  bookRes->uri=path;
  bookRes->data=tempContent;
  return bookRes;
}

/**
 * 获取章节总数量
 */
gint              book_epub_get_chapter_num                  (BookEpub *bookepub){
	GList *chapter;
    GepubDoc *doc;
  gint num=0;
  if(bookepub==NULL)
    {
      return num;
    }
   doc=bookepub->doc;
  if(doc==NULL)
    {
  return num;
    }
  num=  gepub_doc_get_n_chapters(doc);
  return num;
}


gint book_epub_path_to_chapter_index(BookEpub *bookepub,const gchar *path){
  gint chapterindex=-1;
   GepubDoc *doc;
  if(bookepub==NULL || path==NULL)
    {
      return chapterindex;
    }
   doc=bookepub->doc;
  if(doc==NULL)
    {
  return chapterindex;
    }

      gchar *currPath,*basePath,*filePath;
      currPath=gepub_doc_get_current_path(doc);
  basePath=g_path_get_dirname(currPath);
  if(basePath!=NULL && strlen (basePath)>0)
    {
       filePath=g_strdup_printf("%s/%s",basePath,path);
    }else{
       filePath=g_strdup(path);
    }

      chapterindex=gepub_doc_get_chapter_by_path(doc,filePath);


  return chapterindex;
}


void book_epub_loop_child_navpoint(const gchar *basePath,xmlNode *item, BookNavPoint  *parentNavPoint)
{

  xmlNode *labelText = NULL;
	xmlNode * contentNode = NULL;
	xmlChar *text = NULL;
	xmlChar *src = NULL;
	xmlChar *id = NULL;
	xmlChar *playOrder = NULL;
	BookNavPoint   *nextNavPoint = NULL;
  int index=0;
	while (item) {
		if (item->type != XML_ELEMENT_NODE) {
			item = item->next;
			continue;
		}

		if (!strcmp((const char*)item->name, "navPoint")) {
			labelText = gepub_utils_get_element_by_tag(item, "text");
			text = xmlNodeGetContent(labelText);
			contentNode = gepub_utils_get_element_by_tag(item, "content");
			src = xmlGetProp(contentNode, BAD_CAST("src"));
			id = gepub_utils_get_prop(item, "id");
			playOrder = gepub_utils_get_prop(item, "playOrder");
			// g_printf("标题：%s(id:%s,playOrder:%s,src:%s)\n", g_strdup ((const char *) text),id,playOrder,g_strdup ((const char *) src));
			BookNavPoint     *navPoint = g_malloc(sizeof(BookNavPoint));
			navPoint->id = g_strdup((const char*)id);
			navPoint->playOrder = g_strdup((const char*)playOrder);
			navPoint->src =g_strdup(src); //g_strdup((const char*)src);
			navPoint->text = g_strdup((const char*)text);
      navPoint->hasNext=FALSE;
      navPoint->hasChildren=FALSE;
			if (index ==0) {
				nextNavPoint = navPoint;
        if(parentNavPoint==NULL)
          {
            parentNavPoint=nextNavPoint;
          }else{
            parentNavPoint->children = nextNavPoint;
            parentNavPoint->hasChildren=TRUE;
           // g_printf("0父：%s子标题:%s\n",parentNavPoint->text,parentNavPoint->children->text);
          }
			}else    {
				nextNavPoint->next = navPoint;
        nextNavPoint->hasNext=TRUE;
       //  g_printf("父：%s子标题:%s\n",nextNavPoint->text,nextNavPoint->next->text);
        nextNavPoint=nextNavPoint->next ;

        TOPINDEX++;
			}
      index++;
      if(item->children)
        {
			      book_epub_loop_child_navpoint(basePath,item->children, navPoint);
        }
		}
		item = item->next;
	}

}


/**
 * 导航菜单
 */
BookNavPoint * book_epub_get_nav(BookEpub *bookepub){

	xmlDoc *xdoc = NULL;
	xmlNode *root_element = NULL;
	xmlNode *mnode = NULL;
	xmlNode *item = NULL;
	const char *data;
	gsize size;
	GBytes * toc_data;
  gchar *tocfile;
	BookNavPoint   *rootNavPoint = NULL;
  GHashTable *docRes=NULL;
  gchar * basePath;
   GepubDoc *doc;
  if(bookepub==NULL)
    {
      return NULL;
    }
   doc=bookepub->doc;
  if(doc==NULL)
    {
  return NULL;
    }

//toc_data = gepub_doc_get_resource_by_id(doc, "ncx");
//tocfile=g_strdup_printf("%s%s",doc->content_base,"toc.ncx");
//  toc_data=gepub_doc_get_resource(doc,tocfile);
 // g_free(tocfile);

	GList *keys;
 GepubResource *gres;
printf("OK---\n");
  docRes=gepub_doc_get_resources(doc);
	keys = g_hash_table_get_keys(docRes);
basePath=gepub_doc_get_current_path(doc);
  basePath=g_path_get_dirname(basePath);
	while (keys) {
		gres = ((BookNavPoint*)g_hash_table_lookup(docRes, keys->data));
		if (!strcmp(gres->mime, "application/x-dtbncx+xml"))
      {
         toc_data=gepub_doc_get_resource(doc,gres->uri);
			break;
      }
		keys = keys->next;
	}


  g_return_val_if_fail(toc_data != NULL, NULL);
	data = g_bytes_get_data(toc_data, &size);
	// g_printf("获取toc.ncx文件内容：%s\n",data);
	xdoc = xmlRecoverMemory(data, size);
	root_element = xmlDocGetRootElement(xdoc);
	mnode = gepub_utils_get_element_by_tag(root_element, "navMap");
if(mnode==NULL)
    {
      	xmlFreeDoc(xdoc);
          return  NULL;
    }
	item = mnode->children;
  	rootNavPoint = g_malloc(sizeof(BookNavPoint));
			rootNavPoint->id = NULL;
			rootNavPoint->playOrder =NULL;
			rootNavPoint->src =NULL;
			rootNavPoint->text = NULL;
      rootNavPoint->hasNext=FALSE;
      rootNavPoint->hasChildren=FALSE;

   book_epub_loop_child_navpoint(basePath,item, rootNavPoint);
  //g_printf("准备打印：%s标题\n",rootNavPoint->text);
	//gepub_navPoint_print(rootNavPoint);
	//g_free(rootNavPoint);
  //g_printf("END\n");
	xmlFreeDoc(xdoc);

	return rootNavPoint;
}


