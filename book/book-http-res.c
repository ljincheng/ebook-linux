#include "book-http-res.h"
#include "book-common.h"

struct _BookHttpRes{
  	GObject parent;
  gchar* code;
  gchar* msg;
  BookMeta* data;
};


enum
{
  PROP_0,

  PROP_CODE,
  PROP_MSG,
  PROP_DATA,
  PROP_NUM
};

static GParamSpec *properties[PROP_NUM] = { NULL, };

G_DEFINE_TYPE (BookHttpRes, book_http_res, G_TYPE_OBJECT)

static void book_http_res_init(BookHttpRes *obj)
{
  obj->code=NULL;
  obj->msg=NULL;
  obj->data=NULL;
}

static void
book_http_res_finalize(GObject *object)
{
    g_printf("[BookHttpRes]退出清理内存\n");
	BookHttpRes *obj = BOOK_HTTP_RES(object);
  g_free(obj->code);
  g_free(obj->msg);
  if(obj->data!=NULL)
  g_clear_object(&obj->data);
	G_OBJECT_CLASS(book_http_res_parent_class)->finalize(object);
}


static void
book_http_res_set_property (GObject      *gobject,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  switch (prop_id)
    {
    case PROP_CODE:
       g_free (BOOK_HTTP_RES (gobject)->code);
      BOOK_HTTP_RES (gobject)->code = g_value_dup_string (value);
      break;
    case PROP_MSG:
       g_free (BOOK_HTTP_RES (gobject)->msg);
      BOOK_HTTP_RES (gobject)->msg = g_value_dup_string (value);
      break;
    case PROP_DATA:
       g_free (BOOK_HTTP_RES (gobject)->data);
      BOOK_HTTP_RES (gobject)->data = g_value_get_pointer (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
book_http_res_get_property (GObject    *gobject,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  switch (prop_id)
    {
    case PROP_CODE:
      g_value_set_string (value, BOOK_HTTP_RES (gobject)->code);
      break;
    case PROP_MSG:
      g_value_set_string (value, BOOK_HTTP_RES (gobject)->msg);
      break;
    case PROP_DATA:
      g_value_set_gtype (value, BOOK_HTTP_RES (gobject)->data);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}
static void book_http_res_class_init(BookHttpResClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = book_http_res_finalize;
    object_class->set_property = book_http_res_set_property;
  object_class->get_property = book_http_res_get_property;

  properties[PROP_CODE] =
		g_param_spec_string("code",
				    "code",
				    "the code",
				    NULL,
				    G_PARAM_READWRITE |
				    G_PARAM_CONSTRUCT_ONLY |
				    G_PARAM_STATIC_STRINGS);
	properties[PROP_MSG] =
		g_param_spec_string("msg",
				 "msg",
				 "the msg",
				 NULL,
				    G_PARAM_READWRITE |
				    G_PARAM_CONSTRUCT_ONLY |
				    G_PARAM_STATIC_STRINGS);
  	properties[PROP_DATA] =
		g_param_spec_pointer("data",
				 "data",
				 "the data",
				    G_PARAM_READWRITE |
				    G_PARAM_CONSTRUCT_ONLY |
				    G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(object_class, PROP_NUM, properties);

}



BookHttpRes* book_http_res_new(void)
{

BookHttpRes* obj =  g_object_new(BOOK_TYPE_HTTP_RES,  NULL);
  return obj;

}

gchar* book_http_res_get_code(BookHttpRes* res){
  return res->code;
}
gchar* book_http_res_get_msg(BookHttpRes* res){
  return res->msg;
}
 
