# These should be in the order you want them to appear in the
# demo app, which means alphabetized by demo title, not filename
booksApp='book'
glib_dep = dependency('glib-2.0', version: '>= 2.43.4')
gtk_dep = dependency('gtk+-3.0', version: '>= 3.22.0')
gio_dep =  dependency('gio-2.0', version: '>= 2.43.4')
sqlite3_dep = dependency('sqlite3', version: '>= 3.0')
gnome = import('gnome')
pkg = import('pkgconfig')

gtk_prefix = get_option('prefix')
gtk_includedir = join_paths(gtk_prefix, get_option('includedir'))
gtk_libdir = join_paths(gtk_prefix, get_option('libdir'))
gtk_datadir = join_paths(gtk_prefix, get_option('datadir'))
gtk_localedir = join_paths(gtk_prefix, get_option('localedir'))
gtk_sysconfdir = join_paths(gtk_prefix, get_option('sysconfdir'))
gtk_applicationsdir = join_paths(gtk_datadir, 'applications')
gtk_schemasdir = join_paths(gtk_datadir, 'glib-2.0/schemas')
gtk_appdatadir = join_paths(gtk_datadir, 'metainfo')

confinc = include_directories('.')
 extra_demo_ldflags = []

sources = files(['bookwindow.c','bookmenu.c','bookstorewin.c','main.c'
,'ephy-sqlite-connection.c','ephy-sqlite-statement.c'
,'book-widget.c','book-doc.c','book-epub.c','book-mobi.c','book-mng.c'
,'bookdatabase.c'])
booksAppSources = files(['bookwindow.c','bookmenu.c','bookapplication.c','bookstorewin.c','bookmain.c','booktreeview.c'
,'ephy-sqlite-connection.c','ephy-sqlite-statement.c'
,'book-widget.c','book-doc.c','book-epub.c','book-mobi.c','book-mng.c'
,'bookdatabase.c'])
#booksAppSources = files([ 'bookapplication.c' ,'bookmain.c','bookstorewin.c','bookwindow.c'])
testAppSources=files(['bookwindowtest.c','bookwindow.c','bookmenu.c'])


gtk_schema_build_dir = meson.current_build_dir()
 booksApp_resources = gnome.compile_resources('book_resources',
  'bookapp.gresource.xml',
  source_dir: '.')

gtk_settings_schemas = [

]
install_data(gtk_settings_schemas, install_dir: gtk_schemasdir)
#gnome.compile_schemas(depend_files: files(gtk_settings_schemas),
#                    build_by_default: true)
booksApp_schemas = gnome.compile_schemas()

executable(booksApp,
  booksApp_resources,
  booksApp_schemas,
  sources:booksAppSources,
     include_directories: top_inc,
    dependencies: [
    libgepub_dep,
    glib_dep,
    gtk_dep,
    gio_dep,
    sqlite3_dep,
    libmobi_dep
  ]
)


 