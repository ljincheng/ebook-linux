#include "book-manage.h"

static gchar* DBFILE="book.db";

struct _BookManage {
GObject parent;

BookDatabase* db;

};

G_DEFINE_TYPE (BookManage, book_manage, G_TYPE_OBJECT)


gchar* book_manage_db_file(void)
{
  gchar *dbDir = g_strdup_printf("%s/.book", g_get_home_dir());

  if(  access(dbDir,   NULL)!=0   )
  {
      if(mkdir(dbDir,   0755)==-1)
      {
                     g_printf("mkdir   error");
      }
  }
	gchar *dbFile = g_build_filename(g_get_home_dir(), ".book", DBFILE, NULL);
  return dbFile;
}

static void book_manage_init (BookManage *self)
{
self->db=book_database_new(book_manage_db_file());
}

static void book_manage_finalize (GObject *object)
{
   BookManage *self = BOOK_MANAGE(object);
  book_database_close(self->db);
 g_clear_object(&self->db);
  G_OBJECT_CLASS (book_manage_parent_class)->finalize (object);
}

static void
book_manage_class_init (BookManageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_manage_finalize;
}


BookManage* book_manage_new(void){
  BookManage *db;
  db=g_object_new(BOOK_TYPE_MANAGE,NULL);
  return db;
}

void book_manage_check_database(void){
  BookDatabase* db;
  BookManage* mng;
   GError *error = NULL;
  mng=book_manage_new();
  db=mng->db;
  book_database_open(db,&error);
  book_database_create_bookmeta (db,error);
   if(error)
    {
      book_log_gerror (1,"执行数据库失败:%s\n",error);
    }
  book_database_create_bookparam (db,error);
    if(error)
    {
      book_log_gerror (1,"执行数据库失败:%s\n",error);
    }
  g_clear_object(&mng);
}
gchar * book_manage_query_param(const gchar *queryKey){
  gchar* result;
  BookDatabase* db;
  BookManage* mng;
  GError *error = NULL;
  result=NULL;
  mng=book_manage_new();
  db=mng->db;
  book_database_open(db,&error);
  result=book_database_get_bookparam (db,queryKey);
  g_clear_object(&mng);
  if(error)
    {
      book_log_gerror (1,"执行数据库失败:%s\n",error);
    }
  book_log_debug("查询参数值,%s=%s\n",queryKey,result);
  return result;
}

gboolean book_manage_updateMark_bookmeta(const gchar* bookId,const gchar* mark,GError **error){
    gboolean result;
  BookDatabase* db;
  BookManage* mng;
  BookMeta* bookMeta;
  result=FALSE;
  mng=book_manage_new();
  db=mng->db;
  bookMeta=book_meta_new();
  book_meta_set_metaId (bookMeta,bookId);
  book_meta_set_mark(bookMeta,mark);
  book_database_open(db,&error);
  result=book_database_updateMark_bookmeta(db,bookMeta,error);
  book_log_debug("更新bookId=%s,mark=%s值结果:%d\n",bookId,mark,result?"成功":"失败");
  if(error)
    {
      book_log_gerror (1,"执行数据库失败:%s\n",error);
    }
  g_clear_object(&bookMeta);
  g_clear_object(&mng);
  return result;
}

gboolean book_manage_insert_bookmeta(const gchar *bookId,const  gchar *bookTitle,const  gchar * bookAuthor, const gchar *bookSrc,const gchar *bookFlag,GError **error){
gboolean result;
  BookDatabase* db;
  BookManage* mng;
  BookMeta* bookMeta;
  result=FALSE;
  mng=book_manage_new();
  db=mng->db;
  bookMeta=book_meta_new();
  book_meta_set_metaId (bookMeta,bookId);
  book_meta_set_title (bookMeta,bookTitle);
  book_meta_set_author (bookMeta,bookAuthor);
  book_meta_set_src (bookMeta,bookSrc);
  book_meta_set_flag (bookMeta,bookFlag);
  book_database_open(db,&error);
  result=book_database_insert_bookmeta(db,bookMeta,error);
  book_log_debug("保存bookId=%s,title=%s值结果:%d\n",bookId,bookTitle,result?"成功":"失败");
    if(error)
    {
      book_log_gerror (1,"执行数据库失败:%s\n",error);
    }
  g_clear_object(&bookMeta);
  g_clear_object(&mng);
  return result;
}

BookMeta* book_manage_get_bookmeta(const gchar *metaId){

  BookDatabase* db;
  BookManage* mng;
  BookMeta* bookMeta;
  bookMeta=NULL;
  mng=book_manage_new();
  db=mng->db;
  book_database_open(db,NULL);
  bookMeta=book_database_get_bookmeta(db,metaId);
  book_log_debug("获取bookId=%s结果:%d\n",metaId,book_meta_toString (bookMeta));
  g_clear_object(&mng);
  return bookMeta;
}

GSList * book_manage_list_bookmeta(gint flag,const gchar* orderBy){

  BookDatabase* db;
  BookManage* mng;
  GSList* result;
  result=NULL;
  mng=book_manage_new();
  db=mng->db;
  book_database_open(db,NULL);
  result=book_database_list_bookmeta (db,flag,orderBy);
  g_clear_object(&mng);
  return result;
}

GSList * book_manage_all_bookmeta(const gchar* orderBy){

  BookDatabase* db;
  BookManage* mng;
  GSList* result;
  result=NULL;
  mng=book_manage_new();
  db=mng->db;
  book_database_open(db,NULL);
  result=book_database_all_bookmeta (db,orderBy);
  g_clear_object(&mng);
  return result;
}

GSList * book_manage_lastRead_bookmeta(){

  BookDatabase* db;
  BookManage* mng;
  GSList* result;
  result=NULL;
  mng=book_manage_new();
  db=mng->db;
  book_database_open(db,NULL);
  result=book_database_lastRead_bookmeta (db);
  g_clear_object(&mng);
  return result;
}

GSList * book_manage_nullFlag_bookmeta(){
    BookDatabase* db;
  BookManage* mng;
  GSList* result;
  result=NULL;
  mng=book_manage_new();
  db=mng->db;
  book_database_open(db,NULL);
  result=book_database_nullFlag_bookmeta (db);
  g_clear_object(&mng);
  return result;
}

GSList * book_manage_query_bookmeta(const gchar* titleKey){
  BookDatabase* db;
  BookManage* mng;
  GSList* result;
  result=NULL;
  mng=book_manage_new();
  db=mng->db;
  book_database_open(db,NULL);
  result=book_database_query_bookmeta (db,titleKey);
  book_log_info("查询关键词：%s\n",titleKey);
  g_clear_object(&mng);
  return result;
}
