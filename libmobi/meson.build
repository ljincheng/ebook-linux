libmobi_deps = [
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

libmobiApp='libmobitest'

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

libmobiAppSources = files(['buffer.c','compression.c','debug.c','encryption.c'
,'index.c','memory.c','meta.c','miniz.c','opf.c','parse_rawml.c','read.c','sha1.c'
,'structure.c','util.c','write.c','xmlwriter.c'
,'mobi-common.c','mobi-parse.c'
#,'book-widget.c','book-doc.c'
#,'main.c'
])

gtk_schema_build_dir = meson.current_build_dir()
sqliteApp_schemas = gnome.compile_schemas()

libmobi = library(
  'libmobi-1.0',
  sources: libmobiAppSources,
  version: '1.0',
  soversion: '0',
  include_directories: top_inc,
  dependencies: libmobi_deps,
  install: true,
  install_dir: gepub_libdir
)

libmobi_dep = declare_dependency(
  link_with: libmobi,
  include_directories: include_directories('.'),
  dependencies: libmobi_deps
)

executable(libmobiApp,
  sqliteApp_schemas,
 sources:libmobiAppSources,
   include_directories: top_inc,
    dependencies: libmobi_deps
)

 configure_file(
  output: 'config.h',
  	input:'config.h',
  configuration: configuration_data()
)
