/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Novell, Inc.
 *
 * Athena is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Athena is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Anders Carlsson <andersca@imendio.com>
 *
 */

#include <config.h>
#include "athena-search-bar.h"

#include <libathena-private/athena-icon-info.h>

#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

struct AthenaSearchBarDetails {
	GtkWidget *entry;
	gboolean entry_borrowed;
};

enum {
       ACTIVATE,
       CANCEL,
       LAST_SIGNAL
}; 

static guint signals[LAST_SIGNAL];

G_DEFINE_TYPE (AthenaSearchBar, athena_search_bar, GTK_TYPE_BOX);

static gboolean
athena_search_bar_draw (GtkWidget *widget,
			  cairo_t *cr)
{
	GtkStyleContext *context;

	context = gtk_widget_get_style_context (widget);

	gtk_style_context_save (context);
	gtk_style_context_add_class (context, GTK_STYLE_CLASS_INFO);

	gtk_render_background (context, cr, 0, 0,
			       gtk_widget_get_allocated_width (widget),
			       gtk_widget_get_allocated_height (widget));

	gtk_render_frame (context, cr, 0, 0,
			  gtk_widget_get_allocated_width (widget),
			  gtk_widget_get_allocated_height (widget));

	gtk_style_context_restore (context);

	GTK_WIDGET_CLASS (athena_search_bar_parent_class)->draw (widget, cr);

	return FALSE;
}

static void
athena_search_bar_class_init (AthenaSearchBarClass *class)
{
	GtkBindingSet *binding_set;
	GtkWidgetClass *wclass;

	wclass = GTK_WIDGET_CLASS (class);
	wclass->draw = athena_search_bar_draw;

	signals[ACTIVATE] =
		g_signal_new ("activate",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (AthenaSearchBarClass, activate),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);

	signals[CANCEL] =
		g_signal_new ("cancel",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (AthenaSearchBarClass, cancel),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);

	binding_set = gtk_binding_set_by_class (class);
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_Escape, 0, "cancel", 0);

	g_type_class_add_private (class, sizeof (AthenaSearchBarDetails));
}

static gboolean
entry_has_text (AthenaSearchBar *bar)
{
       const char *text;

       text = gtk_entry_get_text (GTK_ENTRY (bar->details->entry));

       return text != NULL && text[0] != '\0';
}

static void
entry_icon_release_cb (GtkEntry *entry,
		       GtkEntryIconPosition position,
		       GdkEvent *event,
		       AthenaSearchBar *bar)
{
	g_signal_emit_by_name (entry, "activate", 0);
}

static void
entry_activate_cb (GtkWidget *entry, AthenaSearchBar *bar)
{
       if (entry_has_text (bar) && !bar->details->entry_borrowed) {
               g_signal_emit (bar, signals[ACTIVATE], 0);
       }
}

static void
athena_search_bar_init (AthenaSearchBar *bar)
{
	GtkWidget *label;
	GtkWidget *align;

	bar->details =
		G_TYPE_INSTANCE_GET_PRIVATE (bar, ATHENA_TYPE_SEARCH_BAR,
					     AthenaSearchBarDetails);

	gtk_widget_set_redraw_on_allocate (GTK_WIDGET (bar), TRUE);

	label = gtk_label_new (_("Search:"));
	gtk_style_context_add_class (gtk_widget_get_style_context (label),
				     "athena-cluebar-label");
	gtk_widget_show (label);

	gtk_box_pack_start (GTK_BOX (bar), label, FALSE, FALSE, 0);

	g_object_set (label,
		      "margin-left", 6,
		      NULL);

	align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (align),
				   6, 6, 0, 6);
	gtk_box_pack_start (GTK_BOX (bar), align, TRUE, TRUE, 0);
	gtk_widget_show (align);

	bar->details->entry = gtk_entry_new ();
	gtk_entry_set_icon_from_icon_name (GTK_ENTRY (bar->details->entry),
					   GTK_ENTRY_ICON_SECONDARY,
					   "edit-find");
	gtk_container_add (GTK_CONTAINER (align), bar->details->entry);

	g_signal_connect (bar->details->entry, "activate",
			  G_CALLBACK (entry_activate_cb), bar);
	g_signal_connect (bar->details->entry, "icon-release",
			  G_CALLBACK (entry_icon_release_cb), bar);

	gtk_widget_show (bar->details->entry);
}

GtkWidget *
athena_search_bar_get_entry (AthenaSearchBar *bar)
{
	return bar->details->entry;
}

GtkWidget *
athena_search_bar_borrow_entry (AthenaSearchBar *bar)
{
	GtkBindingSet *binding_set;
	
	bar->details->entry_borrowed = TRUE;

	binding_set = gtk_binding_set_by_class (G_OBJECT_GET_CLASS (bar));
	gtk_binding_entry_remove (binding_set, GDK_KEY_Escape, 0);
	return bar->details->entry;
}

void
athena_search_bar_return_entry (AthenaSearchBar *bar)
{
	GtkBindingSet *binding_set;
	
	bar->details->entry_borrowed = FALSE;
	
	binding_set = gtk_binding_set_by_class (G_OBJECT_GET_CLASS (bar));
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_Escape, 0, "cancel", 0);
}

GtkWidget *
athena_search_bar_new (void)
{
	GtkWidget *bar;

	bar = g_object_new (ATHENA_TYPE_SEARCH_BAR,
			    "orientation", GTK_ORIENTATION_HORIZONTAL,
			    "spacing", 6,
			    NULL);

	return bar;
}

AthenaQuery *
athena_search_bar_get_query (AthenaSearchBar *bar)
{
	const char *query_text;
	AthenaQuery *query;

	query_text = gtk_entry_get_text (GTK_ENTRY (bar->details->entry));

	/* Empty string is a NULL query */
	if (query_text && query_text[0] == '\0') {
		return NULL;
	}
	
	query = athena_query_new ();
	athena_query_set_text (query, query_text);

	return query;
}

void
athena_search_bar_grab_focus (AthenaSearchBar *bar)
{
	gtk_widget_grab_focus (bar->details->entry);
}

void
athena_search_bar_clear (AthenaSearchBar *bar)
{
	gtk_entry_set_text (GTK_ENTRY (bar->details->entry), "");
}
