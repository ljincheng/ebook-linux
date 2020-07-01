
#include <stdlib.h>
#include "book-application.h"
#include "book-app-window.h"
#include "book-application-import.h"
#include "book-manage.h"
#include <libnotify/notify.h>

#define BOOK_APPNAME "BOOK"

 GtkWidget   *booklist;
struct _BookApplication
{
  GtkApplication         application;
};

/**
 * 切换界面主题
 */
static void change_theme_state (GSimpleAction *action,
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



/**
 *
 * 创建App窗口
 */
static GtkWidget * create_window_menu (gint depth)
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
 //g_signal_connect(GTK_MENU_ITEM(menuitem),"activate",G_CALLBACK(file_activate_open),NULL);

  return menu;
}

/**
 * 新窗口
 */
static void new_window (GApplication *app,
            GFile        *file)
{
  GtkWidget *window;
  char *filepath;

  // 新建window

  //window=book_application_window_init(app);
if(file)
    {
      filepath=g_file_get_uri(file);
      g_printf("filepath=%s\n",filepath);
    }
window=book_app_window_new(app);
  gtk_widget_show_all (GTK_WIDGET (window));

book_manage_check_database();
}

/**
 * APP启动事件
 */
static void activate (GApplication *application)
{
  new_window (application, NULL);
}

/**
 * 打开新窗口
 */
static void open (GApplication  *application,
      GFile        **files,
      gint           n_files,
      const gchar   *hint)
{
  gint i;

  for (i = 0; i < n_files; i++)
    new_window (application, files[i]);
}

G_DEFINE_TYPE (BookApplication, book_application, GTK_TYPE_APPLICATION)

/**
 * 打开“关于”窗口
 */
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
                         "program-name", BOOK_APPNAME,
                         "version", "1.1",
                         "website", "http://www.booktable.cn",
                         "authors", authors,
                         "logo-icon-name", "view-dual-symbolic",
                         "title", BOOK_APPNAME,
                         NULL);
}

/**
 * 退出APP事件
 */
static void quit_app (GSimpleAction *action,
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



/**
 * 重复活动事件
 */
static void activate_toggle (GSimpleAction *action,
                 GVariant      *parameter,
                 gpointer       user_data)
{
  GVariant *state;

  state = g_action_get_state (G_ACTION (action));
  g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (state)));
  g_variant_unref (state);
}

/**
 * APP目录
 */
static GActionEntry app_entries[] = {
  { "about", show_about, NULL, NULL, NULL },
  { "quit", quit_app, NULL, NULL, NULL },
  { "dark", activate_toggle, NULL, "false", change_theme_state },
  { "import", activate_toggle, NULL, "false", book_application_import_dir_open },
  { "open", activate_toggle, NULL, "false", book_application_import_file_open }
 // { "open", activate_open, NULL, NULL, NULL },
 // { "new", new_activated, NULL, NULL, NULL }
};


/**
 * app 启动
 */
static void startup (GApplication *application)
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
                                "        <attribute name='accel'>&lt;Primary&gt;t</attribute>"
                               "      </item>"
                                "      <item>"
                               "        <attribute name='label' translatable='yes'>_Import</attribute>"
                               "        <attribute name='action'>app.import</attribute>"
                                "        <attribute name='accel'>&lt;Primary&gt;i</attribute>"
                               "      </item>"
                               "      <item>"
                               "        <attribute name='label' translatable='yes'>_Open</attribute>"
                               "        <attribute name='action'>app.open</attribute>"
                                "        <attribute name='accel'>&lt;Primary&gt;o</attribute>"
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

/**
 * APP初始化
 */
static void book_application_init (BookApplication *app)
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

/**
 * new app
 */
BookApplication * book_application_new (void)
{

  BookApplication *app;
  app=g_object_new (BOOK_TYPE_APPLICATION,
                       "application-id", "com.bebe.BookApp",
                       "flags", G_APPLICATION_HANDLES_OPEN,
                       "resource-base-path", "/bebebookapp",
                       NULL);

  return app;
}

int
main (int argc, char *argv[])
{

  BookApplication *book_application;
  int status;
g_set_prgname ("ebook");
 g_set_application_name ("ebook");

  book_application = book_application_new ();

  notify_init ("ebook");
  if(!g_thread_supported()) g_thread_init(NULL);
    gdk_threads_init();
//gdk_threads_enter();
  status = g_application_run (G_APPLICATION (book_application), argc, argv);
  g_object_unref (book_application);
 //gdk_threads_leave();
  return status;

}

