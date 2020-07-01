
#ifndef __BOOK_DOWNLOAD_H__
#define __BOOK_DOWNLOAD_H__

#pragma once

#include <glib-object.h>
#include <webkit2/webkit2.h>

G_BEGIN_DECLS

#define BOOK_TYPE_DOWNLOAD (book_download_get_type ())

G_DECLARE_FINAL_TYPE (BookDownload, book_download, BOOK, DOWNLOAD, GObject)

typedef enum
{
  BOOK_DOWNLOAD_ACTION_NONE,
  BOOK_DOWNLOAD_ACTION_BROWSE_TO,
  BOOK_DOWNLOAD_ACTION_OPEN
} BookDownloadActionType;

BookDownload *book_download_new                   (WebKitDownload *download);


G_END_DECLS

#endif
