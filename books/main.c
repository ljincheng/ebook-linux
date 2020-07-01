
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include "bookwindow.h"



int main (int argc, char **argv)
{
    if (argc < 2) {
        printf ("you should provide an .epub file\n");
        return 1;
    }

  gtk_init (&argc, &argv);

g_type_class_unref (g_type_class_ref (GTK_TYPE_IMAGE_MENU_ITEM));
g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", TRUE, NULL);

  GtkWidget *window;
    window =book_open_epub (argv[1]);
    gtk_widget_show_all (window);
    gtk_main ();

   return 0;
}
 
