

#include "book-common.h"

void test_book_meta()
{
   GError *error = NULL;
  const gchar *bookId="test001";
     GSList *iterator = NULL;
  BookMeta *bookMeta;

   BookDatabase *db=book_database_new("/home/ljc/.book/book.db");
  book_database_open(db,&error);
  if(error!=NULL)
    book_log_gerror(1,"打开数据库失败:%s\n",error);

  //验证是否表存在
  gboolean isExit=book_database_exit_table (db,"book_info");
  book_log_info(" book_info表是否存在：%d\n ",isExit);
  if(isExit)
    {
      book_log_debug("==================测试新增记录================\n");
 bookMeta=book_meta_initWithMeta(bookId,"测试数据","hi ljc",NULL,NULL,NULL,NULL,NULL);
gint res=book_database_insert_bookmeta(db,bookMeta,&error);
      book_log_info("插入新记录返回结果:%d\n",res);
g_clear_object(&bookMeta);
       if(error!=NULL)
    book_log_gerror(1,"新增数据失败:%s\n",error);
    }else{
      book_log_debug("==================创建新表================\n");
     gint res= book_database_create_bookmeta (db,error);
      book_log_debug("创建新表返回结果:%d\n",res);
      if(error!=NULL)
         book_log_gerror(1,"新增数据失败:%s\n",error);
    }

  book_log_debug("==================测试根据ID获取记录================\n");
 bookMeta= book_database_get_bookmeta(db, bookId);
  if(bookMeta!=NULL)
    {
      g_free(book_meta_toString (bookMeta));

    }
  book_log_debug("==================测试修改记录================\n");
  //bookMeta->src=g_strdup("/workspace/temp/test.mk");
  book_meta_set_mark (bookMeta,"sssfsf");
 gint res= book_database_updateMark_bookmeta (db, bookMeta, error);
  if(res>0)
    {  g_clear_object(&bookMeta);
      bookMeta= book_database_get_bookmeta(db, bookId);
  if(bookMeta!=NULL)
    {
      g_free(book_meta_toString (bookMeta));
      g_clear_object(&bookMeta);
    }
    }

book_log_debug("==================测试查询数据关键================\n");
 GSList *bookInfoList=book_database_query_bookmeta(db,"测试数据");
for (iterator = bookInfoList; iterator; iterator = iterator->next) {
       bookMeta=(BookMeta *)iterator->data;
       g_free(book_meta_toString (bookMeta));
      g_clear_object(&bookMeta);
    }
  g_slist_free(bookInfoList);

  book_log_debug("==================删除数据================\n");
book_database_delete_bookmeta (db,bookId, error);

 book_database_close(db);
  g_clear_object(&db);
}

void test_book_param()
{
 GError *error = NULL;
  const gchar *bookId="bookdirectory2";
gchar* paramValue;
   BookDatabase *db=book_database_new("/home/ljc/.book/book.db");
  book_database_open(db,&error);
   book_log_debug("==================查询数据================\n");
paramValue=book_database_get_bookparam(db,bookId);
  book_log_info ("bookId=%s,data=%s\n",bookId,paramValue);
g_free(paramValue);

 book_log_debug("==================新增数据================\n");
gint res=book_database_insert_bookparam(db,bookId,"test",error);
if(res==0)
    {
      book_log_info("新增失败：%d\n",res);
     // book_log_gerror (1,"新增数据失败:%s\n",error);
    }
  if(error)
    book_log_error("新增失败,error:%s\n",error->message);
  book_log_debug("==================查询数据2================\n");
  paramValue=book_database_get_bookparam(db,bookId);
  book_log_info ("bookId=%s,data=%s\n",bookId,paramValue);
g_free(paramValue);

  book_database_delete_bookparam(db,bookId,error);
  book_log_debug("==================完成测试================\n");

  book_database_close(db);
}

int
main (int argc, char **argv)
{
  book_log_debug("bookdataApp\n");
 test_book_meta();
 // test_book_param();
 book_log_debug("bookdataApp成功退出\n");
  return 0;
}
