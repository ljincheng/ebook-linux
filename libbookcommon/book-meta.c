#include "book-meta.h"

#include "book-log.h"

struct _BookMeta {
GObject parent;
gchar  *metaId;
gchar  *title;
gchar  *author;
gchar  *src;
gchar  *openTime;
gchar  *createTime;
gchar *flag;
gchar *mark;
};


G_DEFINE_TYPE (BookMeta, book_meta, G_TYPE_OBJECT)

enum
{
  META_0,
  META_METAID,
  META_TITLE,
  META_AUTHOR,
  META_SRC,
  META_OPENTIME,
  META_CREATETIME,
  META_FLAG,
  META_MARK,
  META_NUM
};

static GParamSpec *properties[META_NUM] = { NULL, };


static void book_meta_init (BookMeta *self)
{
}

static void book_meta_finalize (GObject *object)
{
  book_log_debug("[BookMeta对象退出清理内存]\n");
   BookMeta *self = BOOK_META(object);
   g_clear_pointer (&self->metaId, g_free);
  g_clear_pointer (&self->title, g_free);
  g_clear_pointer (&self->author, g_free);
  g_clear_pointer (&self->src, g_free);
  g_clear_pointer (&self->openTime, g_free);
  g_clear_pointer (&self->createTime, g_free);
  g_clear_pointer (&self->flag, g_free);
  g_clear_pointer (&self->mark, g_free);
  G_OBJECT_CLASS (book_meta_parent_class)->finalize (object);
}


static void
book_meta_set_property (GObject      *gobject,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  BookMeta* bookMeta=BOOK_META(gobject);
  switch (prop_id)
    {
    case META_METAID:
       g_free (bookMeta->metaId);
      bookMeta->metaId = g_value_dup_string (value);
      break;
    case META_TITLE:
       g_free (bookMeta->title);
      bookMeta->title = g_value_dup_string (value);
      break;
    case META_AUTHOR:
       g_free (bookMeta->author);
      bookMeta->author = g_value_dup_string (value);
      break;
    case META_SRC:
       g_free (bookMeta->src);
      bookMeta->src = g_value_dup_string (value);
      break;
      case META_OPENTIME:
       g_free (bookMeta->openTime);
      bookMeta->openTime = g_value_dup_string (value);
      break;
      case META_CREATETIME:
       g_free (bookMeta->createTime);
      bookMeta->createTime = g_value_dup_string (value);
      break;
      case META_FLAG:
       g_free (bookMeta->flag);
      bookMeta->flag = g_value_dup_string (value);
      break;
      case META_MARK:
       g_free (bookMeta->mark);
      bookMeta->mark = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
book_meta_get_property (GObject    *gobject,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
   BookMeta* bookMeta=BOOK_META(gobject);
  switch (prop_id)
    {
    case META_METAID:
      g_value_set_string (value, bookMeta->metaId);
      break;
     case META_TITLE:
      g_value_set_string (value, bookMeta->title);
      break;
       case META_AUTHOR:
      g_value_set_string (value, bookMeta->author);
      break;
       case META_SRC:
      g_value_set_string (value, bookMeta->src);
      break;
       case META_OPENTIME:
      g_value_set_string (value, bookMeta->openTime);
      break;
       case META_CREATETIME:
      g_value_set_string (value, bookMeta->createTime);
      break;
       case META_FLAG:
      g_value_set_string (value, bookMeta->flag);
      break;
       case META_MARK:
      g_value_set_string (value, bookMeta->mark);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
book_meta_class_init (BookMetaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_meta_finalize;
   object_class->set_property = book_meta_set_property;
  object_class->get_property = book_meta_get_property;


   properties[META_METAID] = g_param_spec_string("metaId", "metaId", "the meta id",  NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |  G_PARAM_STATIC_STRINGS);
	properties[META_TITLE] = g_param_spec_string("title", "title", "the book title", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  	properties[META_AUTHOR] =g_param_spec_string("author", "author",  "the book author", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[META_SRC] =g_param_spec_string("src", "src",  "the book src", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[META_OPENTIME] =g_param_spec_string("openTime", "openTime",  "the book opentime", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[META_CREATETIME] =g_param_spec_string("createTime", "createTime",  "the book createTime", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[META_FLAG] =g_param_spec_string("flag", "flag",  "the book flag", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[META_MARK] =g_param_spec_string("mark", "mark",  "the book mark", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(object_class, META_NUM, properties);

}



BookMeta* book_meta_new(void){
  BookMeta *db;
  db=g_object_new(BOOK_TYPE_META,NULL);
  return db;
}

BookMeta* book_meta_initWithMeta(const gchar  *metaId,
                                 const gchar *title,
                                 const gchar *author,
                                 const gchar    *src,
                                 const gchar    *openTime,
                                 const gchar   *createTime,
                                 const gchar *flag,
                                 const gchar *mark){

   BookMeta *db;
  db=g_object_new(BOOK_TYPE_META,NULL);
  if(metaId!=NULL)
  db->metaId=g_strdup(metaId);
  if(title!=NULL)
  db->title=g_strdup(title);
  if(author!=NULL)
  db->author=g_strdup(author);
  if(src!=NULL)
  db->src=g_strdup(src);
  if(openTime!=NULL)
  db->openTime=g_strdup(openTime);
  if(createTime!=NULL)
  db->createTime=g_strdup(createTime);
  if(flag!=NULL)
  db->flag=g_strdup(flag);
  if(mark!=NULL)
  db->mark=g_strdup(mark);
  return db;
}

void book_meta_set_metaId(BookMeta* bookMeta,const gchar *metaId){
  if(bookMeta->metaId!=NULL)
    {
      g_free(bookMeta->metaId);
      bookMeta->metaId=NULL;
    }
  bookMeta->metaId=g_strdup(metaId);
}
void book_meta_set_title(BookMeta* bookMeta,const gchar *title){
  if(bookMeta->title!=NULL)
    {
      g_free(bookMeta->title);
      bookMeta->title=NULL;
    }
  bookMeta->title=g_strdup(title);

}
void book_meta_set_author(BookMeta* bookMeta,const gchar *author){
   if(bookMeta->author!=NULL)
    {
      g_free(bookMeta->author);
      bookMeta->author=NULL;
    }
  bookMeta->author=g_strdup(author);
}
void book_meta_set_src(BookMeta* bookMeta,const gchar *src){
   if(bookMeta->src!=NULL)
    {
      g_free(bookMeta->src);
      bookMeta->src=NULL;
    }
  bookMeta->src=g_strdup(src);
}
void book_meta_set_openTime(BookMeta* bookMeta,const gchar *openTime){
  if(bookMeta->openTime!=NULL)
    {
      g_free(bookMeta->openTime);
      bookMeta->openTime=NULL;
    }
  bookMeta->openTime=g_strdup(openTime);
}
void book_meta_set_createTime(BookMeta* bookMeta,const gchar *createTime){
  if(bookMeta->createTime!=NULL)
    {
      g_free(bookMeta->createTime);
      bookMeta->createTime=NULL;
    }
  bookMeta->createTime=g_strdup(createTime);
}
void book_meta_set_flag(BookMeta* bookMeta,const gchar *flag){
  if(bookMeta->flag!=NULL)
    {
      g_free(bookMeta->flag);
      bookMeta->flag=NULL;
    }
  bookMeta->flag=g_strdup(flag);
}
void book_meta_set_mark(BookMeta* bookMeta,const gchar *mark){
  if(bookMeta->mark!=NULL)
    {
      g_free(bookMeta->mark);
      bookMeta->mark=NULL;
    }
  bookMeta->mark=g_strdup(mark);
}


gchar* book_meta_toString(BookMeta *bookMeta){
gchar * str;
  str=NULL;
  if(bookMeta==NULL)
    {
      return str;
    }
str=g_strdup_printf("'metaId':'%s','title':'%s','author':'%s','src':'%s','openTime':'%s','createTime':'%s','flag':'%s','mark':'%s'",bookMeta->metaId
                    ,bookMeta->title,bookMeta->author,bookMeta->src,bookMeta->openTime,bookMeta->createTime,bookMeta->flag,bookMeta->mark);

  book_log_debug ("BookMeta=%s\n",str);
  return str;
}
