#include "book-database-bookparam.h"


gint book_database_create_bookparam(BookDatabase* database,GError **error){


gint res=0;
	if (!book_database_exit_table(database,"book_param")) {
    if(!book_database_statement (database,  "create table book_param (id varchar(200) PRIMARY KEY,title varchar(200),data text)"))
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


gchar* book_database_get_bookparam(BookDatabase* db,const gchar* paramId){
gchar *res;
  res=NULL;
if (!book_database_statement(db, "select data from book_param where id=?")) {
			  book_log_debug("查询book_param记录失败:Id=%s\n", paramId);
		}else		if (!book_database_statement_bind_string(db,0, paramId)) {
			    book_log_debug("查询book_info记录设置参数失败:Id=%s\n", paramId);
		}else{

while(book_database_statement_step(db) == BOOK_DATABASE_CODE_ROW)
    {
      gchar * dataValue=book_database_column_string (db, 0);
    res=g_strdup(dataValue);
      break;
    }
    }

 book_database_statement_finalize(db);
return res;
}


gint book_database_update_bookparam(BookDatabase* db,const gchar* paramId,const gchar* paramValue,GError **error)
{

gint res=0;

  if( paramId!=NULL   )
    {
  if (!book_database_statement(db, "update book_param set  data=?  where id=? ")) {
			  book_log_debug("修改book_param记录失败:paramId=%s\n", paramId);
    book_database_get_error (db, error);
		}else		if (!( book_database_statement_bind_string(db,0, paramValue)
        &&  book_database_statement_bind_string(db,1, paramId))
        ) {
			    book_log_debug("修改book_param记录设置参数失败:paramId=%s\n", paramId);
      book_database_get_error (db, error);
		}else if (book_database_statement_step(db)==BOOK_DATABASE_CODE_OK ) {
      res=1;
    }else{
       book_database_get_error (db, error);
    }
     book_database_statement_finalize(db);

    }
  return res;

}


gint book_database_insert_bookparam(BookDatabase* db,const gchar* paramId,const gchar* paramValue,GError **error){

gint res=0;

  if( paramId!=NULL   )
    {
  if (!book_database_statement(db, "insert into book_param (id,data) values(?,?)")) {
			  book_log_debug("新增book_param记录失败:paramId=%s\n", paramId);
    book_database_get_error (db, error);
		}else		if (!( book_database_statement_bind_string(db,0,paramId )
        &&  book_database_statement_bind_string(db,1, paramValue))
        ) {
			    book_log_debug("新增book_param记录设置参数失败:paramId=%s\n", paramId);
      book_database_get_error (db, error);
		}else if (book_database_statement_step(db)==BOOK_DATABASE_CODE_DONE ) {
      res=1;
    }else{
       book_database_get_error (db, error);
    }
     book_database_statement_finalize(db);

    }
  return res;

}


gint book_database_delete_bookparam(BookDatabase* db,const gchar* paramId,GError **error){

  gint res=0;

  if( paramId!=NULL   )
    {
  if (!book_database_statement(db, "delete from  book_param where id=?")) {
			  book_log_debug("删除book_param记录失败:paramId=%s\n", paramId);
    book_database_get_error (db, error);
		}else		if (!book_database_statement_bind_string(db,0,paramId) ){
			    book_log_debug("删除book_param记录设置参数失败:paramId=%s\n", paramId);
      book_database_get_error (db, error);
		}else if (book_database_statement_step(db)==BOOK_DATABASE_CODE_OK ) {
      res=1;
    }else{
       book_database_get_error (db, error);
    }
     book_database_statement_finalize(db);

    }
  return res;
}
