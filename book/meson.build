# These should be in the order you want them to appear in the
# demo app, which means alphabetized by demo title, not filename
ebookApp='bookApp'
glib_dep = dependency('glib-2.0', version: '>= 2.43.4')
gtk_dep = dependency('gtk+-3.0', version: '>= 3.22.0')
gio_dep =  dependency('gio-2.0', version: '>= 2.43.4')
jsonglib_dep = dependency('json-glib-1.0', version: '>= 1.0')
curl_dep = dependency('libcurl', version: '>= 7.59.0')
poppler_dep=dependency('poppler-glib',version:'>= 0.33.0')
libnotify_dep = dependency('libnotify', version: '>= 0.5.1')
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

bookSources = ['book-application.c','book-app-headerbar.c','book-app-window.c','book-app-marktreeview.c','book-app-bookinfotreeview.c'
,'book-window.c','book-webview.c'
,'book-doc.c','book-epub.c','book-mobi.c','book-application-import.c','book-doc-nav.c'
,'book-httpclient.c','book-http-res.c','book-manage.c','book-pdf.c','book-pdf-window.c','book-pdf-render.c','book-dialogs.c']

bookWidgetSources=['widgets/book-bench.c','widgets/book-erm.c','widgets/book-web-widget.c','widgets/book-download.c']
#bookDevtoolsSources=['devtools/book-dev-table.c']
bookTestSources=['book-test-window.c']

ebookSources =  bookSources+bookWidgetSources+bookTestSources


gtk_schema_build_dir = meson.current_build_dir()
 booksApp_resources = gnome.compile_resources('book_app_resources',
  'bookapp.gresource.xml',
  source_dir: '.')

gtk_settings_schemas = [

]
install_data(gtk_settings_schemas, install_dir: gtk_schemasdir)
#gnome.compile_schemas(depend_files: files(gtk_settings_schemas),
#                    build_by_default: true)
booksApp_schemas = gnome.compile_schemas()

executable(ebookApp,
  booksApp_resources,
  booksApp_schemas,
  sources:ebookSources,
     include_directories: top_inc,
    dependencies: [
    libgepub_dep,
    glib_dep,
    gtk_dep,
    gio_dep,
    poppler_dep,
    libmobi_dep,
    libbookcommon_dep,
    jsonglib_dep,
    curl_dep,
    libnotify_dep
  ]
)

#rpm = import('rpm')
#rpm.generate_spec_template()
