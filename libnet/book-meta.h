  

#ifndef book_meta_h
#define book_meta_h
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>


typedef struct _BookMeta  BookMeta;
typedef struct _BookMeta{
        uint16_t type;
	uint32_t len;
        unsigned char * data;
};

typedef struct {
    size_t offset;  
    size_t maxlen;  
    unsigned char *data; 
} BookBuffer;


BookBuffer * book_buffer_init(const size_t len);
BookBuffer * book_buffer_init_null(unsigned char *data, const size_t len);
void book_buffer_resize(BookBuffer *buf, const size_t newlen);
void book_buffer_add8(BookBuffer *buf, const uint8_t data);
void book_buffer_add16(BookBuffer *buf, const uint16_t data);
void book_buffer_add32(BookBuffer *buf, const uint32_t data);
void book_buffer_addraw(BookBuffer *buf, const unsigned char* data, const size_t len);
void book_buffer_addstring(BookBuffer *buf, const char *str);
void book_buffer_addzeros(BookBuffer *buf, const size_t count);
uint8_t book_buffer_get8(BookBuffer *buf);
uint16_t book_buffer_get16(BookBuffer *buf);
uint32_t book_buffer_get32(BookBuffer *buf);
uint32_t book_buffer_get_varlen(BookBuffer *buf, size_t *len);
uint32_t book_buffer_get_varlen_dec(BookBuffer *buf, size_t *len);
void book_buffer_dup8(uint8_t **val, BookBuffer *buf);
void book_buffer_dup16(uint16_t **val, BookBuffer *buf);
void book_buffer_dup32(uint32_t **val, BookBuffer *buf);
void book_buffer_getstring(char *str, BookBuffer *buf, const size_t len);
void book_buffer_appendstring(char *str, BookBuffer *buf, const size_t len);
void book_buffer_getraw(void *data, BookBuffer *buf, const size_t len);
void book_buffer_free(BookBuffer *buf);
void book_buffer_free_null(BookBuffer *buf);

BookMeta * book_meta_init(unsigned char *data);
BookMeta * book_meta_append(BookMeta *bookMeta,unsigned char *data, const uint32_t len);

void book_meta_free(BookMeta *bookMeta);
void book_meta_free_null(BookMeta *bookMeta);

#endif
