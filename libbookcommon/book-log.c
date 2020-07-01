#include "book-log.h"
#include "config.h"
#ifndef BOOK_DEBUG
#define BOOK_DEBUG FALSE
#endif

void book_log(const gchar *format, ...){
   va_list args;
  va_start (args, format);
   g_vprintf (format, args);
  va_end (args);
}
void book_log_info(const gchar *format, ...){
   va_list args;
  va_start (args, format);
   g_vprintf (format, args);
  va_end (args);
}
void book_log_debug(const gchar *format, ...){
  if(BOOK_DEBUG)
    {
   va_list args;
  va_start (args, format);
   g_vprintf (format, args);
  va_end (args);
      }
}
void book_log_error(const gchar *format, ...){
   va_list args;
  va_start (args, format);
   g_vprintf (format, args);
  va_end (args);
}
void book_log_warn(const gchar *format, ...){
   va_list args;
  va_start (args, format);
   g_vprintf (format, args);
  va_end (args);
}

void book_log_gerror(gint logLevel,gchar *format,GError *error){
  gchar *msg;
  if(error!=NULL)
    {
      msg=error->message;
    }
switch (logLevel)
  {
  case 1:
    book_log_error(format,msg);
    break;
  case 2:
    book_log_warn(format,msg);
    break;
     case 3:
    book_log_info(format,msg);
    break;
     case 4:
    book_log_debug(format,msg);
    break;
  default:
    book_log(format,msg);
  }

 }
