/* GepubDoc
 *
 * Copyright (C) 2011 Daniel Garcia <danigm@wadobo.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <config.h>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <string.h>

#include "gepub-utils.h"
#include "gepub-doc.h"
#include "gepub-archive.h"
#include "gepub-text-chunk.h"


static GQuark
gepub_error_quark(void)
{
	static GQuark q = 0;

	if (q == 0)
		q = g_quark_from_string("gepub-quark");
	return q;
}

/**
 * GepubDocError:
 * @GEPUB_ERROR_INVALID: Invalid file
 *
 * Common errors that may be reported by GepubDoc.
 */
typedef enum {
	GEPUB_ERROR_INVALID = 0, /*< nick=Invalid >*/
} GepubDocError;



static void gepub_doc_fill_resources(GepubDoc *doc);
static void gepub_doc_fill_spine(GepubDoc *doc);
static void gepub_doc_initable_iface_init(GInitableIface *iface);

struct _GepubDoc {
	GObject parent;

	GepubArchive *archive;
	GBytes *content;
	gchar *content_base;
	gchar *path;
	GHashTable *resources;

	GList *spine;
	GList *chapter;
};

struct _GepubDocClass {
	GObjectClass parent_class;
};

enum {
	PROP_0,
	PROP_PATH,
	PROP_CHAPTER,
	NUM_PROPS
};

static GParamSpec *properties[NUM_PROPS] = { NULL, };

G_DEFINE_TYPE_WITH_CODE(GepubDoc, gepub_doc, G_TYPE_OBJECT,
			G_IMPLEMENT_INTERFACE(G_TYPE_INITABLE, gepub_doc_initable_iface_init))

static void
gepub_resource_free(GepubResource *res)
{
	g_free(res->mime);
	g_free(res->uri);
	g_free(res);
}

static void
gepub_doc_finalize(GObject *object)
{
	GepubDoc *doc = GEPUB_DOC(object);

	g_clear_object(&doc->archive);
	g_clear_pointer(&doc->content, g_bytes_unref);
	g_clear_pointer(&doc->path, g_free);
	g_clear_pointer(&doc->resources, g_hash_table_destroy);

	if (doc->spine) {
		g_list_foreach(doc->spine, (GFunc)g_free, NULL);
		g_clear_pointer(&doc->spine, g_list_free);
	}

	G_OBJECT_CLASS(gepub_doc_parent_class)->finalize(object);
}

static void
gepub_doc_set_property(GObject      *object,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec   *pspec)
{
	GepubDoc *doc = GEPUB_DOC(object);

	switch (prop_id) {
	case PROP_PATH:
		doc->path = g_value_dup_string(value);
		break;

	case PROP_CHAPTER:
		gepub_doc_set_chapter(doc, g_value_get_int(value));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void
gepub_doc_get_property(GObject    *object,
		       guint prop_id,
		       GValue     *value,
		       GParamSpec *pspec)
{
	GepubDoc *doc = GEPUB_DOC(object);

	switch (prop_id) {
	case PROP_PATH:
		g_value_set_string(value, doc->path);
		break;

	case PROP_CHAPTER:
		g_value_set_int(value, gepub_doc_get_chapter(doc));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void
gepub_doc_init(GepubDoc *doc)
{
	/* doc resources hashtable:
	 * id : (mime, path)
	 */
	doc->resources = g_hash_table_new_full(g_str_hash,
					       g_str_equal,
					       (GDestroyNotify)g_free,
					       (GDestroyNotify)gepub_resource_free);
}

static void
gepub_doc_class_init(GepubDocClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->finalize = gepub_doc_finalize;
	object_class->set_property = gepub_doc_set_property;
	object_class->get_property = gepub_doc_get_property;

	properties[PROP_PATH] =
		g_param_spec_string("path",
				    "Path",
				    "Path to the EPUB document",
				    NULL,
				    G_PARAM_READWRITE |
				    G_PARAM_CONSTRUCT_ONLY |
				    G_PARAM_STATIC_STRINGS);
	properties[PROP_CHAPTER] =
		g_param_spec_int("chapter",
				 "Current chapter",
				 "The current chapter index",
				 -1, G_MAXINT, 0,
				 G_PARAM_READWRITE |
				 G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(object_class, NUM_PROPS, properties);
}

static gboolean
gepub_doc_initable_init(GInitable     *initable,
			GCancellable  *cancellable,
			GError       **error)
{
	GepubDoc *doc = GEPUB_DOC(initable);
	gchar *file;
	gint i = 0, len;

	g_assert(doc->path != NULL);

	doc->archive = gepub_archive_new(doc->path);
	file = gepub_archive_get_root_file(doc->archive);
	if (!file) {
		if (error != NULL) {
			g_set_error(error, gepub_error_quark(), GEPUB_ERROR_INVALID,
				    "Invalid epub file: %s", doc->path);
		}
		return FALSE;
	}
	doc->content = gepub_archive_read_entry(doc->archive, file);
	if (!doc->content) {
		if (error != NULL) {
			g_set_error(error, gepub_error_quark(), GEPUB_ERROR_INVALID,
				    "Invalid epub file: %s", doc->path);
		}
		return FALSE;
	}

	len = strlen(file);
	doc->content_base = g_strdup("");
	for (i = 0; i < len; i++) {
		if (file[i] == '/') {
			g_free(doc->content_base);
			doc->content_base = g_strndup(file, i + 1);
			break;
		}
	}

	gepub_doc_fill_resources(doc);
	gepub_doc_fill_spine(doc);

	g_free(file);

	return TRUE;
}

static void
gepub_doc_initable_iface_init(GInitableIface *iface)
{
	iface->init = gepub_doc_initable_init;
}

/**
 * gepub_doc_new:
 * @path: the epub doc path
 * @error: (nullable): Error
 *
 * Returns: (transfer full): the new GepubDoc created
 */
GepubDoc *
gepub_doc_new(const gchar *path, GError **error)
{
	return g_initable_new(GEPUB_TYPE_DOC,
			      NULL, error,
			      "path", path,
			      NULL);
}

static void
gepub_doc_fill_resources(GepubDoc *doc)
{
	xmlDoc *xdoc = NULL;
	xmlNode *root_element = NULL;
	xmlNode *mnode = NULL;
	xmlNode *item = NULL;
	gchar *id, *tmpuri, *uri;
	GepubResource *res;
	const char *data;
	gsize size;
	data = g_bytes_get_data(doc->content, &size);
	xdoc = xmlRecoverMemory(data, size);
	root_element = xmlDocGetRootElement(xdoc);
	mnode = gepub_utils_get_element_by_tag(root_element, "manifest");

	item = mnode->children;
	while (item) {
		if (item->type != XML_ELEMENT_NODE) {
			item = item->next;
			continue;
		}

		id = gepub_utils_get_prop(item, "id");
		tmpuri = gepub_utils_get_prop(item, "href");
		uri = g_strdup_printf("%s%s", doc->content_base, tmpuri);
		g_free(tmpuri);

		res = g_malloc(sizeof(GepubResource));
		res->mime = gepub_utils_get_prop(item, "media-type");
		res->uri = uri;
		g_hash_table_insert(doc->resources, id, res);
		item = item->next;
	}

	xmlFreeDoc(xdoc);
}

static void
gepub_doc_fill_spine(GepubDoc *doc)
{
	xmlDoc *xdoc = NULL;
	xmlNode *root_element = NULL;
	xmlNode *snode = NULL;
	xmlNode *item = NULL;
	gchar *id;
	const char *data;
	gsize size;
	GList *spine = NULL;
	data = g_bytes_get_data(doc->content, &size);
	xdoc = xmlRecoverMemory(data, size);
	root_element = xmlDocGetRootElement(xdoc);
	snode = gepub_utils_get_element_by_tag(root_element, "spine");

	item = snode->children;
	while (item) {
		if (item->type != XML_ELEMENT_NODE) {
			item = item->next;
			continue;
		}

		id = gepub_utils_get_prop(item, "idref");

		spine = g_list_prepend(spine, id);
		item = item->next;
	}

	doc->spine = g_list_reverse(spine);
	doc->chapter = doc->spine;

	xmlFreeDoc(xdoc);
}

/**
 * gepub_doc_get_content:
 * @doc: a #GepubDoc
 *
 * Returns: (transfer none): the document content
 */
GBytes *
gepub_doc_get_content(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);

	return doc->content;
}

/**
 * gepub_doc_get_metadata:
 * @doc: a #GepubDoc
 * @mdata: a metadata name string, GEPUB_META_TITLE for example
 *
 * Returns: (transfer full): metadata string
 */
gchar * gepub_doc_get_metadata(GepubDoc *doc,const gchar *mdata)
{
	xmlDoc *xdoc=NULL;
	xmlNode *root_element=NULL;
	xmlNode *mnode=NULL;
	xmlNode *mdata_node=NULL;
	gchar *ret;
	xmlChar *text;
	const char *data;
	gsize size;

	g_return_val_if_fail(GEPUB_IS_DOC(doc),NULL);
	g_return_val_if_fail(mdata != NULL,NULL);
	data = g_bytes_get_data(doc->content,&size);
	xdoc = xmlRecoverMemory(data,size);
	root_element = xmlDocGetRootElement(xdoc);
	mnode = gepub_utils_get_element_by_tag(root_element,"metadata");
	mdata_node = gepub_utils_get_element_by_tag(mnode,mdata);

	text = xmlNodeGetContent(mdata_node);
	ret = g_strdup((const char*)text);
	xmlFree(text);
	
	xmlFreeDoc(xdoc);

	return ret;
}	

/**
 * gepub_doc_get_resources:
 * @doc: a #GepubDoc
 *
 * Returns: (element-type utf8 Gepub.Resource) (transfer none): doc resource table
 */
GHashTable *
gepub_doc_get_resources(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);

	return doc->resources;
}

/**
 * gepub_doc_get_resource_by_id:
 * @doc: a #GepubDoc
 * @id: the resource id
 *
 * Returns: (transfer full): the resource content
 */
GBytes *
gepub_doc_get_resource_by_id(GepubDoc *doc, const gchar *id)
{
	GepubResource *gres;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(id != NULL, NULL);

	gres = g_hash_table_lookup(doc->resources, id);
	if (!gres) {
		// not found
		return NULL;
	}

	return gepub_archive_read_entry(doc->archive, gres->uri);
}

/**
 * gepub_doc_get_resource:
 * @doc: a #GepubDoc
 * @path: the resource path
 *
 * Returns: (transfer full): the resource content
 */
GBytes *
gepub_doc_get_resource(GepubDoc *doc, const gchar *path)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(path != NULL, NULL);
	return gepub_archive_read_entry(doc->archive, path);
}

/**
 * gepub_doc_get_resource_mime_by_id:
 * @doc: a #GepubDoc
 * @id: the resource id
 *
 * Returns: (transfer full): the resource content
 */
gchar *
gepub_doc_get_resource_mime_by_id(GepubDoc *doc, const gchar *id)
{
	GepubResource *gres;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(id != NULL, NULL);

	gres = g_hash_table_lookup(doc->resources, id);
	if (!gres) {
		// not found
		return NULL;
	}

	return g_strdup(gres->mime);
}

/**
 * gepub_doc_get_resource_mime:
 * @doc: a #GepubDoc
 * @path: the resource path
 *
 * Returns: (transfer full): the resource mime
 */
gchar *
gepub_doc_get_resource_mime(GepubDoc *doc, const gchar *path)
{
	GepubResource *gres;
	GList *keys;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(path != NULL, NULL);

	keys = g_hash_table_get_keys(doc->resources);

	while (keys) {
		gres = ((GepubResource*)g_hash_table_lookup(doc->resources, keys->data));
		if (!strcmp(gres->uri, path))
			break;
		keys = keys->next;
	}

	if (keys)
		return g_strdup(gres->mime);
	else
		return NULL;
}

/**
 * gepub_doc_get_current_mime:
 * @doc: a #GepubDoc
 *
 * Returns: (transfer full): the current resource mime
 */
gchar *
gepub_doc_get_current_mime(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(doc->chapter != NULL, NULL);

	return gepub_doc_get_resource_mime_by_id(doc, doc->chapter->data);
}

/**
 * gepub_doc_get_current:
 * @doc: a #GepubDoc
 *
 * Returns: (transfer full): the current chapter data
 */
GBytes *
gepub_doc_get_current(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(doc->chapter != NULL, NULL);

	return gepub_doc_get_resource_by_id(doc, doc->chapter->data);
}

/**
 * gepub_doc_get_current_with_epub_uris:
 * @doc: a #GepubDoc
 *
 * Returns: (transfer full): the current chapter
 * data, with resource uris renamed so they have the epub:/// prefix and all
 * are relative to the root file
 */
GBytes *
gepub_doc_get_current_with_epub_uris(GepubDoc *doc)
{
	GBytes *content, *replaced;
	gchar *path, *base;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);

	content = gepub_doc_get_current(doc);
	path = gepub_doc_get_current_path(doc);
	// getting the basepath of the current xhtml loaded
	base = g_path_get_dirname(path);
	replaced = gepub_utils_replace_resources(content, base);

	g_free(path);
	g_bytes_unref(content);

	return replaced;
}

/**
 * gepub_doc_get_text:
 * @doc: a #GepubDoc
 *
 * Returns: (element-type Gepub.TextChunk) (transfer full): the list of text in the current chapter.
 */
GList *
gepub_doc_get_text(GepubDoc *doc)
{
	xmlDoc *xdoc = NULL;
	xmlNode *root_element = NULL;
	GBytes *current;
	const gchar *data;
	gsize size;

	GList *texts = NULL;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);

	current = gepub_doc_get_current(doc);
	if (!current) {
		return NULL;
	}
	data = g_bytes_get_data(current, &size);
	xdoc = htmlReadMemory(data, size, "", NULL, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
	root_element = xmlDocGetRootElement(xdoc);
	texts = gepub_utils_get_text_elements(root_element);

	g_bytes_unref(current);
	xmlFreeDoc(xdoc);

	return texts;
}

/**
 * gepub_doc_get_text_by_id:
 * @doc: a #GepubDoc
 * @id: the resource id
 *
 * Returns: (element-type Gepub.TextChunk) (transfer full): the list of text in the current chapter.
 */
GList *
gepub_doc_get_text_by_id(GepubDoc *doc, const gchar *id)
{
	xmlDoc *xdoc = NULL;
	xmlNode *root_element = NULL;
	gsize size;
	const gchar *res;
	GBytes *contents;

	GList *texts = NULL;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(id != NULL, NULL);

	contents = gepub_doc_get_resource_by_id(doc, id);
	if (!contents) {
		return NULL;
	}
	res = g_bytes_get_data(contents, &size);
	xdoc = htmlReadMemory(res, size, "", NULL, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
	root_element = xmlDocGetRootElement(xdoc);
	texts = gepub_utils_get_text_elements(root_element);

	g_bytes_unref(contents);
	xmlFreeDoc(xdoc);

	return texts;
}

static gboolean
gepub_doc_set_chapter_internal(GepubDoc *doc,
			       GList    *chapter)
{
	if (!chapter || doc->chapter == chapter)
		return FALSE;

	doc->chapter = chapter;
	g_object_notify_by_pspec(G_OBJECT(doc), properties[PROP_CHAPTER]);

	return TRUE;
}

/**
 * gepub_doc_go_next:
 * @doc: a #GepubDoc
 *
 * Returns: TRUE on success, FALSE if there's no next chapter
 */
gboolean
gepub_doc_go_next(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), FALSE);
	g_return_val_if_fail(doc->chapter != NULL, FALSE);

	return gepub_doc_set_chapter_internal(doc, doc->chapter->next);
}

/**
 * gepub_doc_go_prev:
 * @doc: a #GepubDoc
 *
 * Returns: TRUE on success, FALSE if there's no previous chapter
 */
gboolean
gepub_doc_go_prev(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), FALSE);
	g_return_val_if_fail(doc->chapter != NULL, FALSE);

	return gepub_doc_set_chapter_internal(doc, doc->chapter->prev);
}

/**
 * gepub_doc_get_n_chapters:
 * @doc: a #GepubDoc
 *
 * Returns: the number of chapters in the document
 */
int
gepub_doc_get_n_chapters(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), 0);

	return g_list_length(doc->spine);
}

/**
 * gepub_doc_get_chapter:
 * @doc: a #GepubDoc
 *
 * Returns: the current chapter index, starting from 0
 */
int
gepub_doc_get_chapter(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), 0);
	g_return_val_if_fail(doc->spine != NULL, 0);
	g_return_val_if_fail(doc->chapter != NULL, 0);

	return g_list_position(doc->spine, doc->chapter);
}

/**
 * gepub_doc_set_chapter:
 * @doc: a #GepubDoc
 * @index: the index of the new chapter
 *
 * Sets the document current chapter to @index.
 */
void
gepub_doc_set_chapter(GepubDoc *doc,
		      gint index)
{
	GList *chapter;

	g_return_if_fail(GEPUB_IS_DOC(doc));

	g_return_if_fail(index >= 0 && index <= gepub_doc_get_n_chapters(doc));

	chapter = g_list_nth(doc->spine, index);
	gepub_doc_set_chapter_internal(doc, chapter);
}

/**
 * gepub_doc_get_cover:
 * @doc: a #GepubDoc
 *
 * Returns: (transfer full): cover file path to retrieve with
 * gepub_doc_get_resource
 */
gchar *
gepub_doc_get_cover(GepubDoc *doc)
{
	xmlDoc *xdoc = NULL;
	xmlNode *root_element = NULL;
	xmlNode *mnode = NULL;
	gchar *ret;
	const char *data;
	gsize size;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(doc->content != NULL, NULL);

	data = g_bytes_get_data(doc->content, &size);
	xdoc = xmlRecoverMemory(data, size);
	root_element = xmlDocGetRootElement(xdoc);
	mnode = gepub_utils_get_element_by_attr(root_element, "name", "cover");
	ret = gepub_utils_get_prop(mnode, "content");

	xmlFreeDoc(xdoc);

	return ret;
}

/**
 * gepub_doc_get_resource_path:
 * @doc: a #GepubDoc
 * @id: the resource id
 *
 * Returns: (transfer full): the resource path
 */
gchar *
gepub_doc_get_resource_path(GepubDoc *doc, const gchar *id)
{
	GepubResource *gres;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(id != NULL, NULL);

	gres = g_hash_table_lookup(doc->resources, id);
	if (!gres) {
		// not found
		return NULL;
	}

	return g_strdup(gres->uri);
}

/**
 * gepub_doc_get_current_path:
 * @doc: a #GepubDoc
 *
 * Returns: (transfer full): the current resource path
 */
gchar *
gepub_doc_get_current_path(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(doc->chapter != NULL, NULL);

	return gepub_doc_get_resource_path(doc, doc->chapter->data);
}

/**
 * gepub_doc_get_current_id:
 * @doc: a #GepubDoc
 *

 * Returns: (transfer none): the current resource id
 */
const gchar *
gepub_doc_get_current_id(GepubDoc *doc)
{
	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(doc->chapter != NULL, NULL);

	return doc->chapter->data;
}

gchar   *
gepub_doc_get_path(GepubDoc *doc)
{
	return doc->path;
}

GList *
gepub_doc_get_chapter_all(GepubDoc *doc)
{
	return doc->chapter;
}

void gepub_navPoint_free(GepubNavPoint *point)
{
	if (point != NULL) {
		while (point->next) {
			gepub_navPoint_free(point->next);
			while (point->children) {
				gepub_navPoint_free(point->children);
				g_free(point->children->text);
				g_free(point->children->id);
				g_free(point->children->src);
				g_free(point->children);
			}

			g_free(point->next->text);
			g_free(point->next->id);
			g_free(point->next->src);
			g_free(point->next);
		}
		g_printf("NavPoint正常释放");
	}
}

void gepub_navPoint_print(GepubNavPoint *navPoint)
{
	//g_return_val_if_fail(navPoint != NULL, NULL);

    //  g_printf("标题：%s(id:%s,playOrder:%d,src:%s)\n", navPoint->text, navPoint->id, navPoint->playOrder, navPoint->src);
      if(navPoint->hasChildren)
        gepub_navPoint_print(navPoint->children);
      if(navPoint->hasNext)
        {
          gepub_navPoint_print(navPoint->next);
        }


}

GepubNavPoint   * gepub_nav_get_list(GepubDoc *doc)
{
  printf("生成epub菜单\n");
	xmlDoc *xdoc = NULL;
	xmlNode *root_element = NULL;
	xmlNode *mnode = NULL;
	xmlNode *item = NULL;
	const char *data;
	gsize size;
	GBytes * toc_data;
  gchar *tocfile;
	GepubNavPoint   *rootNavPoint = NULL;

//toc_data = gepub_doc_get_resource_by_id(doc, "ncx");
//tocfile=g_strdup_printf("%s%s",doc->content_base,"toc.ncx");
//  toc_data=gepub_doc_get_resource(doc,tocfile);
 // g_free(tocfile);

	GList *keys;
 GepubResource *gres;
printf("OK---\n");
	keys = g_hash_table_get_keys(doc->resources);

	while (keys) {
		gres = ((GepubResource*)g_hash_table_lookup(doc->resources, keys->data));
      g_printf("文件mime：%s \n",gres->mime);
		if (!strcmp(gres->mime, "application/x-dtbncx+xml"))
      {
        g_printf("ncx文件：%s \n",gres->uri);
         toc_data=gepub_doc_get_resource(doc,gres->uri);
			break;
      }
		keys = keys->next;
	}


 g_printf("获取ncx文件\n");
  g_return_val_if_fail(toc_data != NULL, NULL);
 g_printf("获取ncx文件2\n");
	data = g_bytes_get_data(toc_data, &size);
  g_printf("获取ncx文件3\n");
	// g_printf("获取toc.ncx文件内容：%s\n",data);
	xdoc = xmlRecoverMemory(data, size);
    g_printf("获取ncx文件4\n");
	root_element = xmlDocGetRootElement(xdoc);
  g_printf("获取ncx文件5\n");
	mnode = gepub_utils_get_element_by_tag(root_element, "navMap");
if(mnode==NULL)
    {
      	xmlFreeDoc(xdoc);
          return  NULL;
    }
	item = mnode->children;
  g_printf("获取ncx文件6\n");
  	rootNavPoint = g_malloc(sizeof(GepubNavPoint));
			rootNavPoint->id = NULL;
			rootNavPoint->playOrder =NULL;
			rootNavPoint->src =NULL;
			rootNavPoint->text = NULL;
      rootNavPoint->hasNext=FALSE;
      rootNavPoint->hasChildren=FALSE;

   loop_child_navpoint(doc->content_base,item, rootNavPoint);
  //g_printf("准备打印：%s标题\n",rootNavPoint->text);
	//gepub_navPoint_print(rootNavPoint);
	//g_free(rootNavPoint);
  //g_printf("END\n");
	xmlFreeDoc(xdoc);

	return rootNavPoint;
}
int TOPINDEX=0;


void loop_child_navpoint(const gchar *basePath,xmlNode *item, GepubNavPoint  *parentNavPoint)
{

  xmlNode *labelText = NULL;
	xmlNode * contentNode = NULL;
	xmlChar *text = NULL;
	xmlChar *src = NULL;
	xmlChar *id = NULL;
	xmlChar *playOrder = NULL;
	GepubNavPoint   *nextNavPoint = NULL;
  int index=0;
	while (item) {
		if (item->type != XML_ELEMENT_NODE) {
			item = item->next;
			continue;
		}

		if (!strcmp((const char*)item->name, "navPoint")) {
			labelText = gepub_utils_get_element_by_tag(item, "text");
			text = xmlNodeGetContent(labelText);
			contentNode = gepub_utils_get_element_by_tag(item, "content");
			src = xmlGetProp(contentNode, BAD_CAST("src"));
			id = gepub_utils_get_prop(item, "id");
			playOrder = gepub_utils_get_prop(item, "playOrder");
			// g_printf("标题：%s(id:%s,playOrder:%s,src:%s)\n", g_strdup ((const char *) text),id,playOrder,g_strdup ((const char *) src));
			GepubNavPoint     *navPoint = g_malloc(sizeof(GepubNavPoint));
			navPoint->id = g_strdup((const char*)id);
			navPoint->playOrder = g_strdup((const char*)playOrder);
			navPoint->src =g_strdup_printf("%s%s", basePath, src); //g_strdup((const char*)src);
			navPoint->text = g_strdup((const char*)text);
      navPoint->hasNext=FALSE;
      navPoint->hasChildren=FALSE;
			if (index ==0) {
				nextNavPoint = navPoint;
        if(parentNavPoint==NULL)
          {
            parentNavPoint=nextNavPoint;
          }else{
            parentNavPoint->children = nextNavPoint;
            parentNavPoint->hasChildren=TRUE;
           // g_printf("0父：%s子标题:%s\n",parentNavPoint->text,parentNavPoint->children->text);
          }
			}else    {
				nextNavPoint->next = navPoint;
        nextNavPoint->hasNext=TRUE;
       //  g_printf("父：%s子标题:%s\n",nextNavPoint->text,nextNavPoint->next->text);
        nextNavPoint=nextNavPoint->next ;

        TOPINDEX++;
			}
      index++;
      if(item->children)
        {
			      loop_child_navpoint(basePath,item->children, navPoint);
        }
		}
		item = item->next;
	}

}

gchar *gepub_doc_get_file_fullpath(GepubDoc *doc,const gchar *tmpuri){
gchar *uri;
uri = g_strdup_printf("epub:///%s%s", doc->content_base, tmpuri);
return uri;
}

int gepub_doc_get_chapter_by_path(GepubDoc *doc, const gchar *path){
	GepubResource *gres;
	GList *keys;
GList *list,*it;

	g_return_val_if_fail(GEPUB_IS_DOC(doc), NULL);
	g_return_val_if_fail(path != NULL, NULL);

	keys = g_hash_table_get_keys(doc->resources);
	while (keys) {
		gres = ((GepubResource*)g_hash_table_lookup(doc->resources, keys->data));
   // g_printf("gres-uri=%s,path=%s\n",gres->uri,path);
		if (!strcmp(gres->uri, path))
			break;
		keys = keys->next;
	}
	
	if (keys)
	{
		list=doc->spine;
		int index=0;
		  for (it = list; it; it = it->next) {
			if (!strcmp(it->data, keys->data))
				return index;
			index++;
    		  }
	} 
	else
	return -1;
}
