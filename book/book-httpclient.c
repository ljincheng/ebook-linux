#include "book-httpclient.h"
  #include <curl/curl.h>
#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>
#include <glib-object.h>
#include "book-http-res.h"
#include <string.h>
#include "book-common.h"

struct HttpDataStruct {
  char *data;
  size_t size;
};

struct _BookHttpClient {
	GObject parent;
  char *  userAgent;
};

G_DEFINE_TYPE (BookHttpClient, book_http_client, G_TYPE_OBJECT)


static void
book_http_client_finalize(GObject *object)
{
    g_printf("[BookHttpClient]退出清理内存\n");
	BookHttpClient *httpclient = BOOK_HTTP_CLIENT(object);
 // g_clear_pointer(&httpclient->userAgent, g_free);
	G_OBJECT_CLASS(book_http_client_parent_class)->finalize(object);
}

static void book_http_client_init(BookHttpClient *httpclient)
{
  httpclient->userAgent="libcurl-agent/1.0";
}

static void book_http_client_class_init(BookHttpClientClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = book_http_client_finalize;

}


BookHttpClient* book_http_client_new(void){

BookHttpClient* httpclient =  g_object_new(BOOK_TYPE_HTTP_CLIENT,  NULL);
  return httpclient;
}




static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct HttpDataStruct *mem = (struct HttpDataStruct *)userp;

  char *ptr = realloc(mem->data, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->data = ptr;
  memcpy(&(mem->data[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->data[mem->size] = 0;

  return realsize;
}


gchar* book_http_client_post(BookHttpClient* httpclient,const char* url,GHashTable * form){
  CURL *curl;
  //curl_slist *list;
  struct curl_slist *list=NULL;
  CURLM *multi_handle;
  CURLcode res;
  struct curl_httppost *formpost = NULL;
  struct curl_httppost *lastptr = NULL;
  struct curl_slist *headerlist = NULL;
 struct HttpDataStruct chunk;
  GHashTableIter iter;
    gpointer key, value;

  chunk.data = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;

   curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if( curl ){
//--设置变量
if(form!=NULL)
        {
     g_hash_table_iter_init (&iter, form);

    while(g_hash_table_iter_next(&iter, &key, &value)) {

        g_print("key: %s\n value: %s\n", key , value);
 curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME,key,
                  CURLFORM_COPYCONTENTS,value,CURLFORM_END);
            }
        }

      //--上传图片
     // curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME,"video_file",
     //             CURLFORM_FILE, "d://video_file", CURLFORM_END);
      list = curl_slist_append(NULL,"Authorization:AUTHORIZATION");//请将AUTHORIZATION替换为根据API_KEY和API_SECRET得到的签名认证串
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
      curl_easy_setopt(curl, CURLOPT_URL,url);
      curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA,(void *)&chunk);
       curl_easy_setopt(curl, CURLOPT_USERAGENT, httpclient->userAgent);


      res = curl_easy_perform(curl);

      if( res != CURLE_OK ){

        fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

      }else{
        //  printf("%lu bytes retrieved:\n%s\n", (unsigned long)chunk.size,chunk.data);
      }


      curl_easy_cleanup(curl);

  }
  //free(chunk.data);
 // free(url);
 // free(form);
    curl_global_cleanup();

return chunk.data;
}





book_http_client_test(void){
  gchar* httpdata;
  GError *error;
  error = NULL;
  gchar* res_code;
  gchar* res_msg;
    JsonArray * res_data;
  BookHttpRes *res;


  //请求参数
  GHashTable *hashTable = g_hash_table_new(g_str_hash, g_str_equal);
  g_hash_table_insert(hashTable, "params", "{'bizNo':'H1811121113'}");
  g_hash_table_insert(hashTable, "sign", "e816c011b82dd0d106df7d27b0a7cf1f");

  BookHttpClient *httpclient=book_http_client_new();
  //book_http_client_post(httpclient,"http://www.booktable.cn/tv/tv.jsp","a=1&b=2");
  httpdata=book_http_client_post(httpclient,"http://10.52.10.151:9981/jcpt-api/hhnxd/queryLoanRepayment.do",hashTable);
  g_clear_object(&httpclient);

  g_hash_table_destroy(hashTable);

  g_printf("http data:\n%s\n",httpdata);
  res = json_gobject_from_data (BOOK_TYPE_HTTP_RES, httpdata, strlen(httpdata), &error);
  if (error)
    g_error ("Unable to create instance: %s", error->message);


   g_object_get (G_OBJECT (res),
                "code", &res_code,
                "msg", &res_msg,
                 "data",&res_data,
                NULL);
g_printf("获取的code=%s,msg=%s,data=%s \n",res_code,res_msg,res_data);

BookMeta * bookMeta;
  gchar* testObjJson="{'code':'OK','msg':'成功','data':{'metaId':'001','title':'测试','author':'作者','src':'地址','openTime':'(null)','createTime':'(null)','flag':'(null)','mark':'(null)'}";
  res=json_gobject_from_data(BOOK_TYPE_HTTP_RES,testObjJson,-1,NULL);
  g_object_get(G_OBJECT(res),"data",&bookMeta,NULL);
  if(bookMeta!=NULL)
    {
book_meta_toString (bookMeta);
  g_clear_object(&bookMeta);

    }
//g_printf("获取的code=%s,msg=%s,data=%s \n",book_http_res_get_code(res),book_http_res_get_msg(res), book_http_res_get_data(res));

  g_object_unref(res);
}
