
#include "book-doc.h"
#include <string.h>
#include <gio/gio.h>
#include "book-epub.h"
#include "book-mobi.h"



struct _BookDoc {
	GObject parent;
	gchar *path;
  gchar *bookId;
  gint bookType;
  gint chapterNum;
  gint  chapterIndex;
  BookEpub    *bookEpub;
  BookMobi *bookMobi;
};

G_DEFINE_TYPE (BookDoc, book_doc, G_TYPE_OBJECT)

static GQuark
book_error_quark(void)
{
	static GQuark q = 0;

	if (q == 0)
		q = g_quark_from_string("book-quark");
	return q;
}

/**
 * BookDocError:
 * @BOOK_ERROR_INVALID: Invalid file
 *
 * Common errors that may be reported by BookDoc.
 */

//typedef struct _BookDoc      BookDoc;
//typedef struct _BookDocClass BookDocClass;


typedef enum {
	BOOK_ERROR_INVALID = 0, /*< nick=Invalid >*/
} BookDocError;



enum {
	PROP_0,
	PROP_PATH,
	PROP_CHAPTER,
	NUM_PROPS
};



static GParamSpec *properties[NUM_PROPS] = { NULL, };
/*
G_DEFINE_TYPE_WITH_CODE(BookDoc, book_doc, G_TYPE_OBJECT,
			G_IMPLEMENT_INTERFACE(G_TYPE_INITABLE, book_doc_initable_iface_init))
*/


void book_doc_resource_free(BookResource *res)
{
    g_printf("[BookResource]退出清理内存\n");
	g_free(res->mime);
	g_free(res->uri);
  	g_bytes_unref(res->data);
	g_free(res);
}

static void
book_doc_finalize(GObject *object)
{
    g_printf("[BookDoc]退出清理内存\n");
	BookDoc *doc = BOOK_DOC(object);
	g_clear_pointer(&doc->path, g_free);
  	g_clear_pointer(&doc->bookId, g_free);
  if(doc->bookEpub!=NULL)
  	g_clear_object(&doc->bookEpub);
  if(doc->bookMobi!=NULL)
    {
      g_clear_object(&doc->bookMobi);
    }
	G_OBJECT_CLASS(book_doc_parent_class)->finalize(object);
}

static void book_doc_set_property(GObject      *object,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec   *pspec)
{
	BookDoc *doc = BOOK_DOC(object);

	switch (prop_id) {
	case PROP_PATH:
		doc->path = g_value_dup_string(value);
		break;

	case PROP_CHAPTER:
    doc->chapterIndex= g_value_get_int(value);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void book_doc_get_property(GObject    *object,
		       guint prop_id,
		       GValue     *value,
		       GParamSpec *pspec)
{
	BookDoc *doc = BOOK_DOC(object);

	switch (prop_id) {
	case PROP_PATH:
		g_value_set_string(value, doc->path);
		break;

	case PROP_CHAPTER:
		g_value_set_int(value, doc->chapterIndex);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void book_doc_init(BookDoc *doc)
{
  doc->chapterNum=0;
  doc->chapterIndex=0;
  doc->bookEpub=NULL;
  doc->bookId=NULL;
  doc->bookType=0;
  doc->bookMobi=NULL;
}

static void book_doc_class_init(BookDocClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = book_doc_finalize;
	object_class->set_property = book_doc_set_property;
	object_class->get_property = book_doc_get_property;

	properties[PROP_PATH] =
		g_param_spec_string("path",
				    "Path",
				    "Path to the Book document",
				    NULL,
				    G_PARAM_READWRITE |
				    G_PARAM_CONSTRUCT_ONLY |
				    G_PARAM_STATIC_STRINGS);
	properties[PROP_CHAPTER] =
		g_param_spec_int("chapter",
				 "Current chapter",
				 "The current chapter index",
				 -1, G_MAXINT, 0,
				 G_PARAM_READWRITE |
				 G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(object_class, NUM_PROPS, properties);
}


/**
 * gepub_doc_new:
 * @path: the epub doc path
 * @error: (nullable): Error
 *
 * Returns: (transfer full): the new GepubDoc created
 */
BookDoc* book_doc_new(const gchar *path, GError **error)
{
	char *ext;

	if (path == NULL) {
		return NULL;
	}
	BookDoc* doc =  g_object_new(BOOK_TYPE_DOC, "path", path, NULL);
	if (!doc->path) {
    g_printf("获取到空bookDoc\n");
		return NULL;
	}
	//g_assert(doc->path != NULL);
	ext = strrchr(doc->path, '.');
	if (strcmp(ext, ".epub") == 0) {
		//g_printf("bookDoc:epub类型书籍\n");
		BookEpub *bookEpub =  book_epub_new(doc->path);
		if (bookEpub == NULL) {
			//   g_printf("bookDoc:epub书籍加载失败\n");
			if (error != NULL) {
				g_set_error(error, book_error_quark(), BOOK_ERROR_INVALID,  "Invalid epub file: %s", doc->path);
			}
		}else{
			doc->bookEpub = bookEpub;
			doc->bookType = 1;
		}
	}else if (strcmp(ext, ".mobi") == 0) {
		BookMobi *mobi = book_mobi_new(doc->path);
		if (mobi == NULL) {
			g_printf("Invalid mobi file: %s\n", doc->path);
			if (error != NULL) {
				g_set_error(error, book_error_quark(), BOOK_ERROR_INVALID,  "Invalid mobi file: %s", doc->path); \
			}
		}else{
			doc->bookMobi = mobi;
			doc->bookType = 2;
		}

	}else if (strcmp(ext, ".azw3") == 0) {
		BookMobi *mobi = book_mobi_new(doc->path);
		if (mobi == NULL) {
			g_printf("Invalid azw3 file: %s\n", doc->path);
			if (error != NULL) {
				g_set_error(error, book_error_quark(), BOOK_ERROR_INVALID,  "Invalid azw3 file: %s", doc->path);
			}
		}else{
			doc->bookMobi = mobi;
			doc->bookType = 3;
		}
	}

	return doc;
	/*
	   return g_initable_new(BOOK_TYPE_DOC,
	                      NULL, error,
	                      "path", path,
	                      NULL);
	 */
}

/**
 * book_doc_get_metadata:
 * @doc: a #BookDoc
 * @mdata: a metadata name string, BOOK_META_TITLE for example
 *
 * Returns: (transfer full): metadata string
 */
gchar * book_doc_get_metadata(BookDoc *doc, const gchar *mdata)
{
	gchar *ret;
  switch (doc->bookType)
    {
    case 1:
        {
         ret= book_epub_get_metadata (doc->bookEpub, mdata);
        }
      break;
    case 2:
    case 3:
     ret=book_mobi_get_metadata (doc->bookMobi,mdata);
    break;
    }
	return ret;
}


/**
 * 根据资源路径获取资源数据
 */
BookResource           *book_doc_get_resource                    (BookDoc *doc, const gchar *path){

     switch (doc->bookType)
    {
    case 1:
      {
          if(doc->bookEpub!=NULL)
                return book_epub_get_resource (doc->bookEpub, path);
        }
      break;
    case 2:
    case 3:
      if(doc->bookMobi!=NULL )
          return book_mobi_get_resource (doc->bookMobi, path);
    break;
    }
  return NULL;
}

/**
 * 获取章节
 */
BookResource   *book_doc_get_chapter                         (BookDoc *doc, gint index){

 switch (doc->bookType)
    {
    case 1:
      {
          if(doc->bookEpub!=NULL)
                return book_epub_get_chapter (doc->bookEpub, index);
        }
      break;
    case 2:
    case 3:
      if(doc->bookMobi!=NULL )
          return book_mobi_get_chapter (doc->bookMobi, index);
    break;
    }
  return NULL;
}

/**
 * 获取章节总数量
 */
gint              book_doc_get_chapter_num                  (BookDoc *doc){

switch (doc->bookType)
    {
    case 1:
      {
          if(doc->bookEpub!=NULL)
                return book_epub_get_chapter_num (doc->bookEpub);
        }
      break;
    case 2:
    case 3:
      if(doc->bookMobi!=NULL)
          return book_mobi_get_chapter_num (doc->bookMobi);
    break;
    }
  return 0;
}

gint book_doc_path_to_chapter_index(BookDoc *doc,const gchar *path){
gint chapterindex=-1;
  switch (doc->bookType)
    {
    case 1:
      {
          if(doc->bookEpub!=NULL)
          {

            chapterindex= book_epub_path_to_chapter_index (doc->bookEpub,path);
            g_printf("chapterindex:path=%s,chapterindex=%d\n ",path,chapterindex);
          }

        }
      break;
    case 2:
    case 3:
      if(path!=NULL)
        {
          if(strstr(path, "part") != NULL) {
		        chapterindex = atoi(path + 4);
                          }
        }

    break;
    }
  return chapterindex;
}

BookNavPoint * book_doc_get_nav(BookDoc *doc){

switch (doc->bookType)
    {
    case 1:
      {
          if(doc->bookEpub!=NULL)
                return book_epub_get_nav (doc->bookEpub);
        }
      break;
    case 2:
    case 3:
      if(doc->bookMobi!=NULL )
          return book_mobi_get_nav (doc->bookMobi);
    break;
    }
  return NULL;
}
