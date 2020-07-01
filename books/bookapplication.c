#include <stdlib.h>
#include <gtk/gtk.h>
#include "bookapplication.h"
#include "bookstorewin.h"
#include "booktreeview.h"
#include "bookdatabase.h"

#define BOOKAPPNAME "BOOK"

 GtkWidget   *booklist;
struct _BookApplication
{
  GtkApplication         application;
};


static void
change_theme_state (GSimpleAction *action,
                    GVariant      *state,
                    gpointer       user_data)
{
  GtkSettings *settings = gtk_settings_get_default ();

  g_object_set (G_OBJECT (settings),
                "gtk-application-prefer-dark-theme",
                g_variant_get_boolean (state),
                NULL);

  g_simple_action_set_state (action, state);
}


static void
open_folder_loading (gchar *dirpath)
{
  GDir *dir;
  const gchar *name;
 
  dir = g_dir_open (dirpath, 0, NULL);
  if (!dir)
    return;

gchar **arr, *lastDirName;
	gint i;

	arr = g_strsplit(dirpath, "/", -1);
	for (i = 0; arr[i] != NULL; i++) {
			lastDirName = arr[i];
	}

  name = g_dir_read_name (dir);
  while (name != NULL)
    {
      gchar *path, *display_name;
      gboolean is_dir;

      /* We ignore hidden files that start with a '.' */
      if (name[0] != '.')
        {
          path = g_build_filename (dirpath, name, NULL);

          is_dir = g_file_test (path, G_FILE_TEST_IS_DIR);

          display_name = g_filename_to_utf8 (name, -1, NULL, NULL, NULL);
if(!is_dir)
{
	if(strstr(display_name,".epub") || strstr(display_name,".mobi") || strstr(display_name,".azw3"))
 		book_dir_import (path,lastDirName,booklist);
}else {
	open_folder_loading(path);
}

          g_free (path);
          g_free (display_name);
        }

      name = g_dir_read_name (dir);
    }
g_free(lastDirName);
g_free(arr);
  g_dir_close (dir);
}

static void
open_folder_response_cb (GtkNativeDialog *dialog,
                  gint             response_id,
                  gpointer         user_data)
{
  GtkFileChooserNative *native = user_data;
  GApplication *app = g_object_get_data (G_OBJECT (native), "app");
  GtkWidget *message_dialog;
  GError *error = NULL;

  if (response_id == GTK_RESPONSE_ACCEPT)
    {
        char *filename,*dirPath;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
    filename = gtk_file_chooser_get_current_folder (chooser);
     // g_printf("打开的目录：%s\n",filename);
	open_folder_loading(filename);   
	printf("完成导入\n");
 
g_free (filename);
    }

  gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (native));
  g_object_unref (native);
}


static void
open_response_cb (GtkNativeDialog *dialog,
                  gint             response_id,
                  gpointer         user_data)
{
  GtkFileChooserNative *native = user_data;
  GApplication *app = g_object_get_data (G_OBJECT (native), "app");
  GtkWidget *message_dialog;
  GError *error = NULL;

  if (response_id == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
    filename = gtk_file_chooser_get_filename (chooser);
      g_printf("打开的文件：%s\n",filename);
     if( book_database_add_file (filename,booklist))
            {
               GtkWidget *epubwindow;
             epubwindow =book_open_epub (filename);
           if(epubwindow!=NULL)
                                  {
             gtk_widget_show_all(epubwindow);
                                 }
            }
    g_free (filename);

    }

  gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (native));
  g_object_unref (native);
}

static void
activate_open (GSimpleAction *action,
               GVariant      *parameter,
               gpointer       user_data)
{
  GApplication *app = user_data;
  GtkFileChooserNative *native;

  native = gtk_file_chooser_native_new ("Open File",
                                        NULL,
                                        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                        "_Open",
                                        "_Cancel");

  g_object_set_data_full (G_OBJECT (native), "app", g_object_ref (app), g_object_unref);
  g_signal_connect (native,
                    "response",
                    G_CALLBACK (open_folder_response_cb),
                    native);
 

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
}

static void
file_activate_open (GtkWidget *widget, GdkEvent *event)
{
  GtkFileChooserNative *native;

  native = gtk_file_chooser_native_new ("Open File",
                                        NULL,
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        "_Open",
                                        "_Cancel");

  g_signal_connect (native,
                    "response",
                    G_CALLBACK (open_response_cb),
                    native);

//后缀为.epub的文件
    GtkFileFilter* filter = gtk_file_filter_new();
gtk_file_filter_set_name (filter, ("All files"));
    gtk_file_filter_add_pattern(filter,"*.[Ee][Pp][Uu][Bb]");
 gtk_file_filter_add_pattern(filter,"*.[Mm][Oo][Bb][Ii]");
gtk_file_filter_add_pattern(filter,"*.[Aa][Zz][Ww][3]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter, ("epub"));
    gtk_file_filter_add_pattern(filter,"*.[Ee][Pp][Uu][Bb]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter,("mobi"));
    gtk_file_filter_add_pattern(filter,"*.[Mm][Oo][Bb][Ii]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter,("azw3"));
    gtk_file_filter_add_pattern(filter,"*.[Aa][Zz][Ww][3]");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (native),filter);




  gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
}

static void toolbarItem_clickAction(GtkWidget * btn,GtkWidget *btn2)
{
 gtk_widget_set_sensitive (btn, FALSE);
  gtk_widget_set_sensitive (btn2, TRUE);

}

static void toolbarItem_search_changed_cb (GtkSearchEntry *entry,GtkWidget *booklistTreeview)
{
  const char *text;
  GtkTreeModel *booklistmodel;
  text = gtk_entry_get_text (GTK_ENTRY (entry));

  booklistmodel = gtk_tree_view_get_model (booklistTreeview);
  gtk_tree_store_clear(booklistmodel);
  book_menu_list_query_title(text,booklistTreeview);
        g_message ("search changed: %s", text);
  //g_free(text);

}


static GtkWidget *
create_window_menu (gint depth)
{
  GtkWidget *menu,*menuitem;
  GtkRadioMenuItem *last_item;
  char buf[32];
  int i, j;

  if (depth < 1)
    return NULL;

  menu = gtk_menu_new ();
  last_item = NULL;

 menuitem=gtk_menu_item_new_with_label("打开");
 gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
 g_signal_connect(GTK_MENU_ITEM(menuitem),"activate",G_CALLBACK(file_activate_open),NULL);

  return menu;
}


static void
new_window (GApplication *app,
            GFile        *file)
{
  GtkWidget *window,*menuscrolled, *scrolled, *view, *overlay;
  GtkWidget *header;
  GtkWidget *box;
  GtkWidget *hpaned;
 GtkWidget *menubar;
GtkWidget *menu;
GtkWidget *menuitem;
  GtkWidget *toolbarItem_local,*toolbarItem_net;
   GIcon *icon;
  GtkWidget *header_title_box;
GIcon *winIcon;
   GtkWidget *searchEntry;

  window = gtk_application_window_new (GTK_APPLICATION (app));
  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (window), FALSE);

  gtk_window_set_default_size ((GtkWindow*)window, 1000, 600);
  gtk_window_set_title (GTK_WINDOW (window), BOOKAPPNAME);
  winIcon = g_themed_icon_new ("view-dual-symbolic");
    gtk_window_set_icon(GTK_WINDOW(window), winIcon);
  gtk_window_set_icon_name (GTK_WINDOW (window), "view-dual-symbolic");

  header = gtk_header_bar_new ();
  gtk_widget_show (header);
  //gtk_header_bar_set_title (GTK_HEADER_BAR (header), BOOKAPPNAME);
  gtk_window_set_titlebar (GTK_WINDOW (window), header);
	gtk_header_bar_set_show_close_button(header, TRUE);

 //  工具栏中间部分
  header_title_box= gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  	gtk_style_context_add_class(gtk_widget_get_style_context(header_title_box), "linked");
   // gtk_button_box_set_layout (GTK_BUTTON_BOX (header_title_box), GTK_BUTTONBOX_EXPAND);

   	//本地按扭
  toolbarItem_local =gtk_button_new_with_label("本地");
 //网络按键
  toolbarItem_net=gtk_button_new_with_label("书库");

 gtk_container_add (GTK_CONTAINER (header_title_box), toolbarItem_local);
  gtk_container_add (GTK_CONTAINER (header_title_box), toolbarItem_net);
  gtk_header_bar_set_custom_title(header,header_title_box);
 g_signal_connect (toolbarItem_local, "clicked",  G_CALLBACK (toolbarItem_clickAction), toolbarItem_net);
  g_signal_connect (toolbarItem_net, "clicked",  G_CALLBACK (toolbarItem_clickAction), toolbarItem_local);
 gtk_widget_set_sensitive (toolbarItem_local, FALSE);

  searchEntry= gtk_search_entry_new ();
  gtk_header_bar_pack_end(GTK_HEADER_BAR(header), searchEntry);


 box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
      gtk_container_add (GTK_CONTAINER (window), box);
      gtk_widget_show (box);
/*
 menubar = gtk_menu_bar_new ();
      gtk_widget_set_hexpand (menubar, TRUE);
      gtk_box_pack_start (GTK_BOX (box), menubar, FALSE, TRUE, 0);
      gtk_widget_show (menubar);
 menu = create_window_menu (1);

  menuitem = gtk_menu_item_new_with_label ("文件");
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);
      gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);
      gtk_widget_show (menuitem);
*/

 hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
      gtk_box_pack_start (GTK_BOX (box), hpaned, FALSE, TRUE, 0);
      gtk_container_set_border_width (GTK_CONTAINER(hpaned), 2);

 

 menuscrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (menuscrolled, TRUE);
gtk_widget_set_vexpand (menuscrolled, TRUE);


 gtk_widget_set_size_request (menuscrolled, 300, 60);
 gtk_paned_add1 (GTK_PANED (hpaned), menuscrolled);

 overlay = gtk_overlay_new ();
//  gtk_container_add (GTK_CONTAINER (window), overlay);
gtk_paned_add2 (GTK_PANED (hpaned), overlay);

  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_hexpand (scrolled, TRUE);
gtk_widget_set_vexpand (scrolled, TRUE);
 // view = gtk_text_view_new ();

  GtkWidget *treeview;
      treeview =book_get_book_treeview (window);
GtkWidget *menutreeview;
      menutreeview =book_get_mymenu_treeview (treeview);
  gtk_container_add (GTK_CONTAINER (menuscrolled), menutreeview);
  gtk_container_add (GTK_CONTAINER (scrolled), treeview);
  //gtk_container_add (GTK_CONTAINER (overlay), box);
  gtk_container_add (GTK_CONTAINER (overlay), scrolled);
  //gtk_container_add (GTK_CONTAINER (window), treeview);

  //增加搜索框的查询事件
  g_signal_connect (searchEntry, "search-changed",
                        G_CALLBACK (toolbarItem_search_changed_cb), treeview);


  if (file != NULL)
    {
      gchar *contents;
      gsize length;

      if (g_file_load_contents (file, NULL, &contents, &length, NULL, NULL))
        {
          GtkTextBuffer *buffer;

          buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
          gtk_text_buffer_set_text (buffer, contents, length);
          g_free (contents);
        }
    }

  gtk_widget_show_all (GTK_WIDGET (window));

  booklist=treeview;
}

static void
activate (GApplication *application)
{
  new_window (application, NULL);
}

static void
open (GApplication  *application,
      GFile        **files,
      gint           n_files,
      const gchar   *hint)
{
  gint i;

  for (i = 0; i < n_files; i++)
    new_window (application, files[i]);
}

//typedef GtkApplication BookApplication;
// typedef GtkApplicationClass BookApplicationClass;

//G_DEFINE_TYPE (BookApplication, book_application, GTK_TYPE_APPLICATION)
G_DEFINE_TYPE (BookApplication, book_application, GTK_TYPE_APPLICATION)

static void
show_about (GSimpleAction *action,
            GVariant      *parameter,
            gpointer       user_data)
{

  GtkWidget *window = user_data;

  const gchar *authors[] = {
    "LJincheng <ljincheng@126.com>",
    "Daniel Garcia <danigm@wadobo.com>",
    NULL
  };


    gtk_show_about_dialog (GTK_WINDOW (window),
                         "program-name", "Book",
                         "version", "1.0",
                         "website", "http://www.booktable.cn",
                         "authors", authors,
                         "logo-icon-name", "gtk4-demo",
                         "title", "Book",
                         NULL);
}


static void
quit_app (GSimpleAction *action,
          GVariant      *parameter,
          gpointer       user_data)
{
  GList *list, *next;
  GtkWindow *win;

  g_print ("Going down...\n");

  list = gtk_application_get_windows (GTK_APPLICATION (g_application_get_default ()));
  while (list)
    {
      win = list->data;
      next = list->next;

      gtk_widget_destroy (GTK_WIDGET (win));

      list = next;
    }
}

static void
new_activated (GSimpleAction *action,
               GVariant      *parameter,
               gpointer       user_data)
{
  GApplication *app = user_data;

  g_application_activate (app);
}

static void
activate_toggle (GSimpleAction *action,
                 GVariant      *parameter,
                 gpointer       user_data)
{
  GVariant *state;

  state = g_action_get_state (G_ACTION (action));
  g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (state)));
  g_variant_unref (state);
}

static GActionEntry app_entries[] = {
  { "about", show_about, NULL, NULL, NULL },
  { "quit", quit_app, NULL, NULL, NULL },
  { "dark", activate_toggle, NULL, "false", change_theme_state },
  { "open", activate_open, NULL, NULL, NULL },
  { "new", new_activated, NULL, NULL, NULL }
};



static void
startup (GApplication *application)
{
  GtkBuilder *builder;

  G_APPLICATION_CLASS (book_application_parent_class)->startup (application);

  g_action_map_add_action_entries (G_ACTION_MAP (application), app_entries, G_N_ELEMENTS (app_entries), application);

  if (g_getenv ("APP_MENU_FALLBACK"))
    g_object_set (gtk_settings_get_default (), "gtk-shell-shows-app-menu", FALSE, NULL);

  builder = gtk_builder_new ();
  gtk_builder_add_from_string (builder,
                               "<interface>"
                               "  <menu id='app-menu'>"
                               "    <section>"
                               "      <item>"
                               "        <attribute name='label' translatable='yes'>_Dark</attribute>"
                               "        <attribute name='action'>app.dark</attribute>"
                               "      </item>"
                                "      <item>"
                               "        <attribute name='label' translatable='yes'>_Import</attribute>"
                               "        <attribute name='action'>app.open</attribute>"
                               "      </item>"
                               "      <item>"
                               "        <attribute name='label' translatable='yes'>_About Book</attribute>"
                               "        <attribute name='action'>app.about</attribute>"
                               "      </item>"
                               "      <item>"
                               "        <attribute name='label' translatable='yes'>_Quit</attribute>"
                               "        <attribute name='action'>app.quit</attribute>"
                               "        <attribute name='accel'>&lt;Primary&gt;q</attribute>"
                               "      </item>"
                               "    </section>"
                               "  </menu>"
                               "</interface>", -1, NULL);
  gtk_application_set_app_menu (GTK_APPLICATION (application), G_MENU_MODEL (gtk_builder_get_object (builder, "app-menu")));
  g_object_unref (builder);
}

static void
book_application_init (BookApplication *app)
{
}

static void
book_application_class_init (BookApplicationClass *class)
{
  GApplicationClass *application_class = G_APPLICATION_CLASS (class);

  application_class->startup = startup;
  application_class->activate = activate;
  application_class->open = open;
}


BookApplication *
book_application_new (void)
{
  return g_object_new (BOOK_TYPE_APPLICATION,
                       "application-id", "com.bebe.BookApplication",
                       "flags", G_APPLICATION_HANDLES_OPEN,
                       "resource-base-path", "/bebebookapp",
                       NULL);
}

/*
int
main (int argc, char **argv)
{
  BookApplication *menu_button;
  int status;

  menu_button = book_application_new ();
  status = g_application_run (G_APPLICATION (menu_button), argc, argv);
  g_object_unref (menu_button);

  return status;
}
 */
