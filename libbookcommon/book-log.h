
#ifndef __BOOK_LOG_H__
#define __BOOK_LOG_H__



#include <glib.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * 普通日志
 */
void book_log(const gchar *format, ...)G_GNUC_PRINTF (1, 2);
/**
 * 信息日志
 */
void book_log_info(const gchar *format, ...)G_GNUC_PRINTF (1, 2);
/**
 * 测试日志
 */
void book_log_debug(const gchar *format, ...)G_GNUC_PRINTF (1, 2);
/**
 * 测试日志
 */
void book_log_error(const gchar *format, ...)G_GNUC_PRINTF (1, 2);
/**
 * 警告日志
 */
void book_log_warn(const gchar *format, ...)G_GNUC_PRINTF (1, 2);

void book_log_gerror(gint logLevel,gchar *format,GError *error);

#endif
