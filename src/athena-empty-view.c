/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-empty-view.c - implementation of empty view of directory.

   Copyright (C) 2006 Free Software Foundation, Inc.
   
   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Authors: Christian Neumair <chris@gnome-de.org>
*/

#include <config.h>

#include "athena-empty-view.h"

#include "athena-view.h"
#include "athena-view-factory.h"

#include <string.h>
#include <libathena-private/athena-file-utilities.h>
#include <eel/eel-vfs-extensions.h>

struct AthenaEmptyViewDetails {
	int number_of_files;
};

static GList *athena_empty_view_get_selection                   (AthenaView   *view);
static GList *athena_empty_view_get_selection_for_file_transfer (AthenaView   *view);
static void   athena_empty_view_scroll_to_file                  (AthenaView      *view,
								   const char        *uri);

G_DEFINE_TYPE (AthenaEmptyView, athena_empty_view, ATHENA_TYPE_VIEW)

static void
athena_empty_view_add_file (AthenaView *view, AthenaFile *file, AthenaDirectory *directory)
{
	static GTimer *timer = NULL;
	static gdouble cumu = 0, elaps;
	ATHENA_EMPTY_VIEW (view)->details->number_of_files++;
	GdkPixbuf *icon;

	if (!timer) timer = g_timer_new ();

	g_timer_start (timer);
	icon = athena_file_get_icon_pixbuf (file, athena_get_icon_size_for_zoom_level (ATHENA_ZOOM_LEVEL_STANDARD), TRUE, 0);

	elaps = g_timer_elapsed (timer, NULL);
	g_timer_stop (timer);

	g_object_unref (icon);
	
	cumu += elaps;
	g_message ("entire loading: %.3f, cumulative %.3f", elaps, cumu);
}


static void
athena_empty_view_begin_loading (AthenaView *view)
{
}

static void
athena_empty_view_clear (AthenaView *view)
{
}


static void
athena_empty_view_file_changed (AthenaView *view, AthenaFile *file, AthenaDirectory *directory)
{
}

static GList *
athena_empty_view_get_selection (AthenaView *view)
{
	return NULL;
}


static GList *
athena_empty_view_get_selection_for_file_transfer (AthenaView *view)
{
	return NULL;
}

static guint
athena_empty_view_get_item_count (AthenaView *view)
{
	return ATHENA_EMPTY_VIEW (view)->details->number_of_files;
}

static gboolean
athena_empty_view_is_empty (AthenaView *view)
{
	return ATHENA_EMPTY_VIEW (view)->details->number_of_files == 0;
}

static void
athena_empty_view_end_file_changes (AthenaView *view)
{
}

static void
athena_empty_view_remove_file (AthenaView *view, AthenaFile *file, AthenaDirectory *directory)
{
	ATHENA_EMPTY_VIEW (view)->details->number_of_files--;
	g_assert (ATHENA_EMPTY_VIEW (view)->details->number_of_files >= 0);
}

static void
athena_empty_view_set_selection (AthenaView *view, GList *selection)
{
	athena_view_notify_selection_changed (view);
}

static void
athena_empty_view_select_all (AthenaView *view)
{
}

static void
athena_empty_view_reveal_selection (AthenaView *view)
{
}

static void
athena_empty_view_merge_menus (AthenaView *view)
{
	ATHENA_VIEW_CLASS (athena_empty_view_parent_class)->merge_menus (view);
}

static void
athena_empty_view_update_menus (AthenaView *view)
{
	ATHENA_VIEW_CLASS (athena_empty_view_parent_class)->update_menus (view);
}

/* Reset sort criteria and zoom level to match defaults */
static void
athena_empty_view_reset_to_defaults (AthenaView *view)
{
}

static void
athena_empty_view_bump_zoom_level (AthenaView *view, int zoom_increment)
{
}

static AthenaZoomLevel
athena_empty_view_get_zoom_level (AthenaView *view)
{
	return ATHENA_ZOOM_LEVEL_STANDARD;
}

static void
athena_empty_view_zoom_to_level (AthenaView *view,
			    AthenaZoomLevel zoom_level)
{
}

static void
athena_empty_view_restore_default_zoom_level (AthenaView *view)
{
}

static gboolean 
athena_empty_view_can_zoom_in (AthenaView *view) 
{
	return FALSE;
}

static gboolean 
athena_empty_view_can_zoom_out (AthenaView *view) 
{
	return FALSE;
}

static void
athena_empty_view_start_renaming_file (AthenaView *view,
				  AthenaFile *file,
				  gboolean select_all)
{
}

static void
athena_empty_view_click_policy_changed (AthenaView *directory_view)
{
}


static int
athena_empty_view_compare_files (AthenaView *view, AthenaFile *file1, AthenaFile *file2)
{
	if (file1 < file2) {
		return -1;
	}

	if (file1 > file2) {
		return +1;
	}

	return 0;
}

static gboolean
athena_empty_view_using_manual_layout (AthenaView *view)
{
	return FALSE;
}

static void
athena_empty_view_end_loading (AthenaView *view,
			   gboolean all_files_seen)
{
}

static char *
athena_empty_view_get_first_visible_file (AthenaView *view)
{
	return NULL;
}

static void
athena_empty_view_scroll_to_file (AthenaView *view,
			      const char *uri)
{
}

static void
athena_empty_view_sort_directories_first_changed (AthenaView *view)
{
}

static const char *
athena_empty_view_get_id (AthenaView *view)
{
	return ATHENA_EMPTY_VIEW_ID;
}

static void
athena_empty_view_class_init (AthenaEmptyViewClass *class)
{
	AthenaViewClass *athena_view_class;

	g_type_class_add_private (class, sizeof (AthenaEmptyViewDetails));

	athena_view_class = ATHENA_VIEW_CLASS (class);

	athena_view_class->add_file = athena_empty_view_add_file;
	athena_view_class->begin_loading = athena_empty_view_begin_loading;
	athena_view_class->bump_zoom_level = athena_empty_view_bump_zoom_level;
	athena_view_class->can_zoom_in = athena_empty_view_can_zoom_in;
	athena_view_class->can_zoom_out = athena_empty_view_can_zoom_out;
        athena_view_class->click_policy_changed = athena_empty_view_click_policy_changed;
	athena_view_class->clear = athena_empty_view_clear;
	athena_view_class->file_changed = athena_empty_view_file_changed;
	athena_view_class->get_selection = athena_empty_view_get_selection;
	athena_view_class->get_selection_for_file_transfer = athena_empty_view_get_selection_for_file_transfer;
	athena_view_class->get_item_count = athena_empty_view_get_item_count;
	athena_view_class->is_empty = athena_empty_view_is_empty;
	athena_view_class->remove_file = athena_empty_view_remove_file;
	athena_view_class->merge_menus = athena_empty_view_merge_menus;
	athena_view_class->update_menus = athena_empty_view_update_menus;
	athena_view_class->reset_to_defaults = athena_empty_view_reset_to_defaults;
	athena_view_class->restore_default_zoom_level = athena_empty_view_restore_default_zoom_level;
	athena_view_class->reveal_selection = athena_empty_view_reveal_selection;
	athena_view_class->select_all = athena_empty_view_select_all;
	athena_view_class->set_selection = athena_empty_view_set_selection;
	athena_view_class->compare_files = athena_empty_view_compare_files;
	athena_view_class->sort_directories_first_changed = athena_empty_view_sort_directories_first_changed;
	athena_view_class->start_renaming_file = athena_empty_view_start_renaming_file;
	athena_view_class->get_zoom_level = athena_empty_view_get_zoom_level;
	athena_view_class->zoom_to_level = athena_empty_view_zoom_to_level;
	athena_view_class->end_file_changes = athena_empty_view_end_file_changes;
	athena_view_class->using_manual_layout = athena_empty_view_using_manual_layout;
	athena_view_class->end_loading = athena_empty_view_end_loading;
	athena_view_class->get_view_id = athena_empty_view_get_id;
	athena_view_class->get_first_visible_file = athena_empty_view_get_first_visible_file;
	athena_view_class->scroll_to_file = athena_empty_view_scroll_to_file;
}

static void
athena_empty_view_init (AthenaEmptyView *empty_view)
{
	empty_view->details = G_TYPE_INSTANCE_GET_PRIVATE (empty_view, ATHENA_TYPE_EMPTY_VIEW,
							   AthenaEmptyViewDetails);
}

static AthenaView *
athena_empty_view_create (AthenaWindowSlot *slot)
{
	AthenaEmptyView *view;

	g_assert (ATHENA_IS_WINDOW_SLOT (slot));

	view = g_object_new (ATHENA_TYPE_EMPTY_VIEW,
			     "window-slot", slot,
			     NULL);

	return ATHENA_VIEW (view);
}

static gboolean
athena_empty_view_supports_uri (const char *uri,
				  GFileType file_type,
				  const char *mime_type)
{
	if (file_type == G_FILE_TYPE_DIRECTORY) {
		return TRUE;
	}
	if (strcmp (mime_type, ATHENA_SAVED_SEARCH_MIMETYPE) == 0){
		return TRUE;
	}
	if (g_str_has_prefix (uri, "trash:")) {
		return TRUE;
	}
	if (g_str_has_prefix (uri, EEL_SEARCH_URI)) {
		return TRUE;
	}

	return FALSE;
}

static AthenaViewInfo athena_empty_view = {
	ATHENA_EMPTY_VIEW_ID,
	"Empty",
	"Empty View",
	"_Empty View",
	"The empty view encountered an error.",
	"Display this location with the empty view.",
	athena_empty_view_create,
	athena_empty_view_supports_uri
};

void
athena_empty_view_register (void)
{
	athena_empty_view.id = athena_empty_view.id;
	athena_empty_view.view_combo_label = athena_empty_view.view_combo_label;
	athena_empty_view.view_menu_label_with_mnemonic = athena_empty_view.view_menu_label_with_mnemonic;
	athena_empty_view.error_label = athena_empty_view.error_label;
	athena_empty_view.display_location_label = athena_empty_view.display_location_label;

	athena_view_factory_register (&athena_empty_view);
}
