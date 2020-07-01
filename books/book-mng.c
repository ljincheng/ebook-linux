
#include "book-mng.h"
#include <config.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <libsoup/soup.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <stdarg.h>
#include <string.h>
#include "book-widget.h"


#define BOOKTYPECSS "img{ max-width:600px;} body{padding:0px 60px 60px 60px; margin:0px;} #gepubwrap{margin-left:20px; margin-right:20px;}"
static void
book_data_set_epub_uri (xmlNode *node,
              const gchar *path,
              const gchar *tagname,
              const gchar *attr,
              const gchar *ns)
{
    xmlNode *cur_node = NULL;
    xmlChar *text = NULL;

    gchar *attrname = NULL;

    SoupURI *baseURI;
    gchar *basepath = g_strdup_printf ("epub:///%s/", path);
    baseURI = soup_uri_new (basepath);
    g_free (basepath);

    if (ns) {
        attrname = g_strdup_printf ("%s:%s", ns, attr);
    } else {
        attrname = g_strdup (attr);
    }

    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE ) {

            text = xmlGetProp (cur_node, BAD_CAST (attr));


            if (!strcmp ((const char *) cur_node->name, tagname) && text) {
                SoupURI *uri = soup_uri_new_with_base (baseURI, (const char *) text);
                gchar *value = soup_uri_to_string (uri, FALSE);
                xmlSetProp (cur_node, BAD_CAST (attrname), BAD_CAST (value));

                soup_uri_free (uri);
                g_free (value);
            }
            if (text) {
                xmlFree (text);
                text = NULL;
            }
        }

        if (cur_node->children)
            book_data_set_epub_uri (cur_node->children, path, tagname, attr, ns);
    }

    g_free (attrname);

    soup_uri_free (baseURI);
}

void book_data_head_node(xmlNode *root_element)
{
   xmlNode *cur_node = NULL;
    xmlNode *ret = NULL;
//printf("开始找head节点==================\n");
    for (cur_node = root_element; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE ) {
            if (!strcmp ((const char *) cur_node->name, "head"))
            {
            //  printf("找到head节点============================\n");
             xmlNodePtr booknode= xmlNewChild(cur_node,NULL,BAD_CAST"link",NULL);
              xmlNewProp(booknode,BAD_CAST"href",BAD_CAST"book:///book.css");
              xmlNewProp(booknode,BAD_CAST"rel",BAD_CAST"stylesheet");
              xmlNewProp(booknode,BAD_CAST"type",BAD_CAST"text/css");
              return cur_node;
            }
      if (cur_node->children)
            book_data_head_node (cur_node->children);
        }
    }
}

GBytes *
book_data_set_headnode (	GBytes *content)
{
 
   xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    guchar *buffer;
    const gchar *data;
    gsize bufsize;

    data = g_bytes_get_data (content, &bufsize);
    doc = xmlReadMemory (data, bufsize, "", NULL, XML_PARSE_NOWARNING | XML_PARSE_NOERROR);
    root_element = xmlDocGetRootElement (doc);


  book_data_head_node(root_element);
  xmlDocDumpFormatMemory (doc, (xmlChar**)&buffer, (int*)&bufsize, 1);
    xmlFreeDoc (doc);
  return g_bytes_new_take (buffer, bufsize);
    //return g_bytes_new_static (buffer, bufsize);

//return content;

}

GBytes *
book_data_replace_resources (GBytes *content, const gchar *path)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    guchar *buffer;
    const gchar *data;
    gsize bufsize;

    data = g_bytes_get_data (content, &bufsize);
    doc = xmlReadMemory (data, bufsize, "", NULL, XML_PARSE_NOWARNING | XML_PARSE_NOERROR);
    root_element = xmlDocGetRootElement (doc);

    // replacing css resources
    book_data_set_epub_uri (root_element, path, "link", "href", NULL);
    // replacing images resources
    book_data_set_epub_uri (root_element, path, "img", "src", NULL);
    book_data_set_epub_uri (root_element, path, "video", "src", NULL);
    book_data_set_epub_uri (root_element, path, "source", "src", NULL);

    // replacing svg images resources
    book_data_set_epub_uri (root_element, path, "image", "href", "xlink");
    // replacing crosslinks
    book_data_set_epub_uri (root_element, path, "a", "href", NULL);
    
    book_data_head_node(root_element);

    xmlDocDumpFormatMemory (doc, (xmlChar**)&buffer, (int*)&bufsize, 1);
    xmlFreeDoc (doc);
    return g_bytes_new_take (buffer, bufsize);
}

void
book_webview_resource_callback(WebKitURISchemeRequest *request, gpointer user_data)
{
	GInputStream *stream;
	gchar *path;
	gchar *uri;
	gchar *mime = NULL;
	BookWidget *widget = user_data;
	GBytes *contents;
	BookData *bookData = NULL;

	uri = g_strdup(webkit_uri_scheme_request_get_uri(request));

 contents = g_bytes_new_static(BOOKTYPECSS, sizeof(BOOKTYPECSS));

	if (contents == NULL || !contents) {
		contents = g_byte_array_free_to_bytes(g_byte_array_sized_new(0));
		mime = g_strdup("application/octet-stream");
	}

	mime = g_strdup("text/css");
	stream = g_memory_input_stream_new_from_bytes(contents);
	webkit_uri_scheme_request_finish(request, stream, g_bytes_get_size(contents), mime);



	g_object_unref(stream);
	g_bytes_unref(contents);
	g_free(mime);
	g_free(uri);

}
