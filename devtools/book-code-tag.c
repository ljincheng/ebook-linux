#include "book-code-tag.h"

struct _BookCodeTag {
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


G_DEFINE_TYPE (BookCodeTag, book_code_tag, G_TYPE_OBJECT)

enum
{
  CODETAG_0,
  CODETAG_METAID,
  CODETAG_TITLE,
  CODETAG_AUTHOR,
  CODETAG_SRC,
  CODETAG_OPENTIME,
  CODETAG_CREATETIME,
  CODETAG_FLAG,
  CODETAG_MARK,
  CODETAG_NUM
};

static GParamSpec *properties[CODETAG_NUM] = { NULL, };


static void book_code_tag_init (BookCodeTag *self)
{
}

static void book_code_tag_finalize (GObject *object)
{
  book_log_debug("[BookCodeTag对象退出清理内存]\n");
   BookCodeTag *self = BOOK_CODE_TAG(object);
   g_clear_pointer (&self->metaId, g_free);
  g_clear_pointer (&self->title, g_free);
  g_clear_pointer (&self->author, g_free);
  g_clear_pointer (&self->src, g_free);
  g_clear_pointer (&self->openTime, g_free);
  g_clear_pointer (&self->createTime, g_free);
  g_clear_pointer (&self->flag, g_free);
  g_clear_pointer (&self->mark, g_free);
  G_OBJECT_CLASS (book_code_tag_parent_class)->finalize (object);
}


static void
book_code_tag_set_property (GObject      *gobject,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  BookCodeTag* bookMeta=BOOK_CODE_TAG(gobject);
  switch (prop_id)
    {
    case CODETAG_METAID:
       g_free (bookMeta->metaId);
      bookMeta->metaId = g_value_dup_string (value);
      break;
    case CODETAG_TITLE:
       g_free (bookMeta->title);
      bookMeta->title = g_value_dup_string (value);
      break;
    case CODETAG_AUTHOR:
       g_free (bookMeta->author);
      bookMeta->author = g_value_dup_string (value);
      break;
    case CODETAG_SRC:
       g_free (bookMeta->src);
      bookMeta->src = g_value_dup_string (value);
      break;
      case CODETAG_OPENTIME:
       g_free (bookMeta->openTime);
      bookMeta->openTime = g_value_dup_string (value);
      break;
      case CODETAG_CREATETIME:
       g_free (bookMeta->createTime);
      bookMeta->createTime = g_value_dup_string (value);
      break;
      case CODETAG_FLAG:
       g_free (bookMeta->flag);
      bookMeta->flag = g_value_dup_string (value);
      break;
      case CODETAG_MARK:
       g_free (bookMeta->mark);
      bookMeta->mark = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
book_code_tag_get_property (GObject    *gobject,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
   BookCodeTag* bookMeta=BOOK_CODE_TAG(gobject);
  switch (prop_id)
    {
    case CODETAG_METAID:
      g_value_set_string (value, bookMeta->metaId);
      break;
     case CODETAG_TITLE:
      g_value_set_string (value, bookMeta->title);
      break;
       case CODETAG_AUTHOR:
      g_value_set_string (value, bookMeta->author);
      break;
       case CODETAG_SRC:
      g_value_set_string (value, bookMeta->src);
      break;
       case CODETAG_OPENTIME:
      g_value_set_string (value, bookMeta->openTime);
      break;
       case CODETAG_CREATETIME:
      g_value_set_string (value, bookMeta->createTime);
      break;
       case CODETAG_FLAG:
      g_value_set_string (value, bookMeta->flag);
      break;
       case CODETAG_MARK:
      g_value_set_string (value, bookMeta->mark);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
book_code_tag_class_init (BookCodeTagClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_code_tag_finalize;
   object_class->set_property = book_code_tag_set_property;
  object_class->get_property = book_code_tag_get_property;


   properties[CODETAG_METAID] = g_param_spec_string("metaId", "metaId", "the meta id",  NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |  G_PARAM_STATIC_STRINGS);
	properties[CODETAG_TITLE] = g_param_spec_string("title", "title", "the book title", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  	properties[CODETAG_AUTHOR] =g_param_spec_string("author", "author",  "the book author", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[CODETAG_SRC] =g_param_spec_string("src", "src",  "the book src", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[CODETAG_OPENTIME] =g_param_spec_string("openTime", "openTime",  "the book opentime", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[CODETAG_CREATETIME] =g_param_spec_string("createTime", "createTime",  "the book createTime", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[CODETAG_FLAG] =g_param_spec_string("flag", "flag",  "the book flag", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  properties[CODETAG_MARK] =g_param_spec_string("mark", "mark",  "the book mark", NULL, G_PARAM_READWRITE |  G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(object_class, CODETAG_NUM, properties);

}



BookCodeTag* book_code_tag_new(void){
  BookCodeTag *db;
  db=g_object_new(BOOK_TYPE_CODE_TAG,NULL);
  return db;
}

BookCodeTag* book_code_tag_initWithMeta(const gchar  *metaId,
                                 const gchar *title,
                                 const gchar *author,
                                 const gchar    *src,
                                 const gchar    *openTime,
                                 const gchar   *createTime,
                                 const gchar *flag,
                                 const gchar *mark){

   BookCodeTag *db;
  db=g_object_new(BOOK_TYPE_CODE_TAG,NULL);
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

void book_code_tag_set_metaId(BookCodeTag* bookMeta,const gchar *metaId){
  if(bookMeta->metaId!=NULL)
    {
      g_free(bookMeta->metaId);
      bookMeta->metaId=NULL;
    }
  bookMeta->metaId=g_strdup(metaId);
}
void book_code_tag_set_title(BookCodeTag* bookMeta,const gchar *title){
  if(bookMeta->title!=NULL)
    {
      g_free(bookMeta->title);
      bookMeta->title=NULL;
    }
  bookMeta->title=g_strdup(title);

}
void book_code_tag_set_author(BookCodeTag* bookMeta,const gchar *author){
   if(bookMeta->author!=NULL)
    {
      g_free(bookMeta->author);
      bookMeta->author=NULL;
    }
  bookMeta->author=g_strdup(author);
}
void book_code_tag_set_src(BookCodeTag* bookMeta,const gchar *src){
   if(bookMeta->src!=NULL)
    {
      g_free(bookMeta->src);
      bookMeta->src=NULL;
    }
  bookMeta->src=g_strdup(src);
}
void book_code_tag_set_openTime(BookCodeTag* bookMeta,const gchar *openTime){
  if(bookMeta->openTime!=NULL)
    {
      g_free(bookMeta->openTime);
      bookMeta->openTime=NULL;
    }
  bookMeta->openTime=g_strdup(openTime);
}
void book_code_tag_set_createTime(BookCodeTag* bookMeta,const gchar *createTime){
  if(bookMeta->createTime!=NULL)
    {
      g_free(bookMeta->createTime);
      bookMeta->createTime=NULL;
    }
  bookMeta->createTime=g_strdup(createTime);
}
void book_code_tag_set_flag(BookCodeTag* bookMeta,const gchar *flag){
  if(bookMeta->flag!=NULL)
    {
      g_free(bookMeta->flag);
      bookMeta->flag=NULL;
    }
  bookMeta->flag=g_strdup(flag);
}
void book_code_tag_set_mark(BookCodeTag* bookMeta,const gchar *mark){
  if(bookMeta->mark!=NULL)
    {
      g_free(bookMeta->mark);
      bookMeta->mark=NULL;
    }
  bookMeta->mark=g_strdup(mark);
}


gchar* book_code_tag_toString(BookCodeTag *bookMeta){
gchar * str;
  str=NULL;
  if(bookMeta==NULL)
    {
      return str;
    }
str=g_strdup_printf("'metaId':'%s','title':'%s','author':'%s','src':'%s','openTime':'%s','createTime':'%s','flag':'%s','mark':'%s'",bookMeta->metaId
                    ,bookMeta->title,bookMeta->author,bookMeta->src,bookMeta->openTime,bookMeta->createTime,bookMeta->flag,bookMeta->mark);

  book_log_debug ("BookCodeTag=%s\n",str);
  return str;
}
