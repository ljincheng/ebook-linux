#include "bookapplication.h"
#include <gtk/gtk.h>
#include "bookstorewin.h"

int
main (int argc, char *argv[])
{

/*
  GtkApplication *app;

  app = GTK_APPLICATION (g_object_new (book_application_get_type (),
                                       "application-id", "com.bebewhere.book",
                                       "flags", G_APPLICATION_HANDLES_OPEN,
                                       NULL));
  return g_application_run (G_APPLICATION (app), 0, NULL);

*/
/*
gtk_init (&argc, &argv);
//g_type_class_unref (g_type_class_ref (GTK_TYPE_IMAGE_MENU_ITEM));
//g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", TRUE, NULL);
//  return g_application_run (G_APPLICATION (book_application_new ()), argc, argv);

   GtkWidget *window;
  window= do_iconview(NULL);
    gtk_widget_show_all (window);
    gtk_main ();
   return  0;
*/

  BookApplication *book_application;
  int status;

  book_application = book_application_new ();
  status = g_application_run (G_APPLICATION (book_application), argc, argv);
  g_object_unref (book_application);

  return status;

}

