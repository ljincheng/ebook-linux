#include "book-meta.h"
#include <stdlib.h>
#include <string.h>


BookBuffer * book_buffer_init(const size_t len)
{
  unsigned char *data = malloc(len);
    if (data == NULL) {
        printf("%s", "Buffer data allocation failed\n");
        return NULL;
    }
    BookBuffer *buf = book_buffer_init_null(data, len);
    if (buf == NULL) {
        free(data);
    }
    return buf;
}
BookBuffer * book_buffer_init_null(unsigned char *data, const size_t len)
{
 BookBuffer *buf = malloc(sizeof(BookBuffer));
	if (buf == NULL) {
        printf("%s", "Buffer allocation failed\n");
        return NULL;
    }
    buf->data = data;
	buf->offset = 0;
	buf->maxlen = len;
	return buf;
}
void book_buffer_resize(BookBuffer *buf, const size_t newlen)
{
 unsigned char *tmp = realloc(buf->data, newlen);
    if (tmp == NULL) {
        printf("%s", "Buffer allocation failed\n");
        return;
    }
    buf->data = tmp;
    buf->maxlen = newlen;
    if (buf->offset >= newlen) {
        buf->offset = newlen - 1;
    }
    printf("Buffer successfully resized to %zu\n", newlen);
}
void book_buffer_add8(BookBuffer *buf, const uint8_t data)
{
 if (buf->offset + 1 > buf->maxlen) {
        printf("%s", "Buffer full\n");
        return;
    }
    buf->data[buf->offset++] = data;
}
void book_buffer_add16(BookBuffer *buf, const uint16_t data)
{
 if (buf->offset + 2 > buf->maxlen) {
        printf("%s", "Buffer full\n");
        return;
    }
    unsigned char *buftr = buf->data + buf->offset;
    *buftr++ = (uint8_t)((uint32_t)(data & 0xff00U) >> 8);
    *buftr = (uint8_t)((uint32_t)(data & 0xffU));
    buf->offset += 2;
}
void book_buffer_add32(BookBuffer *buf, const uint32_t data)
{
 if (buf->offset + 4 > buf->maxlen) {
        printf("%s", "Buffer full\n");
        return;
    }
    unsigned char *buftr = buf->data + buf->offset;
    *buftr++ = (uint8_t)((uint32_t)(data & 0xff000000U) >> 24);
    *buftr++ = (uint8_t)((uint32_t)(data & 0xff0000U) >> 16);
    *buftr++ = (uint8_t)((uint32_t)(data & 0xff00U) >> 8);
    *buftr = (uint8_t)((uint32_t)(data & 0xffU));
    buf->offset += 4;
}
void book_buffer_addraw(BookBuffer *buf, const unsigned char* data, const size_t len)
{
 if (buf->offset + len > buf->maxlen) {
        printf("%s", "Buffer full\n");
        return;
    }
    memcpy(buf->data + buf->offset, data, len);
    buf->offset += len;
}
void book_buffer_addstring(BookBuffer *buf, const char *str)
{
  const size_t len = strlen(str);
    book_buffer_addraw(buf, (const unsigned char *) str, len);
}
void book_buffer_addzeros(BookBuffer *buf, const size_t count)
{
 if (buf->offset + count > buf->maxlen) {
        printf("%s", "Buffer full\n");
        return;
    }
    memset(buf->data + buf->offset, 0, count);
    buf->offset += count;
}
uint8_t book_buffer_get8(BookBuffer *buf)
{
if (buf->offset + 1 > buf->maxlen) {
        printf("%s", "End of buffer\n");
        return 0;
    }
    return buf->data[buf->offset++];
}
uint16_t book_buffer_get16(BookBuffer *buf)
{
if (buf->offset + 2 > buf->maxlen) {
        printf("%s", "End of buffer\n");
        return 0;
    }
    uint16_t val;
    val = (uint16_t)((uint16_t) buf->data[buf->offset] << 8 | (uint16_t) buf->data[buf->offset + 1]);
    buf->offset += 2;
    return val;
}
uint32_t book_buffer_get32(BookBuffer *buf)
{
 if (buf->offset + 4 > buf->maxlen) {
        printf("%s", "End of buffer\n");
        return 0;
    }
    uint32_t val;
    val = (uint32_t) buf->data[buf->offset] << 24 | (uint32_t) buf->data[buf->offset + 1] << 16 | (uint32_t) buf->data[buf->offset + 2] << 8 | (uint32_t) buf->data[buf->offset + 3];
    buf->offset += 4;
    return val;
}

static uint32_t _book_buffer_get_varlen(BookBuffer *buf, size_t *len, const int direction) {
    uint32_t val = 0;
    uint8_t byte_count = 0;
    uint8_t byte;
    const uint8_t stop_flag = 0x80;
    const uint8_t mask = 0x7f;
    uint32_t shift = 0;
    do {
        if (direction == 1) {
            if (buf->offset + 1 > buf->maxlen) {
                printf("%s", "End of buffer\n");
                return val;
            }
            byte = buf->data[buf->offset++];
            val <<= 7;
            val |= (byte & mask);
        } else {
            if (buf->offset < 1) {
                printf("%s", "End of buffer\n");
                return val;
            }
            byte = buf->data[buf->offset--];
            val = val | (uint32_t)(byte & mask) << shift;
            shift += 7;
        }        
        (*len)++;
        byte_count++;
    } while (!(byte & stop_flag) && (byte_count < 4));
    return val;
}


uint32_t book_buffer_get_varlen(BookBuffer *buf, size_t *len)
{
	return _book_buffer_get_varlen(buf, len, 1);
}
uint32_t book_buffer_get_varlen_dec(BookBuffer *buf, size_t *len)
{
 return _book_buffer_get_varlen(buf, len, -1);
}

void book_buffer_dup8(uint8_t **val, BookBuffer *buf)
{
 *val = NULL;
    if (buf->offset + 1 > buf->maxlen) {
        return;
    }
    *val = malloc(sizeof(uint8_t));
    if (*val == NULL) {
        return;
    }
    **val = book_buffer_get8(buf);
}
void book_buffer_dup16(uint16_t **val, BookBuffer *buf)
{
 *val = NULL;
    if (buf->offset + 2 > buf->maxlen) {
        return;
    }
    *val = malloc(sizeof(uint16_t));
    if (*val == NULL) {
        return;
    }
    **val = book_buffer_get16(buf);
}

void book_buffer_dup32(uint32_t **val, BookBuffer *buf)
{

 *val = NULL;
    if (buf->offset + 4 > buf->maxlen) {
        return;
    }
    *val = malloc(sizeof(uint32_t));
    if (*val == NULL) {
        return;
    }
    **val = book_buffer_get32(buf);
}

void book_buffer_getstring(char *str, BookBuffer *buf, const size_t len)
{
   if (!str) {
        return;
    }
    if (buf->offset + len > buf->maxlen) {
        printf("%s", "End of buffer\n");
        str[0] = '\0';
        return;
    }
    memcpy(str, buf->data + buf->offset, len);
    str[len] = '\0';
    buf->offset += len;
}

void book_buffer_appendstring(char *str, BookBuffer *buf, const size_t len)
{
 if (!str) {
        return;
    }
    if (buf->offset + len > buf->maxlen) {
        printf("%s", "End of buffer\n");
        return;
    }
    size_t str_len = strlen(str);
    memcpy(str + str_len, buf->data + buf->offset, len);
    str[str_len + len] = '\0';
    buf->offset += len;
}

void book_buffer_getraw(void *data, BookBuffer *buf, const size_t len)
{
 if (!data) {
        return;
    }
    if (buf->offset + len > buf->maxlen) {
        printf("%s", "End of buffer\n");
        return;
    }
    memcpy(data, buf->data + buf->offset, len);
    buf->offset += len;
}

 



void book_buffer_free(BookBuffer *buf)
{
if (buf == NULL) { return; }
	if (buf->data != NULL) {
		free(buf->data);
	}
	free(buf);
}

void book_buffer_free_null(BookBuffer *buf)
{
if (buf == NULL) { return; }
	free(buf);
}

BookMeta * book_meta_init(unsigned char *data)
{
BookMeta *bookMeta = malloc(sizeof(BookMeta));
	if (bookMeta == NULL) {
        	printf("%s\n", "BookMeta allocation failed\n");
        	return NULL;
	}
	bookMeta->data = NULL;
	bookMeta->type = 0;
	bookMeta->len = 0;
	if(data!=NULL)
	{
		bookMeta->len=strlen(data);
		if(bookMeta->len>0)
		{
		 unsigned char *metadata = malloc(bookMeta->len);
			memcpy(metadata, data, bookMeta->len);
		bookMeta->data=metadata;
		}	
	}
	return bookMeta;


 
}

BookMeta * book_meta_append(BookMeta *bookMeta,unsigned char *data, const uint32_t len)
{

if(bookMeta->len==0)
{
	BookBuffer *buf=book_buffer_init(5);
    unsigned char *data = malloc(len);
    if (data == NULL) {
        printf("%s\n", "BookMeta data allocation failed\n");
        return NULL;
    }
}

	//BookMeta *bookMeta = malloc(sizeof(BookMeta));
	if (bookMeta == NULL) {
        	printf("%s\n", "BookMeta allocation failed\n");
        	return NULL;
	}
	bookMeta->data = data;
	bookMeta->type = 0;
	bookMeta->len = len;
	return bookMeta;
}

void book_meta_free(BookMeta *bookMeta)
{
	if (bookMeta == NULL) { return; }
	if (bookMeta->data != NULL) {
		free(bookMeta->data);
	}
	free(bookMeta);
}
void book_meta_free_null(BookMeta *bookMeta)
{
if (bookMeta == NULL) { return; }
	free(bookMeta);

}
 
