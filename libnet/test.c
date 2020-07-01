
#include "book-meta.h"

int main(int argc, char *argv[])
{

BookBuffer *buf=book_buffer_init(1024);
book_buffer_add8(buf,1);
char * hellostr="hello! my name is linjincheng !sdddddlllllllllllllllllllllllllllllllllll111111111111111111111111111222000000000000000000000000000000000000000000000000000000000002320000000000000000000w你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好ＯＫ";
book_buffer_add32(buf,strlen(hellostr));
book_buffer_addstring(buf,hellostr);


buf->offset=0;
uint8_t type=book_buffer_get8(buf);
uint32_t helloLen=book_buffer_get32(buf);
char *msg=malloc(helloLen + 1);
book_buffer_getstring(msg,buf,helloLen);
printf("buf:type=%d,len=%d,data=%s\n",type,helloLen,msg);
book_buffer_free(buf);
free(msg);

return 0;
}
