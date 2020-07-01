# These should be in the order you want them to appear in the
# demo app, which means alphabetized by demo title, not filename
mobiApp='mobitest'
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

mobiAppSources = files(['compress.c','exth.c','main.c','mobi.c','mobi_file.c','pdb.c','utils.c'])

gtk_schema_build_dir = meson.current_build_dir()
sqliteApp_schemas = gnome.compile_schemas()



executable(mobiApp,
  sqliteApp_schemas,
 sources:mobiAppSources,
   include_directories: top_inc,
    dependencies: [
    libgepub_dep,
    glib_dep,
    gtk_dep,
    gio_dep
  ]
)

 