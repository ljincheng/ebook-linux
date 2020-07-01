libdevtools_deps = [
  dependency('glib-2.0', version: '>= 2.43.4'),
 dependency('gtk+-3.0', version: '>= 3.22.0') ,
dependency('gio-2.0', version: '>= 2.43.4')  ,
dependency('sqlite3', version: '>= 3.0'),
dependency('json-glib-1.0', version: '>= 1.0'),
libbookcommon_dep
]

libdevtoolsApp='libdevtoolsApp'

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

libdevtoolsSources = ['book-dev-table.c','book-dev-toollist.c','book-dev-bench.c','book-code-tag.c'
]

#libdevtoolsAppSources =  libdevtoolsSources+['main.c']

gtk_schema_build_dir = meson.current_build_dir()
sqliteApp_schemas = gnome.compile_schemas()

libbookcommon = library(
  'libdevtools-1.0',
  sources: libdevtoolsSources,
  version: '1.0',
  soversion: '0',
  include_directories: top_inc,
  dependencies: libdevtools_deps,
  install: true,
  install_dir: gepub_libdir
)

libbookcommon_dep = declare_dependency(
  link_with: libbookcommon,
  include_directories: include_directories('.'),
  dependencies: libdevtools_deps
)

#executable(libdevtoolsApp,
#  sqliteApp_schemas,
# sources:libdevtoolsAppSources,
#   include_directories: top_inc,
#    dependencies: libdevtools_deps
#)

 configure_file(
  output: 'config.h',
  #	input:'config.h',
  configuration: configuration_data()
)
