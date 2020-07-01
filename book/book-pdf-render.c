/*
 * Copyright (C) 2007 Carlos Garcia Campos  <carlosgc@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "book-pdf-render.h"

#include "config.h"
#include <cairo.h>
#include "book-common.h"




struct _BookPdfRender {
	GtkDrawingArea parent;
BookPdf   *bookPdf;

PopplerDocument      *doc;

	/* Properties */
	gint                  page_index;
	gdouble               scale;

	GtkWidget            *swindow;
	GtkWidget            *darea;
	GtkWidget            *fg_color_button;
	GtkWidget            *bg_color_button;
	GtkWidget            *copy_button;

	PopplerPage          *page;
	cairo_surface_t      *surface;

	GdkPoint              start;
	GdkPoint              stop;
	PopplerRectangle      doc_area;
	cairo_surface_t      *selection_surface;
	PopplerSelectionStyle style;
	PopplerColor          glyph_color;
	PopplerColor          background_color;
	guint                 selections_idle;
	cairo_region_t       *selection_region;
	cairo_region_t       *selected_region;
	GdkCursorType         cursor;
	gchar                *selected_text;
};
G_DEFINE_TYPE (BookPdfRender, book_pdf_render, GTK_TYPE_DRAWING_AREA)


static void book_pdf_render_init (BookPdfRender *self)
{

}

static void book_pdf_render_finalize (GObject *object)
{
    book_log_debug("[BookPdfRender]内存清理\n");
  // BookPdfRender *self = BOOK_PDF_RENDER(object);

  G_OBJECT_CLASS (book_pdf_render_parent_class)->finalize (object);
}

static void book_pdf_render_class_init (BookPdfRenderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = book_pdf_render_finalize;
}



static void
pgd_selections_clear_selections (BookPdfRender *demo)
{
	demo->start.x = -1;

	if (demo->selection_surface) {
		cairo_surface_destroy (demo->selection_surface);
		demo->selection_surface = NULL;
	}

	if (demo->selection_region) {
		cairo_region_destroy (demo->selection_region);
		demo->selection_region = NULL;
	}

	if (demo->selected_text) {
		g_free (demo->selected_text);
		demo->selected_text = NULL;
	}

	if (demo->selected_region) {
		cairo_region_destroy (demo->selected_region);
		demo->selected_region = NULL;
	}
}

static void
pgd_selections_free (BookPdfRender *demo)
{
	if (!demo)
		return;

	if (demo->selections_idle > 0) {
		g_source_remove (demo->selections_idle);
		demo->selections_idle = 0;
	}

	if (demo->doc) {
		g_object_unref (demo->doc);
		demo->doc = NULL;
	}


	if (demo->bookPdf) {
		g_clear_object (&demo->bookPdf);
		demo->bookPdf = NULL;
	}



	if (demo->surface) {
		cairo_surface_destroy (demo->surface);
		demo->surface = NULL;
	}

	pgd_selections_clear_selections (demo);

	g_free (demo);
}

static void
pgd_selections_update_selection_region (BookPdfRender *demo)
{
	PopplerRectangle area = { 0, 0, 0, 0 };

	if (demo->selection_region)
		cairo_region_destroy (demo->selection_region);

  book_pdf_chapter_get_size (demo->bookPdf, demo->page_index,&area.x2, &area.y2);
  demo->selection_region =book_pdf_get_selected_region (demo->bookPdf, demo->page_index,1.0,POPPLER_SELECTION_GLYPH,&area);
/*
poppler_page_get_size (demo->page, &area.x2, &area.y2);
demo->selection_region = poppler_page_get_selected_region (demo->page,
                                                                   1.0,
                                                                   POPPLER_SELECTION_GLYPH,
                                                                 &area);
  */
}

static void
pgd_selections_update_selected_text (BookPdfRender *demo)
{
	gchar *text;

	if (demo->selected_region)
		cairo_region_destroy (demo->selected_region);

   demo->selected_region =book_pdf_get_selected_region (demo->bookPdf, demo->page_index,1.0,demo->style,&demo->doc_area);

	if (demo->selected_text)
		g_free (demo->selected_text);
	demo->selected_text = NULL;

	text = book_pdf_get_selected_text (demo->bookPdf, demo->page_index,
					       demo->style,
					       &demo->doc_area);
	if (text) {
		demo->selected_text = g_utf8_normalize (text, -1, G_NORMALIZE_NFKC);
		g_free (text);
		gtk_widget_set_sensitive(demo->copy_button, TRUE);
	}
}

static void
pgd_selections_update_cursor (BookPdfRender *demo,
			      GdkCursorType      cursor_type)
{
	GdkWindow *window = gtk_widget_get_window (demo);
	GdkCursor *cursor = NULL;

	if (cursor_type == demo->cursor)
		return;

	if (cursor_type != GDK_LAST_CURSOR) {
		cursor = gdk_cursor_new_for_display (gtk_widget_get_display (demo),
						     cursor_type);
	}

	demo->cursor = cursor_type;

	gdk_window_set_cursor (window, cursor);
	gdk_display_flush (gtk_widget_get_display (demo));
	if (cursor)
		g_object_unref (cursor);
}

static gboolean
pgd_selections_render_selections (BookPdfRender *demo)
{
	PopplerRectangle doc_area;
	gdouble page_width, page_height;
	cairo_t *cr;

	if (!demo->bookPdf || demo->start.x == -1) {
		demo->selections_idle = 0;

		return FALSE;
	}

	//poppler_page_get_size (demo->page, &page_width, &page_height);
  book_pdf_chapter_get_size (demo->bookPdf, demo->page_index,  &page_width, &page_height);
	page_width *= demo->scale;
	page_height *= demo->scale;

	doc_area.x1 = demo->start.x / demo->scale;
	doc_area.y1 = demo->start.y / demo->scale;
	doc_area.x2 = demo->stop.x / demo->scale;
	doc_area.y2 = demo->stop.y / demo->scale;

	if (demo->selection_surface)
		cairo_surface_destroy (demo->selection_surface);

  demo->selection_surface=book_pdf_render_selection(demo->bookPdf, demo->page_index,(gint)page_width,(gint)page_height, &doc_area, &demo->doc_area,
				       demo->style,
				       &demo->glyph_color,
				       &demo->background_color);

	demo->doc_area = doc_area;
	gtk_widget_queue_draw (demo);

	demo->selections_idle = 0;

	return FALSE;
}

static gboolean
pgd_selections_drawing_area_draw (GtkWidget         *area,
                                  cairo_t           *cr,
                                  BookPdfRender *demo)
{
	if (!demo->surface)
		return FALSE;

	cairo_save (cr);
	cairo_set_source_surface (cr, demo->surface, 0, 0);
	cairo_paint (cr);
	cairo_restore (cr);

	if (demo->selection_surface) {
		cairo_set_source_surface (cr, demo->selection_surface, 0, 0);
		cairo_paint (cr);
	}

	return TRUE;
}

static gboolean
pgd_selections_drawing_area_button_press (GtkWidget         *area,
					  GdkEventButton    *event,
					  BookPdfRender *demo)
{
	if (!demo->bookPdf)
		return FALSE;

	if (event->button != 1)
		return FALSE;

	demo->start.x = event->x;
	demo->start.y = event->y;
	demo->stop = demo->start;

	switch (event->type) {
	case GDK_2BUTTON_PRESS:
		demo->style = POPPLER_SELECTION_WORD;
		break;
	case GDK_3BUTTON_PRESS:
		demo->style = POPPLER_SELECTION_LINE;
		break;
	default:
		demo->style = POPPLER_SELECTION_GLYPH;
	}

	pgd_selections_render_selections (demo);

	return TRUE;
}

static gboolean
pgd_selections_drawing_area_motion_notify (GtkWidget         *area,
					   GdkEventMotion    *event,
					   BookPdfRender *demo)
{
	if (!demo->bookPdf)
		return FALSE;

	if (demo->start.x != -1) {
		demo->stop.x = event->x;
		demo->stop.y = event->y;
		if (demo->selections_idle == 0) {
			demo->selections_idle =
				g_idle_add ((GSourceFunc)pgd_selections_render_selections,
					    demo);
		}
	} else {
		gboolean over_text;

		over_text = cairo_region_contains_point (demo->selection_region,
                                                         event->x / demo->scale,
                                                         event->y / demo->scale);
		pgd_selections_update_cursor (demo, over_text ? GDK_XTERM : GDK_LAST_CURSOR);
	}

	return TRUE;
}

static gboolean
pgd_selections_drawing_area_button_release (GtkWidget         *area,
					    GdkEventButton    *event,
					    BookPdfRender *demo)
{
    book_log_debug ("=============pgd_selections_drawing_area_button_release ================");
	if (!demo->bookPdf)
		return FALSE;

	if (event->button != 1)
		return FALSE;

	if (demo->start.x != -1)
		pgd_selections_update_selected_text (demo);

	demo->start.x = -1;

	if (demo->selections_idle > 0) {
		g_source_remove (demo->selections_idle);
		demo->selections_idle = 0;
	}

	return TRUE;
}

static void
pgd_selections_drawing_area_realize (GtkWidget         *area,
				     BookPdfRender *demo)
{
	GtkStyleContext *style_context = gtk_widget_get_style_context (area);
        GdkRGBA rgba;

	gtk_widget_add_events (area,
			       GDK_POINTER_MOTION_HINT_MASK |
			       GDK_BUTTON1_MOTION_MASK |
			       GDK_BUTTON_PRESS_MASK |
			       GDK_BUTTON_RELEASE_MASK);
	g_object_set (area, "has-tooltip", TRUE, NULL);

        gtk_style_context_get_color (style_context, GTK_STATE_FLAG_SELECTED, &rgba);
	gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (demo->fg_color_button), &rgba);
	gtk_style_context_get (style_context, GTK_STATE_FLAG_SELECTED, "background-color", &rgba, NULL);
	gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (demo->bg_color_button), &rgba);

  book_log_debug ("=============pgd_selections_drawing_area_realize ================");
}

static gboolean
pgd_selections_drawing_area_query_tooltip (GtkWidget         *area,
					   gint               x,
					   gint               y,
					   gboolean           keyboard_mode,
					   GtkTooltip        *tooltip,
					   BookPdfRender *demo)
{
	gboolean over_selection;
book_log_debug("选择内容：%s\n",demo->selected_text);
	if (!demo->selected_text)
		return FALSE;

	over_selection = cairo_region_contains_point (demo->selected_region,
                                                      x / demo->scale,
                                                      y / demo->scale);

	if (over_selection) {
		GdkRectangle selection_area;

		cairo_region_get_extents (demo->selected_region,
                                          (cairo_rectangle_int_t *)&selection_area);
		selection_area.x *= demo->scale;
		selection_area.y *= demo->scale;
		selection_area.width *= demo->scale;
		selection_area.height *= demo->scale;

		gtk_tooltip_set_text (tooltip, demo->selected_text);
		gtk_tooltip_set_tip_area (tooltip, &selection_area);

		return TRUE;
	}

	return FALSE;
}

static void
pgd_selections_render (
		       BookPdfRender *demo)
{
	gdouble  page_width, page_height;
	cairo_t *cr;
/*
	if (!demo->page)
    {
    demo->page = poppler_document_get_page (demo->doc, demo->page_index);
    }
*/

	if (!demo->bookPdf)
		return;

	pgd_selections_clear_selections (demo);
	pgd_selections_update_selection_region (demo);
     //   gtk_widget_set_sensitive (demo->copy_button, FALSE);

	if (demo->surface)
		cairo_surface_destroy (demo->surface);
	demo->surface = NULL;

  book_pdf_chapter_get_size (demo->bookPdf, demo->page_index, &page_width, &page_height);
	page_width *= demo->scale;
	page_height *= demo->scale;

  demo->surface=book_pdf_chapter_get_surface (demo->bookPdf,demo->page_index, page_width, page_height);

	gtk_widget_set_size_request (demo, page_width, page_height);
	gtk_widget_queue_draw (demo);
}

static void
pgd_selections_copy (GtkButton         *button,
		     BookPdfRender *demo)
{
	GtkClipboard *clipboard = gtk_clipboard_get_for_display(gdk_display_get_default(),
								GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text (clipboard, demo->selected_text, -1);
}

static void
pgd_selections_page_selector_value_changed (GtkSpinButton     *spinbutton,
					    BookPdfRender *demo)
{
	//demo->page_index = (gint)gtk_spin_button_get_value (spinbutton) - 1;

}

static void
pgd_selections_scale_selector_value_changed (GtkSpinButton     *spinbutton,
					     BookPdfRender *demo)
{
	demo->scale = gtk_spin_button_get_value (spinbutton);
}

static void
pgd_selections_fg_color_changed (GtkColorButton    *button,
				 GParamSpec        *pspec,
				 BookPdfRender *demo)
{
	GdkRGBA color;

	gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (button), &color);
	demo->glyph_color.red = CLAMP ((guint) (color.red * 65535), 0, 65535);
	demo->glyph_color.green = CLAMP ((guint) (color.green * 65535), 0, 65535);
	demo->glyph_color.blue = CLAMP ((guint) (color.blue * 65535), 0, 65535);
}

static void
pgd_selections_bg_color_changed (GtkColorButton    *button,
				 GParamSpec        *pspec,
				 BookPdfRender *demo)
{
	GdkRGBA color;

	gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (button), &color);
	demo->background_color.red = CLAMP ((guint) (color.red * 65535), 0, 65535);
	demo->background_color.green = CLAMP ((guint) (color.green * 65535), 0, 65535);
	demo->background_color.blue = CLAMP ((guint) (color.blue * 65535), 0, 65535);
}

GtkWidget *
pgd_selections_properties_selector_create (BookPdfRender *demo)
{
	GtkWidget *hbox, *vbox;
	GtkWidget *label;
	GtkWidget *page_hbox, *page_selector;
	GtkWidget *scale_hbox, *scale_selector;
	GtkWidget *rotate_hbox, *rotate_selector;
	GtkWidget *color_hbox;
	GtkWidget *button;
	gint       n_pages;
	gchar     *str;

	n_pages = poppler_document_get_n_pages (demo->doc);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show (hbox);

	page_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);

	label = gtk_label_new ("Page:");
	gtk_box_pack_start (GTK_BOX (page_hbox), label, TRUE, TRUE, 0);
	gtk_widget_show (label);

	page_selector = gtk_spin_button_new_with_range (1, n_pages, 1);
	g_signal_connect (G_OBJECT (page_selector), "value-changed",
			  G_CALLBACK (pgd_selections_page_selector_value_changed),
			  (gpointer)demo);
	gtk_box_pack_start (GTK_BOX (page_hbox), page_selector, TRUE, TRUE, 0);
	gtk_widget_show (page_selector);

	str = g_strdup_printf ("of %d", n_pages);
	label = gtk_label_new (str);
	gtk_box_pack_start (GTK_BOX (page_hbox), label, TRUE, TRUE, 0);
	gtk_widget_show (label);
	g_free (str);

	gtk_box_pack_start (GTK_BOX (hbox), page_hbox, FALSE, TRUE, 0);
	gtk_widget_show (page_hbox);

	scale_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);

	label = gtk_label_new ("Scale:");
	gtk_box_pack_start (GTK_BOX (scale_hbox), label, TRUE, TRUE, 0);
	gtk_widget_show (label);

	scale_selector = gtk_spin_button_new_with_range (0, 10.0, 0.1);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (scale_selector), 1.0);
	g_signal_connect (G_OBJECT (scale_selector), "value-changed",
			  G_CALLBACK (pgd_selections_scale_selector_value_changed),
			  (gpointer)demo);
	gtk_box_pack_start (GTK_BOX (scale_hbox), scale_selector, TRUE, TRUE, 0);
	gtk_widget_show (scale_selector);

	gtk_box_pack_start (GTK_BOX (hbox), scale_hbox, FALSE, TRUE, 0);
	gtk_widget_show (scale_hbox);

	rotate_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);

	label = gtk_label_new ("Rotate:");
	gtk_box_pack_start (GTK_BOX (rotate_hbox), label, TRUE, TRUE, 0);
	gtk_widget_show (label);

	rotate_selector = gtk_combo_box_text_new ();
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (rotate_selector), "0");
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (rotate_selector), "90");
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (rotate_selector), "180");
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (rotate_selector), "270");
	gtk_combo_box_set_active (GTK_COMBO_BOX (rotate_selector), 0);
#if 0
	g_signal_connect (G_OBJECT (rotate_selector), "changed",
			  G_CALLBACK (pgd_selections_rotate_selector_changed),
			  (gpointer)demo);
#endif
	gtk_box_pack_start (GTK_BOX (rotate_hbox), rotate_selector, TRUE, TRUE, 0);
	gtk_widget_show (rotate_selector);

	gtk_box_pack_start (GTK_BOX (hbox), rotate_hbox, FALSE, TRUE, 0);
	gtk_widget_show (rotate_hbox);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show (hbox);

	color_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);

	label = gtk_label_new ("Foreground Color:");
	gtk_box_pack_start (GTK_BOX (color_hbox), label, TRUE, TRUE, 0);
	gtk_widget_show (label);

	demo->fg_color_button = gtk_color_button_new ();
	g_signal_connect (demo->fg_color_button, "notify::color",
			  G_CALLBACK (pgd_selections_fg_color_changed),
			  (gpointer)demo);
	gtk_box_pack_start (GTK_BOX (color_hbox), demo->fg_color_button, TRUE, TRUE, 0);
	gtk_widget_show (demo->fg_color_button);

	gtk_box_pack_start (GTK_BOX (hbox), color_hbox, FALSE, TRUE, 0);
	gtk_widget_show (color_hbox);

	color_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);

	label = gtk_label_new ("Background Color:");
	gtk_box_pack_start (GTK_BOX (color_hbox), label, TRUE, TRUE, 0);
	gtk_widget_show (label);

	demo->bg_color_button = gtk_color_button_new ();
	g_signal_connect (demo->bg_color_button, "notify::color",
			  G_CALLBACK (pgd_selections_bg_color_changed),
			  (gpointer)demo);
	gtk_box_pack_start (GTK_BOX (color_hbox), demo->bg_color_button, TRUE, TRUE, 0);
	gtk_widget_show (demo->bg_color_button);

	gtk_box_pack_start (GTK_BOX (hbox), color_hbox, FALSE, TRUE, 0);
	gtk_widget_show (color_hbox);

	demo->copy_button = gtk_button_new_with_label ("Copy");
	g_signal_connect (G_OBJECT (demo->copy_button), "clicked",
			  G_CALLBACK (pgd_selections_copy),
			  (gpointer)demo);
	gtk_box_pack_end (GTK_BOX (hbox), demo->copy_button, FALSE, TRUE, 0);
	gtk_widget_set_sensitive(demo->copy_button, FALSE);
	gtk_widget_show (demo->copy_button);

	button = gtk_button_new_with_label ("Render");
	//g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (pgd_selections_render),  (gpointer)demo);
	gtk_box_pack_end (GTK_BOX (hbox), button, FALSE, TRUE, 0);
	gtk_widget_show (button);

	return vbox;
}

void book_pdf_render_default(BookPdfRender *demo)
{
	GdkRGBA color;
gdk_rgba_parse (&color, "#C8856D");
	demo->background_color.red = CLAMP ((guint) (color.red * 65535), 0, 65535);
	demo->background_color.green = CLAMP ((guint) (color.green * 65535), 0, 65535);
	demo->background_color.blue = CLAMP ((guint) (color.blue * 65535), 0, 65535);
}

GtkWidget *book_pdf_render_new(BookPdf *bookPdf,gint pageIndex){
  BookPdfRender *demo;
 GError *pdferror = NULL;
  demo=g_object_new(BOOK_TYPE_PDF_RENDER,NULL);
	//demo->doc = g_object_ref (document);
  demo->bookPdf=bookPdf;
	demo->scale = 1.0;
	demo->cursor = GDK_LAST_CURSOR;
demo->page_index=pageIndex;
  book_pdf_render_default(demo);

	pgd_selections_clear_selections (demo);
	g_signal_connect (demo, "realize",
			  G_CALLBACK (pgd_selections_drawing_area_realize),
			  (gpointer)demo);
	g_signal_connect (demo, "draw",
			  G_CALLBACK (pgd_selections_drawing_area_draw),
			  (gpointer)demo);
	g_signal_connect (demo, "button_press_event",
			  G_CALLBACK (pgd_selections_drawing_area_button_press),
			  (gpointer)demo);
	g_signal_connect (demo, "motion_notify_event",
			  G_CALLBACK (pgd_selections_drawing_area_motion_notify),
			  (gpointer)demo);
	g_signal_connect (demo, "button_release_event",
			  G_CALLBACK (pgd_selections_drawing_area_button_release),
			  (gpointer)demo);
	g_signal_connect (demo, "query_tooltip",
			  G_CALLBACK (pgd_selections_drawing_area_query_tooltip),
			  (gpointer)demo);
//g_thread_create(pgd_selections_render,demo,FALSE,NULL);
 pgd_selections_render(demo);
  gtk_widget_show (demo);


  //  demo;
  return demo;
}

void book_pdf_render_set_scale(BookPdfRender *render,gdouble scale){
  render->scale=scale;
  pgd_selections_render(render);
  //g_thread_create(pgd_selections_render,render,FALSE,NULL);

}
