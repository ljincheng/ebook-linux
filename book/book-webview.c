#include "book-webview.h"

#include <JavaScriptCore/JSValueRef.h>
#include <locale.h>

#define  SHEET "var mySheet = document.styleSheets[0];"
#define  ADDCSSRULE "function addCSSRule(selector, newRule) {if (mySheet.addRule) {mySheet.addRule(selector, newRule);} else {ruleIndex = mySheet.cssRules.length;mySheet.insertRule(selector + '{' + newRule + ';}', ruleIndex);}}"

static gchar *BOOK_BACKGROUND_0="document.body.style.background='#fff';  document.body.style.color='';";
static gchar *BOOK_BACKGROUND_1="document.body.style.background='#fffaf2';  document.body.style.color='#282828';";
static gchar *BOOK_BACKGROUND_2="document.body.style.background='#E8E8E7';  document.body.style.color='#282828';";
static gchar *BOOK_BACKGROUND_3="document.body.style.background='#1f1e1f';  document.body.style.color='#b6b6b6';";
static gchar * BOOKSCHEMEURL="book://index.bo";
struct _BookWebview {
	WebKitWebView parent;
BookDoc  *bookDoc;
  gint chapterIndex;
  gint chapterNum;
  gint pageWidth;
  gint pageHeight;
  gint pagePos;
  gint pageMaxPos;
  gint pageNum;
  gint pageIndex;
  gint splitPage;
  gint isGoBack;
  gint pageInitPos;
  bool isLoadUrl;
  gchar *background;
};

struct _BookWebviewClass {
	WebKitWebViewClass parent_class;
};

enum {
	PROP_0,
	PROP_DOC,
	PROP_PAGINATE,
	PROP_CHAPTER,
	PROP_N_CHAPTERS,
	PROP_CHAPTER_POS,
	NUM_PROPS
};

static GParamSpec *properties[NUM_PROPS] = { NULL, };

void book_webview_set_html(BookWebview *widget, BookResource *bookRes);

G_DEFINE_TYPE(BookWebview, book_webview, WEBKIT_TYPE_WEB_VIEW)

//======================================
//加载页面事件
// ======================================
static void book_webview_page_scroll_to(BookWebview *webview,gint pos){
  gchar * script=g_strdup_printf("pageScroll(%d)",pos);
  g_printf("book_webview_page_scroll_to:%s,pagePos=%d,pageMaxPos=%d\n",script,webview->pagePos,webview->pageMaxPos);
 webkit_web_view_run_javascript(webview,script, NULL, NULL,NULL);
  g_free(script);
  webview->isGoBack=0;
}
static void book_webview_page_pageJumpPos(BookWebview *webview,gint pos){
  gchar * script=g_strdup_printf("pageJumpPos(%d)",pos);
  g_printf("book_webview_page_scroll_to:%s,pagePos=%d,pageMaxPos=%d\n",script,pos,webview->pageMaxPos);
 webkit_web_view_run_javascript(webview,script, NULL, NULL,NULL);
  g_free(script);
  webview->isGoBack=0;
}

static void
book_webview_pagination_pageIndex_initialize_finished(GObject      *object,
			       GAsyncResult *result,
			       gpointer user_data)
{

	WebKitJavascriptResult *js_result;
	JSValueRef value;
	JSGlobalContextRef context;
	GError                 *error = NULL;
	BookWebview            *widget = BOOK_WEBVIEW (user_data);

	js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);
	if (!js_result) {
		g_warning("Error running javascript: %s", error->message);
		g_error_free(error);
		return;
	}

	context = webkit_javascript_result_get_global_context(js_result);
	value = webkit_javascript_result_get_value(js_result);
	if (JSValueIsNumber(context, value)) {
		double n;

		n = JSValueToNumber(context, value, NULL);
		widget->pagePos = (int)n;

	webkit_javascript_result_unref(js_result);
    g_printf("页面总宽度：pagePos=%d\n",widget->pagePos);
}
}
static void
book_webview_pagination_initialize_finished(GObject      *object,
			       GAsyncResult *result,
			       gpointer user_data)
{

	WebKitJavascriptResult *js_result;
	JSValueRef value;
	JSGlobalContextRef context;
	GError                 *error = NULL;
	BookWebview            *widget = BOOK_WEBVIEW (user_data);

	js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);
	if (!js_result) {
		g_warning("Error running javascript: %s", error->message);
		g_error_free(error);
		return;
	}

	context = webkit_javascript_result_get_global_context(js_result);
	value = webkit_javascript_result_get_value(js_result);
	if (JSValueIsNumber(context, value)) {
		double n;

		n = JSValueToNumber(context, value, NULL);
		widget->pageWidth = (int)n;

}else if(JSValueIsString (context,value)){
  g_printf("返回是字符串\n");
  gint maxPos=0;
  gint pos=0;
  gint width=0;
  gint height=0;

  JSStringRef stringRef = JSValueToStringCopy(context,value, NULL);
   size_t bufferSize = JSStringGetMaximumUTF8CStringSize(stringRef);
  char* pagePosInfoValue = g_malloc(bufferSize);
   JSStringGetUTF8CString(stringRef, pagePosInfoValue, bufferSize);

  g_printf("pagePosInfoValue=%s\n",pagePosInfoValue);

    char *temp = strtok(pagePosInfoValue, ",");
    while (temp) {
	    if (strcmp("maxpos", temp) == 0) {
		    temp = strtok(NULL, ",");
		    maxPos = atoi(temp);
	    }else if (strcmp("pos", temp) == 0) {
		    temp = strtok(NULL, ",");
		    pos = atoi(temp);
	    }else if (strcmp("width", temp) == 0) {
		    temp = strtok(NULL, ",");
		    width = atoi(temp);
    }else if (strcmp("height", temp) == 0) {
		    temp = strtok(NULL, ",");
		    height = atoi(temp);
	    }else{
		    temp = strtok(NULL, ",");
	    }
    }

  gint errorPos=pos%width;
 gint newpos=pos>errorPos?(pos-errorPos):0;
g_printf("errorPos=%d,newpos=%d,pos=%d\n",errorPos,newpos,pos);
  widget->pageMaxPos=maxPos;
  widget->pagePos=newpos;
  widget->pageWidth=width;
  widget->pageHeight=height;
  widget->pageIndex=(newpos/width);
  widget->pageNum=(maxPos/width);


if(widget->isGoBack)
    {
     widget->pagePos = widget->pageMaxPos - widget->pageWidth;
    book_webview_page_pageJumpPos(widget,widget->pagePos);
    }

  gint jumpPos=widget->pageInitPos;
  g_printf("pageInitPos=%d\n",jumpPos);
  if(widget->isLoadUrl && jumpPos >0)
    {

      book_webview_page_pageJumpPos(widget,jumpPos);
      widget->pagePos=jumpPos;
      widget->pageInitPos=0;

    }
  widget->isGoBack=0;
g_printf("pagePosInfo[maxPos=%d,pos=%d,width=%d,pageIndex=%d,pageNum=%d,pageInitPos=%d]\n",maxPos,widget->pagePos,width,widget->pageIndex,widget->pageNum,widget->pageInitPos);
  g_free(pagePosInfoValue);
}
}


static void book_webview_sizeallocate_cb(GtkWidget *widget,
		      GdkRectangle *allocation,
		      gpointer user_data)
{
	BookWebview *gwidget = BOOK_WEBVIEW (widget);
	WebKitWebView *web_view = WEBKIT_WEB_VIEW(widget);
	int margin, font_size;
	float line_height;
	gchar *script, *font_family;
	gint themes;
gint splitPage=gwidget->splitPage;
  margin=20;
font_size=0;
  line_height=1.8f;


	script = g_strdup_printf(
		"if (!document.querySelector('#gepubwrap'))"
		"document.body.innerHTML = '<div id=\"gepubwrap\">' + document.body.innerHTML + '</div>';"
		"document.querySelector('#gepubwrap').style.marginLeft = '%dpx';"
		"document.querySelector('#gepubwrap').style.marginRight = '%dpx';"
	"document.querySelector('#gepubwrap').style.border = '0px solid black';"
		, margin, margin);
	webkit_web_view_run_javascript(web_view, script, NULL, NULL, NULL);
	g_free(script);


	if (font_size) {
		script = g_strdup_printf(
			"document.querySelector('#gepubwrap').style.fontSize = '%dpt';"
			, font_size);
		webkit_web_view_run_javascript(web_view, script, NULL, NULL, NULL);
		g_free(script);
	}



	if (line_height) {

		script = g_strdup_printf(
			"document.querySelector('#gepubwrap').style.lineHeight = %f;"
			, line_height);
		webkit_web_view_run_javascript(web_view, script, NULL, NULL, NULL);
		g_free(script);
	}

  if(splitPage==0)
    {
      //增加前进方法
      webkit_web_view_run_javascript(web_view,SHEET,NULL,NULL,NULL);
      webkit_web_view_run_javascript(web_view,ADDCSSRULE,NULL,NULL,NULL);
       webkit_web_view_run_javascript(web_view, gwidget->background,NULL,NULL,NULL);
      webkit_web_view_run_javascript(web_view,"addCSSRule('html', 'font-size:16px; margin-top:0px; margin-bottom:0px; padding-top:30px; padding-left:0px; padding-right:30px;  height:'+ (window.innerHeight-60) +'px; -webkit-column-gap:30px;    column-width:'+ window.innerWidth +'px;  column-rule:0px outset #ff0000;')",NULL,NULL,NULL);
      webkit_web_view_run_javascript(web_view,"addCSSRule('p', 'text-align: justify;')",NULL,NULL,NULL);
      webkit_web_view_run_javascript(web_view,"addCSSRule('img', 'width:auto; max-width:100%; max-height:100%; border:none;')",NULL,NULL,NULL);
      webkit_web_view_run_javascript(web_view,"addCSSRule('body', 'font-weight:normal; margin:0px; padding-top:0px; padding-left:30px; padding-right:30px')",NULL,NULL,NULL);
 webkit_web_view_run_javascript(web_view,"function movePow(y0,y1){var x=window._MYMOVETOTIME_X;var lon=y1-y0;if(x<=100){var pos= Math.pow(x/100,1.9);var apos=parseFloat(pos).toFixed(2);apos=apos*lon;window.scroll(apos+y0,0);x=(x+5);}window._MYMOVETOTIME_X=x;if(window._MYMOVETOTIME_X>=110){window.clearInterval(window.__MYMOVETOTIME);}}",NULL,NULL,NULL);
      webkit_web_view_run_javascript(web_view,"function pageScroll(y1){window.clearInterval(window.__MYMOVETOTIME);window._MYMOVETOTIME_X=10;var y0=document.body.scrollLeft;window.__MYMOVETOTIME=window.setInterval(\"movePow(\"+y0+\",\"+y1+\")\",10);}",NULL,NULL,NULL);
webkit_web_view_run_javascript(web_view,"function pageIndex(){return (Math.ceil(document.body.scrollLeft/window.innerWidth));}",NULL,NULL,NULL);
     webkit_web_view_run_javascript(web_view, " function pageJumpPos(xOffset){ window.scroll(xOffset,0); } ",NULL,NULL,NULL);
      webkit_web_view_run_javascript(web_view,"function pagePosInfo(){return ('maxpos,'+document.body.scrollWidth+',pos,'+document.body.scrollLeft+',width,'+window.innerWidth+',height,'+window.innerHeight);}",NULL,NULL,NULL);
        	script = g_strdup_printf(
		//			       "document.body.style.columnWidth = '1030px';"
 // "document.body.style.columnRule='1px outset #ff0000';"
		//			       "document.body.style.height = (window.innerHeight - %d) +'px';"
		//			       "document.body.style.columnGap = '%dpx';"
      "pagePosInfo()"
		      , 30, 0);
	      webkit_web_view_run_javascript(web_view, script, NULL, book_webview_pagination_initialize_finished, (gpointer)widget);

	      g_free(script);
    }






}


static void book_webview_docready_cb(WebKitWebView  *webview,
		 WebKitLoadEvent load_event,
		 gpointer user_data)
{
	BookWebview *widget = BOOK_WEBVIEW (webview);

	if (load_event == WEBKIT_LOAD_FINISHED) {
		book_webview_sizeallocate_cb(GTK_WIDGET(widget), NULL, NULL);
	}
//	printf("book_docready_cb:\n");
}

//=======================================
static void book_webview_finalize(GObject *object)
{
	BookWebview *widget = BOOK_WEBVIEW(object);
if(widget->bookDoc!=NULL)
    {
       //g_printf("[BookWebview]退出清理内存\n");
  	g_clear_object(&widget->bookDoc);
    }
	G_OBJECT_CLASS(book_webview_parent_class)->finalize(object);
}

//=====注册book
 static void book_webview_scheme_book(WebKitURISchemeRequest *request, BookWebview *webview)
{
GInputStream *stream=NULL;
	gchar *path=NULL;
  gchar *filePath=NULL;
   gchar *resPath=NULL;
	gchar *uri=NULL;
	gchar *mime = NULL;
 BookResource *bookRes=NULL;
GBytes *contents=NULL;
	GBytes *htmlContents=NULL;
	gchar **arr;
	gint i;


	uri = webkit_uri_scheme_request_get_uri(request);
 // g_print("request path=%s,uri=%s\n",webkit_uri_scheme_request_get_path(request),uri);


	//printf("准备读取地址path=%s,url=%s\n", path, uri);

  if(strcmp(BOOKSCHEMEURL,uri)==0)
    {
     // g_printf("current webview chapterIndex=%d\n",webview->chapterIndex);
       bookRes=book_doc_get_chapter (webview->bookDoc, webview->chapterIndex);
      webview->isLoadUrl=true;
    }
 else{
   	// removing "epub:///"
	//path = uri + 7;
  path=webkit_uri_scheme_request_get_path(request);
	//ljc 增加外理地址文件

	arr = g_strsplit(path, "#", -1);
	for (i = 0; arr[i] != NULL; i++) {
		// printf("切path[%d]=%s\n", i,arr[i]);
		if (i == 0) {
			filePath = g_strdup(arr[0]);
		}
	}
  g_free(arr);

   if(filePath[0]=='/')
     {
       resPath=g_strdup(filePath+1);
     }else{
       resPath=g_strdup(filePath);
     }

     // g_free(path);
  // g_free(filePath);

       g_printf("resource path=%s\n",resPath);
   bookRes=book_doc_get_resource (webview->bookDoc , resPath);
 }
  if(bookRes!=NULL)
    {
      mime=bookRes->mime;
      contents=bookRes->data;
      stream = g_memory_input_stream_new_from_bytes(contents);
	      webkit_uri_scheme_request_finish(request, stream, g_bytes_get_size(contents), mime);
      book_doc_resource_free(bookRes);
    }

}

static void
book_webview_init(BookWebview *widget)
{
widget->bookDoc=NULL;
  widget->chapterIndex=0;
  widget->chapterNum=0;
  widget->pageWidth=0;
  widget->pageHeight=0;
  widget->pagePos=0;
  widget->pageMaxPos=0;
  widget->pageNum=0;
  widget->pageIndex=0;
  widget->splitPage=0;
  widget->isGoBack=0;
  widget->pageInitPos=0;
  widget->isLoadUrl=false;
  widget->background=BOOK_BACKGROUND_0;
}

static void
book_webview_constructed(GObject *object)
{
	WebKitWebContext *webcontext;
	BookWebview *widget = BOOK_WEBVIEW(object);

	G_OBJECT_CLASS(book_webview_parent_class)->constructed(object);
	webcontext = webkit_web_view_get_context(WEBKIT_WEB_VIEW(widget));
 webkit_web_context_register_uri_scheme(webcontext, "book", book_webview_scheme_book, widget, NULL);
//	webkit_web_context_register_uri_scheme(ctx, "mobi", book_chapter_uri_callback, widget, NULL);
// 	webkit_web_context_register_uri_scheme(ctx, "book", book_webview_resource_callback, widget, NULL);
	g_signal_connect(widget, "load-changed", G_CALLBACK(book_webview_docready_cb), NULL);
	g_signal_connect(widget, "size-allocate", G_CALLBACK(book_webview_sizeallocate_cb), NULL);
}

static void
book_webview_class_init(BookWebviewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->constructed = book_webview_constructed;
	object_class->finalize = book_webview_finalize;
	//object_class->set_property = book_widget_set_property;
	//object_class->get_property = book_widget_get_property;

}

GtkWidget * book_webview_new(void)
{
	return g_object_new(BOOK_TYPE_WEBVIEW, 	    NULL);
}

void book_webview_set_doc(BookWebview *webview, BookDoc *doc)
{
  webview->bookDoc=doc;
  if(doc!=NULL)
    {
        webview->chapterNum= book_doc_get_chapter_num (doc);
    }
}

BookDoc * book_webview_get_doc(BookWebview *webview){

  return webview->bookDoc;
}
void book_webview_chapter(BookWebview *webview,gint index)
{
  if(!BOOK_IS_WEBVIEW (webview))
    {
      g_printf("不是bookwebview对象\n");
      return ;
    }
  if(webview->bookDoc!=NULL)
    {
        BookDoc * bookDoc= webview->bookDoc;

      if(index < webview->chapterNum && index>=0)
        {
            webview->chapterIndex=index;
         webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview),BOOKSCHEMEURL);
        }

    }
}

void book_webview_nav_go(BookWebview *webview,const gchar *path){
if(!BOOK_IS_WEBVIEW (webview) || path==NULL)
    {
      g_printf("不是bookwebview对象\n");
      return ;
    }
  g_printf("path=%s\n",path);
  gchar *url=g_strdup_printf("%s/%s",BOOKSCHEMEURL,path);
   BookDoc * bookDoc= webview->bookDoc;
  gint chapterIndex=book_doc_path_to_chapter_index(bookDoc,path);
  if(chapterIndex>=0)
    {
      webview->chapterIndex=chapterIndex;
    }
   webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview),url);

  // g_free(url);
 // g_free(path);
}

void book_webview_chapter_next(BookWebview *webview){
if(!BOOK_IS_WEBVIEW (webview))
    {
      g_printf("不是bookwebview对象\n");
      return ;
    }
  if(webview->bookDoc!=NULL)
    {
        BookDoc * bookDoc= webview->bookDoc;

      if(webview->chapterIndex < (webview->chapterNum-1))
        {
            webview->chapterIndex+=1;
          g_printf("chapterIndex=%d,chapterNum=%d\n",webview->chapterIndex,webview->chapterNum);
         webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview),BOOKSCHEMEURL);
        }

    }

}

void book_webview_chapter_prev(BookWebview *webview){
  if(!BOOK_IS_WEBVIEW (webview))
    {
      g_printf("不是bookwebview对象\n");
      return ;
    }
  if(webview->bookDoc!=NULL)
    {
        BookDoc * bookDoc= webview->bookDoc;
       if(webview->chapterIndex >0)
        {
      webview->chapterIndex-=1;
      webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview),BOOKSCHEMEURL);

        }

    }

}


void book_webview_page_next(BookWebview *webview){
   if(!BOOK_IS_WEBVIEW (webview))
    {
      g_printf("不是bookwebview对象\n");
      return ;
    }
  if(webview->bookDoc!=NULL)
    {
        BookDoc * bookDoc= webview->bookDoc;
      gint width= webview->pageWidth;
      gint pos=webview->pagePos;
      gint padding=webview->pageMaxPos - pos;
       if(webview->splitPage==0 && (padding> width*1.5 ) )
                  {
          gint nextPos=pos + width;
          webview->pagePos=nextPos;
          book_webview_page_scroll_to(webview,nextPos);
      }else{
        webview->isGoBack=0;
           book_webview_chapter_next(webview);
                  }

    }
}
void book_webview_page_prev(BookWebview *webview){

  if(!BOOK_IS_WEBVIEW (webview))
    {
      g_printf("不是bookwebview对象\n");
      return ;
    }
  if(webview->bookDoc!=NULL)
    {
        BookDoc * bookDoc= webview->bookDoc;
       if(webview->splitPage==0 && (webview->pagePos > 0))
                  {
          gint pos=webview->pagePos - webview->pageWidth;
          webview->pagePos=pos;
          book_webview_page_scroll_to(webview,pos);
        }else{
            if(webview->splitPage==0)
                                    {
               webview->isGoBack=1;
                                    }
           book_webview_chapter_prev(webview);
                  }

    }
}


void book_webview_set_html(BookWebview *widget, BookResource *bookRes)
{


	if (bookRes != NULL && bookRes->data != NULL) {
//printf("读取内容：");
//printf("长度:%d,mime:%s\n",g_bytes_get_size(bookData->data),bookData->mime);
		webkit_web_view_load_bytes(WEBKIT_WEB_VIEW(widget),
					   bookRes->data,
					   bookRes->mime,
					   "UTF-8", "book://");

	}

}

gchar * book_webview_get_info(BookWebview *webview){
gchar * info;
  info=g_strdup_printf("chapterIndex,%d,chapterNum,%d,pageWidth,%d,pageHeight,%d,pagePos,%d,splitPage,%d",webview->chapterIndex,webview->chapterNum,webview->pageWidth,webview->pageHeight,webview->pagePos,webview->splitPage);
  return info;
}


gint  book_webview_info_towebview(BookWebview *webview,const gchar * initInfo)
{
  gint chapterIndex=0;
  gint chapterNum=0;
  gint pageWidth=1000;
  gint pageHeight=700;
  gint pagePos=0;
  gint splitPage=0;

  //chapterIndex,3,chapterNum,62,pageWidth,1000,pagePos,1000,splitPage,0
if(initInfo!=NULL)
    {
      g_printf("book_webview_info_towebview:initInfo=%s完成1\n",initInfo);
      char *infoStr=g_strdup(initInfo);
    char *temp = strtok(infoStr, ",");
    while (temp) {
	    if (strcmp("chapterIndex", temp) == 0) {
		    temp = strtok(NULL, ",");
		    chapterIndex = atoi(temp);
	    }else if (strcmp("chapterNum", temp) == 0) {
		    temp = strtok(NULL, ",");
		    chapterNum = atoi(temp);
	    }else if (strcmp("pageWidth", temp) == 0) {
		    temp = strtok(NULL, ",");
		    pageWidth = atoi(temp);
    }else if (strcmp("pageHeight", temp) == 0) {
		    temp = strtok(NULL, ",");
		    pageHeight = atoi(temp);
    }else if (strcmp("pagePos", temp) == 0) {
		    temp = strtok(NULL, ",");
		    pagePos = atoi(temp);
    }else if (strcmp("splitPage", temp) == 0) {
		    temp = strtok(NULL, ",");
		    splitPage = atoi(temp);
	    }else{
		    temp = strtok(NULL, ",");
            }


      }


}
  webview->chapterIndex=chapterIndex;
      webview->pageWidth=pageWidth;
      webview->pageHeight=pageHeight;
      webview->pagePos=pagePos;
      webview->splitPage=splitPage;
      g_printf("initInfo[chapterIndex=%d,pageWidth=%d,pagePos=%d,splitPage=%d ]\n",webview->chapterIndex,webview->pageWidth,webview->pagePos,webview->splitPage);
      return pageWidth;
}


void book_webview_set_info(BookWebview *webview,const gchar *info){
if(!BOOK_IS_WEBVIEW (webview))
    {
      g_printf("不是bookwebview对象\n");
      return ;
    }
  if(webview->bookDoc!=NULL)
    {

      book_webview_info_towebview(webview,info);
      webview->pageInitPos=webview->pagePos;
      book_webview_chapter(webview,webview->chapterIndex);
    }

}


gint book_webview_get_pageWidth(BookWebview *webview)
{
  return webview->pageWidth;
}

gint book_webview_get_pageHeight(BookWebview *webview)
{
  return webview->pageHeight;
}
 
void book_webview_set_background(BookWebview *webview,gint backgroundStyle){

  switch (backgroundStyle)
    {
    case 0:
      webview->background=BOOK_BACKGROUND_0;
      break;
       case 1:
      webview->background=BOOK_BACKGROUND_1;
      break;
       case 2:
      webview->background=BOOK_BACKGROUND_2;
      break;
       case 3:
      webview->background=BOOK_BACKGROUND_3;
      break;
    }
  webkit_web_view_run_javascript(webview, webview->background,NULL,NULL,NULL);
}
