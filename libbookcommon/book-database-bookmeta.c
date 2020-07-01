#include "book-database-bookmeta.h"

gint book_database_create_bookmeta(BookDatabase* database,GError **error){

gint res=0;
	if (!book_database_exit_table(database,"book_info")) {
    if(!book_database_statement (database,  "create table book_info (id varchar(200) PRIMARY KEY,title varchar(200),author varchar(100),src varchar(200),openTime datetime,flag int,mark text,createTime datetime)"))
      {
        book_database_get_error (database, error);
      }else if(book_database_statement_step(database)==BOOK_DATABASE_CODE_DONE)
      {
        res=1;
      }else{
        book_database_get_error (database, error);
      }
		}
  return res;
}
/**
 * 保存书籍基本要素
 */
gint book_database_insert_bookmeta(BookDatabase* database, BookMeta* bookMeta,GError **error){
  gint res=0;
  gchar *bookId, *bookTitle, *bookAuthor, *bookPath,*bookFlag;
   g_object_get (G_OBJECT (bookMeta),
                "metaId", &bookId,
                "title", &bookTitle,
                 "author",&bookAuthor,
                 "src",&bookPath,
                 "flag",&bookFlag,
                NULL);
  if( bookId!=NULL && bookMeta!=NULL  )
    {
      gchar *sql=g_strdup_printf("insert into book_info(id,title,author,src,flag,createTime) values(?,?,?,?,%s,datetime('now') )",bookFlag);
  if (!book_database_statement(database, sql)) {
			  book_log_debug("新增book_info记录失败:bookId=%s\n", bookId);
    book_database_get_error (database, error);
		}else		if (!(book_database_statement_bind_string(database,0, bookId)
        &&  book_database_statement_bind_string(database,  1, bookTitle)
        &&  book_database_statement_bind_string(database, 2, bookAuthor)
        &&  book_database_statement_bind_string(database, 3, bookPath))
        ) {
			    book_log_debug("新增book_info记录设置参数失败:bookId=%s\n", bookId);
      book_database_get_error (database, error);
		}else if (book_database_statement_step(database)==BOOK_DATABASE_CODE_OK ) {
			    book_log_debug("数据库执行脚本失败:bookId=%s\n", bookId);
      res=1;
    }else{
       book_database_get_error (database, error);
    }
     book_database_statement_finalize(database);
    g_free(sql);
    }
 book_log_debug("保存记录结果:%d\n",res);
  return res;

}


BookMeta*  book_database_get_bookmeta(BookDatabase* db,const gchar * metaId){
BookMeta *bookMeta;
  bookMeta=NULL;
if (!book_database_statement(db, "select id,title,author,src,openTime,flag,mark,createTime from book_info where id=?")) {
			  book_log_debug("查询book_info记录失败:Id=%s\n", metaId);
		}else		if (!book_database_statement_bind_string(db,0, metaId)) {
			    book_log_debug("查询book_info记录设置参数失败:Id=%s\n", metaId);
		}

while(book_database_statement_step(db) == BOOK_DATABASE_CODE_ROW)
    {
      gchar * idValue=book_database_column_string (db, 0);
      gchar * titleValue=book_database_column_string(db, 1);
      gchar * authorValue=book_database_column_string(db, 2);
      gchar * srcValue=book_database_column_string(db, 3);
      gchar * openTimeValue=book_database_column_string(db, 4);
      gchar * flagValue=book_database_column_string(db, 5);
      gchar * markValue=book_database_column_string(db, 6);
      gchar * createTime=book_database_column_string(db, 7);
      bookMeta=book_meta_initWithMeta (idValue, titleValue, authorValue, srcValue, openTimeValue, createTime, flagValue, markValue);
      break;
    }
 book_database_statement_finalize(db);
  return bookMeta;
}

/**
 *  获取书籍基本要素列表
 */
GSList * book_database_list_bookmeta(BookDatabase* db, gint flag,const gchar* orderBy){
  	GSList *list = NULL;
BookMeta *bookMeta;
  bookMeta=NULL;
  gchar* sql;
  if(orderBy==NULL)
    {
      sql=g_strdup("select id,title,author,src,openTime,flag,mark,createTime from book_info where flag=?   order by title,createTime desc");
    }else{
      sql=g_strdup_printf("select id,title,author,src,openTime,flag,mark,createTime from book_info where flag=?  order by %s",orderBy);
    }
  book_database_statement_finalize(db);
if (!book_database_statement(db, sql)) {
			  book_log_debug("查询book_info记录失败:flag=%d\n", flag);
		}
else		if (!book_database_statement_bind_int(db,0, flag)) {
			    book_log_debug("查询book_info记录设置参数失败:flag=%d\n", flag);
		}else{
    while(book_database_statement_step(db) == BOOK_DATABASE_CODE_ROW )
        {
          gchar * idValue=book_database_column_string (db, 0);
          gchar * titleValue=book_database_column_string(db, 1);
          gchar * authorValue=book_database_column_string(db, 2);
          gchar * srcValue=book_database_column_string(db, 3);
          gchar * openTimeValue=book_database_column_string(db, 4);
          gchar * flagValue=book_database_column_string(db, 5);
          gchar * markValue=book_database_column_string(db, 6);
          gchar * createTime=book_database_column_string(db, 7);
          bookMeta=book_meta_initWithMeta (idValue, titleValue, authorValue, srcValue, openTimeValue, createTime, flagValue, markValue);
           list = g_slist_append(list, bookMeta);
        }
           }
 book_database_statement_finalize(db);
  g_free(sql);
  return list;
}

GSList * book_database_query_bookmeta(BookDatabase* db,const gchar* titleKey){

   	GSList *list = NULL;
BookMeta *bookMeta;
  bookMeta=NULL;
  GError *error;
  gchar * sqlValue_title=g_strdup_printf("%%%s%%",titleKey);
  book_database_statement_finalize(db);
if (!book_database_statement(db, "select id,title,author,src,openTime,flag,mark,createTime from book_info where title like ? or author like ? order by title,openTime")) {
			  book_log_debug("查询book_info记录失败:titleKey=%s\n", titleKey);
   book_log_debug("查询book_info记录失败:titleKeyOOOOO\n");
  book_database_get_error(db,&error);
  if(error)
    {
      book_log_error("查询bookmeta失败原因：%s\n",error->message);
    }
		}
else		if (!book_database_statement_bind_string(db,0, sqlValue_title)) {
			    book_log_debug("查询book_info记录设置参数失败:titleKey=%s\n", titleKey);
}else		if (!book_database_statement_bind_string(db,1, sqlValue_title)) {
			    book_log_debug("查询book_info记录设置参数失败:titleKey=%s\n", titleKey);
		}else{
while(book_database_statement_step(db) == BOOK_DATABASE_CODE_ROW )
    {
      gchar * idValue=book_database_column_string (db, 0);
      gchar * titleValue=book_database_column_string(db, 1);
      gchar * authorValue=book_database_column_string(db, 2);
      gchar * srcValue=book_database_column_string(db, 3);
      gchar * openTimeValue=book_database_column_string(db, 4);
      gchar * flagValue=book_database_column_string(db, 5);
      gchar * markValue=book_database_column_string(db, 6);
      gchar * createTime=book_database_column_string(db, 7);
       book_log_debug("title=%s\n",titleValue);
      bookMeta=book_meta_initWithMeta (idValue, titleValue, authorValue, srcValue, openTimeValue, createTime, flagValue, markValue);
       list = g_slist_append(list, bookMeta);
    }
           }
 book_database_statement_finalize(db);
  g_free(sqlValue_title);
  return list;

}

GSList * book_database_all_bookmeta(BookDatabase* db,const gchar* orderBy){

GSList *list = NULL;
BookMeta *bookMeta;
  bookMeta=NULL;
  gchar * sql;
  if(orderBy==NULL)
    {
      sql=g_strdup("select id,title,author,src,openTime,flag,mark,createTime from book_info  order by title,createTime desc");
    }else{
      sql=g_strdup_printf("select id,title,author,src,openTime,flag,mark,createTime from book_info order by %s",orderBy);
    }

  book_database_statement_finalize(db);
if (!book_database_statement(db, sql)) {
			  book_log_debug("查询book_info记录失败\n");
		} else{
    while(book_database_statement_step(db) == BOOK_DATABASE_CODE_ROW )
        {
          gchar * idValue=book_database_column_string (db, 0);
          gchar * titleValue=book_database_column_string(db, 1);
          gchar * authorValue=book_database_column_string(db, 2);
          gchar * srcValue=book_database_column_string(db, 3);
          gchar * openTimeValue=book_database_column_string(db, 4);
          gchar * flagValue=book_database_column_string(db, 5);
          gchar * markValue=book_database_column_string(db, 6);
          gchar * createTime=book_database_column_string(db, 7);
          bookMeta=book_meta_initWithMeta (idValue, titleValue, authorValue, srcValue, openTimeValue, createTime, flagValue, markValue);
           list = g_slist_append(list, bookMeta);
        }
           }
 book_database_statement_finalize(db);
  g_free(sql);
  return list;
}

GSList * book_database_lastRead_bookmeta(BookDatabase* db){

GSList *list = NULL;
BookMeta *bookMeta;
  bookMeta=NULL;
  book_database_statement_finalize(db);
if (!book_database_statement(db, "select id,title,author,src,openTime,flag,mark,createTime from book_info  where openTime>date('now','-7 day') order by openTime desc")) {
			  book_log_debug("查询book_info记录失败\n");
		} else{
    while(book_database_statement_step(db) == BOOK_DATABASE_CODE_ROW )
        {
          gchar * idValue=book_database_column_string (db, 0);
          gchar * titleValue=book_database_column_string(db, 1);
          gchar * authorValue=book_database_column_string(db, 2);
          gchar * srcValue=book_database_column_string(db, 3);
          gchar * openTimeValue=book_database_column_string(db, 4);
          gchar * flagValue=book_database_column_string(db, 5);
          gchar * markValue=book_database_column_string(db, 6);
          gchar * createTime=book_database_column_string(db, 7);
          bookMeta=book_meta_initWithMeta (idValue, titleValue, authorValue, srcValue, openTimeValue, createTime, flagValue, markValue);
           list = g_slist_append(list, bookMeta);
        }
    }
 book_database_statement_finalize(db);
  return list;

}


GSList * book_database_nullFlag_bookmeta(BookDatabase* db){

GSList *list = NULL;
BookMeta *bookMeta;
  bookMeta=NULL;
  book_database_statement_finalize(db);
if (!book_database_statement(db, "select id,title,author,src,openTime,flag,mark,createTime from book_info  where flag is null order by openTime desc")) {
			  book_log_debug("查询book_info记录失败\n");
		} else{
    while(book_database_statement_step(db) == BOOK_DATABASE_CODE_ROW )
        {
          gchar * idValue=book_database_column_string (db, 0);
          gchar * titleValue=book_database_column_string(db, 1);
          gchar * authorValue=book_database_column_string(db, 2);
          gchar * srcValue=book_database_column_string(db, 3);
          gchar * openTimeValue=book_database_column_string(db, 4);
          gchar * flagValue=book_database_column_string(db, 5);
          gchar * markValue=book_database_column_string(db, 6);
          gchar * createTime=book_database_column_string(db, 7);
          bookMeta=book_meta_initWithMeta (idValue, titleValue, authorValue, srcValue, openTimeValue, createTime, flagValue, markValue);
           list = g_slist_append(list, bookMeta);
        }
    }
 book_database_statement_finalize(db);
  return list;
}

gint book_database_update_bookmeta(BookDatabase* database,BookMeta* bookMeta,GError **error){

gint res=0;
  gchar *bookId, *bookTitle, *bookAuthor, *bookPath,*bookMark,*bookFlag;
   g_object_get (G_OBJECT (bookMeta),
                "metaId", &bookId,
                "title", &bookTitle,
                 "author",&bookAuthor,
                 "src",&bookPath,
                 "mark",&bookMark,
                 "flag",&bookFlag,
                NULL);
  if( bookId!=NULL && bookMeta!=NULL  )
    {
  if (!book_database_statement(database, "update book_info set title=?,author=?,src=?,mark=?,flag=?,openTime=datetime('now') where id=? ")) {
			  book_log_debug("新增book_info记录失败:bookId=%s\n", bookId);
    book_database_get_error (database, error);
		}else		if (!( book_database_statement_bind_string(database,  0, bookTitle)
        &&  book_database_statement_bind_string(database, 1, bookAuthor)
        &&  book_database_statement_bind_string(database, 2, bookPath)
                  &&  book_database_statement_bind_string(database, 3, bookMark)
                  &&  book_database_statement_bind_string(database, 4, bookFlag)
        &&  book_database_statement_bind_string(database,5, bookId))
        ) {
			    book_log_debug("新增book_info记录设置参数失败:bookId=%s\n", bookId);
      book_database_get_error (database, error);
		}else if (book_database_statement_step(database)==BOOK_DATABASE_CODE_OK ) {
			    book_log_debug("数据库执行脚本失败:bookId=%s\n", bookId);
      res=1;
    }
     book_database_statement_finalize(database);

    }
  return res;

}

gint book_database_updateMark_bookmeta(BookDatabase* db,BookMeta* bookMeta,GError **error){

gint res=0;
  gchar *bookId,  *bookMark;
   g_object_get (G_OBJECT (bookMeta),
                "metaId", &bookId,
                 "mark",&bookMark,
                NULL);
  if( bookId!=NULL && bookMeta!=NULL  )
    {
  if (!book_database_statement(db, "update book_info set  mark=?,openTime=datetime('now') where id=? ")) {
			  book_log_debug("新增book_info记录失败:bookId=%s\n", bookId);
    book_database_get_error (db, error);
		}else		if (!( book_database_statement_bind_string(db,0, bookMark)
        &&  book_database_statement_bind_string(db,1, bookId))
        ) {
			    book_log_debug("修改book_info记录设置参数失败:bookId=%s\n", bookId);
      book_database_get_error (db, error);
		}else if (book_database_statement_step(db)==BOOK_DATABASE_CODE_OK ) {
			    book_log_debug("数据库执行脚本失败:bookId=%s\n", bookId);
      res=1;
    }
     book_database_statement_finalize(db);

    }
  return res;
}

gint book_database_delete_bookmeta(BookDatabase* db,const gchar* metaId,GError **error){


gint res=0;

  if( metaId!=NULL && db!=NULL  )
    {
  if (!book_database_statement(db, "delete from  book_info  where id=? ")) {
			  book_log_debug("删除book_info记录失败:bookId=%s\n", metaId);
    book_database_get_error (db, error);
		}else		if (!book_database_statement_bind_string(db,0, metaId)) {
			    book_log_debug("删除book_info记录设置参数失败:bookId=%s\n", metaId);
      book_database_get_error (db, error);
		}else if (book_database_statement_step(db)==BOOK_DATABASE_CODE_OK ) {
			    book_log_debug("数据库执行脚本失败:bookId=%s\n", metaId);
      res=1;
    }
     book_database_statement_finalize(db);

    }
  return res;
}
