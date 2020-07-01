libnet_deps = [
dependency('libnotify'),
  dependency('zlib'),
  dependency('libxml-2.0'),
  dependency('libarchive'),
 dependency('webkit2gtk-4.0'),
  dependency('glib-2.0', version: '>= 2.43.4'),
 dependency('gtk+-3.0', version: '>= 3.22.0') ,
dependency('gio-2.0', version: '>= 2.43.4')  ,
dependency('sqlite3', version: '>= 3.0'),
libgepub_dep
]
libServerApp='server'
libClientApp='client'
libTestApp='testapp'
libTestShellApp='testShell'


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

libServerAppSources = files([ 
'book-server.c','book-meta.c'
])
libClientAppSources = files([ 
'client.c','book-meta.c'
])

libTestAppSources = files([ 
'test.c','book-meta.c'
])

libTestShellAppSources = files([
'testCom.c'
])

gtk_schema_build_dir = meson.current_build_dir()
sqliteApp_schemas = gnome.compile_schemas()
 

executable(libServerApp,
  sqliteApp_schemas,
 sources:libServerAppSources,
   include_directories: top_inc,
    dependencies: libnet_deps
)
executable(libClientApp,
  sqliteApp_schemas,
 sources:libClientAppSources,
   include_directories: top_inc,
    dependencies: libnet_deps
)
executable(libTestApp,
  sqliteApp_schemas,
 sources:libTestAppSources,
   include_directories: top_inc,
    dependencies: libnet_deps
)
executable(libTestShellApp,
  sqliteApp_schemas,
 sources:libTestShellAppSources,
   include_directories: top_inc,
    dependencies: libnet_deps
)


 configure_file(
  output: 'config.h',
  	input:'config.h',
  configuration: configuration_data()
)
