
#ifndef __BOOKDATABASE_H__
#define __BOOKDATABASE_H__

#include <gtk/gtk.h>

enum
{
  BOOK_NAME_COLUMN = 0,
  ID,
  SRC,
  SORT,
  AUTHOR,
  CHAPTER,
  EXT,
  NUM_COLUMNS
};

enum
{
  MENUTITLE = 0,
  MENUID,
  MENU_COLUMNS
};

gboolean book_database_add_file (const gchar *epubfile,GtkWidget *treeview);
gboolean book_menu_list_menuid(const gchar *menuid,GtkWidget *treeview);
gboolean book_menu_list_query_title(const gchar *title,GtkWidget *treeview);
gboolean book_dir_import(const gchar *bookpath,const gchar *menutitle,GtkWidget *treeview);

GtkTreeModel *create_book_menu_all_model (void);
GtkTreeModel *create_book_menu_model (void);

int book_get_history_chapter(const gchar *bookid);

gboolean book_set_history_chapter(gchar * bookid,gint chapter);

#endif
