#ifndef __BOOK_APPLICATION_IMPORT_H
#define __BOOK_APPLICATION_IMPORT_H

#include <gtk/gtk.h>

extern GtkWidget *APPAPINNER;

void book_application_import_dir_open (GSimpleAction *action, GVariant *parameter,gpointer user_data);
void book_application_import_file_open (GtkWidget *widget, GdkEvent *event);



#endif /* __BOOK_APPLICATION_IMPORT_H */
