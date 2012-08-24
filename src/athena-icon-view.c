/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-icon-view.c - implementation of icon view of directory.

   Copyright (C) 2000, 2001 Eazel, Inc.

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

   Authors: John Sullivan <sullivan@eazel.com>
*/

#include <config.h>

#include "athena-icon-view.h"

#include "athena-actions.h"
#include "athena-icon-view-container.h"
#include "athena-desktop-icon-view.h"
#include "athena-error-reporting.h"
#include "athena-view-dnd.h"
#include "athena-view-factory.h"

#include <stdlib.h>
#include <eel/eel-vfs-extensions.h>
#include <errno.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <libathena-private/athena-clipboard-monitor.h>
#include <libathena-private/athena-directory.h>
#include <libathena-private/athena-dnd.h>
#include <libathena-private/athena-file-dnd.h>
#include <libathena-private/athena-file-utilities.h>
#include <libathena-private/athena-ui-utilities.h>
#include <libathena-private/athena-global-preferences.h>
#include <libathena-private/athena-icon-container.h>
#include <libathena-private/athena-icon-dnd.h>
#include <libathena-private/athena-link.h>
#include <libathena-private/athena-metadata.h>
#include <libathena-private/athena-clipboard.h>
#include <libathena-private/athena-desktop-icon-file.h>

#define DEBUG_FLAG ATHENA_DEBUG_ICON_VIEW
#include <libathena-private/athena-debug.h>

#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define POPUP_PATH_ICON_APPEARANCE		"/selection/Icon Appearance Items"

enum 
{
	PROP_COMPACT = 1,
	PROP_SUPPORTS_AUTO_LAYOUT,
	PROP_SUPPORTS_SCALING,
	PROP_SUPPORTS_KEEP_ALIGNED,
	PROP_SUPPORTS_LABELS_BESIDE_ICONS,
	NUM_PROPERTIES
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

typedef struct {
	const AthenaFileSortType sort_type;
	const char *metadata_text;
	const char *action;
	const char *menu_label;
	const char *menu_hint;
} SortCriterion;

typedef enum {
	MENU_ITEM_TYPE_STANDARD,
	MENU_ITEM_TYPE_CHECK,
	MENU_ITEM_TYPE_RADIO,
	MENU_ITEM_TYPE_TREE
} MenuItemType;

struct AthenaIconViewDetails
{
	GList *icons_not_positioned;

	guint react_to_icon_change_idle_id;

	const SortCriterion *sort;
	gboolean sort_reversed;

	GtkActionGroup *icon_action_group;
	guint icon_merge_id;
	
	gboolean filter_by_screen;
	int num_screens;

	gboolean compact;

	gulong clipboard_handler_id;

	GtkWidget *icon_container;

	gboolean supports_auto_layout;
	gboolean supports_scaling;
	gboolean supports_keep_aligned;
	gboolean supports_labels_beside_icons;
};


/* Note that the first item in this list is the default sort,
 * and that the items show up in the menu in the order they
 * appear in this list.
 */
static const SortCriterion sort_criteria[] = {
	{
		ATHENA_FILE_SORT_BY_DISPLAY_NAME,
		"name",
		"Sort by Name",
		N_("by _Name"),
		N_("Keep icons sorted by name in rows")
	},
	{
		ATHENA_FILE_SORT_BY_SIZE,
		"size",
		"Sort by Size",
		N_("by _Size"),
		N_("Keep icons sorted by size in rows")
	},
	{
		ATHENA_FILE_SORT_BY_TYPE,
		"type",
		"Sort by Type",
		N_("by _Type"),
		N_("Keep icons sorted by type in rows")
	},
	{
		ATHENA_FILE_SORT_BY_MTIME,
		"modification date",
		"Sort by Modification Date",
		N_("by Modification _Date"),
		N_("Keep icons sorted by modification date in rows")
	},
	{
		ATHENA_FILE_SORT_BY_TRASHED_TIME,
		"trashed",
		"Sort by Trash Time",
		N_("by T_rash Time"),
		N_("Keep icons sorted by trash time in rows")
	}
};

static void                 athena_icon_view_set_directory_sort_by        (AthenaIconView           *icon_view,
									     AthenaFile         *file,
									     const char           *sort_by);
static void                 athena_icon_view_set_zoom_level               (AthenaIconView           *view,
									     AthenaZoomLevel     new_level,
									     gboolean              always_emit);
static void                 athena_icon_view_update_click_mode            (AthenaIconView           *icon_view);
static gboolean             athena_icon_view_supports_scaling	      (AthenaIconView           *icon_view);
static void                 athena_icon_view_reveal_selection       (AthenaView               *view);
static const SortCriterion *get_sort_criterion_by_sort_type           (AthenaFileSortType  sort_type);
static void                 set_sort_criterion_by_sort_type           (AthenaIconView           *icon_view,
								       AthenaFileSortType  sort_type);
static gboolean             set_sort_reversed                         (AthenaIconView     *icon_view,
								       gboolean              new_value,
								       gboolean              set_metadata);
static void                 switch_to_manual_layout                   (AthenaIconView     *view);
static void                 update_layout_menus                       (AthenaIconView     *view);
static AthenaFileSortType get_default_sort_order                    (AthenaFile         *file,
								       gboolean             *reversed);
static void                 athena_icon_view_clear                  (AthenaView         *view);

G_DEFINE_TYPE (AthenaIconView, athena_icon_view, ATHENA_TYPE_VIEW);

static void
athena_icon_view_destroy (GtkWidget *object)
{
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (object);

	athena_icon_view_clear (ATHENA_VIEW (object));

        if (icon_view->details->react_to_icon_change_idle_id != 0) {
                g_source_remove (icon_view->details->react_to_icon_change_idle_id);
		icon_view->details->react_to_icon_change_idle_id = 0;
        }

	if (icon_view->details->clipboard_handler_id != 0) {
		g_signal_handler_disconnect (athena_clipboard_monitor_get (),
					     icon_view->details->clipboard_handler_id);
		icon_view->details->clipboard_handler_id = 0;
	}

	if (icon_view->details->icons_not_positioned) {
		athena_file_list_free (icon_view->details->icons_not_positioned);
		icon_view->details->icons_not_positioned = NULL;
	}

	GTK_WIDGET_CLASS (athena_icon_view_parent_class)->destroy (object);
}

static AthenaIconContainer *
get_icon_container (AthenaIconView *icon_view)
{
	return ATHENA_ICON_CONTAINER (icon_view->details->icon_container);
}

AthenaIconContainer *
athena_icon_view_get_icon_container (AthenaIconView *icon_view)
{
	return get_icon_container (icon_view);
}

static gboolean
athena_icon_view_supports_manual_layout (AthenaIconView *view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), FALSE);

	return !athena_icon_view_is_compact (view);
}

static gboolean
get_stored_icon_position_callback (AthenaIconContainer *container,
				   AthenaFile *file,
				   AthenaIconPosition *position,
				   AthenaIconView *icon_view)
{
	char *position_string, *scale_string;
	gboolean position_good;
	char c;

	g_assert (ATHENA_IS_ICON_CONTAINER (container));
	g_assert (ATHENA_IS_FILE (file));
	g_assert (position != NULL);
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));

	if (!athena_icon_view_supports_manual_layout (icon_view)) {
		return FALSE;
	}

	/* Get the current position of this icon from the metadata. */
	position_string = athena_file_get_metadata
		(file, ATHENA_METADATA_KEY_ICON_POSITION, "");
	position_good = sscanf
		(position_string, " %d , %d %c",
		 &position->x, &position->y, &c) == 2;
	g_free (position_string);

	/* If it is the desktop directory, maybe the gnome-libs metadata has information about it */

	/* Disable scaling if not on the desktop */
	if (athena_icon_view_supports_scaling (icon_view)) {
		/* Get the scale of the icon from the metadata. */
		scale_string = athena_file_get_metadata
			(file, ATHENA_METADATA_KEY_ICON_SCALE, "1");
		position->scale = g_ascii_strtod (scale_string, NULL);
		if (errno != 0) {
			position->scale = 1.0;
		}

		g_free (scale_string);
	} else {
		position->scale = 1.0;
	}
	
	return position_good;
}

static void
real_set_sort_criterion (AthenaIconView *icon_view,
                         const SortCriterion *sort,
                         gboolean clear,
			 gboolean set_metadata)
{
	AthenaFile *file;

	file = athena_view_get_directory_as_file (ATHENA_VIEW (icon_view));

	if (clear) {
		athena_file_set_metadata (file,
					    ATHENA_METADATA_KEY_ICON_VIEW_SORT_BY, NULL, NULL);
		athena_file_set_metadata (file,
					    ATHENA_METADATA_KEY_ICON_VIEW_SORT_REVERSED, NULL, NULL);
		icon_view->details->sort =
			get_sort_criterion_by_sort_type	(get_default_sort_order
							 (file, &icon_view->details->sort_reversed));
	} else if (set_metadata) {
		/* Store the new sort setting. */
		athena_icon_view_set_directory_sort_by (icon_view,
						    file,
						    sort->metadata_text);
	}

	/* Update the layout menus to match the new sort setting. */
	update_layout_menus (icon_view);
}

static void
set_sort_criterion (AthenaIconView *icon_view,
		    const SortCriterion *sort,
		    gboolean set_metadata)
{
	if (sort == NULL ||
	    icon_view->details->sort == sort) {
		return;
	}

	icon_view->details->sort = sort;

        real_set_sort_criterion (icon_view, sort, FALSE, set_metadata);
}

static void
clear_sort_criterion (AthenaIconView *icon_view)
{
	real_set_sort_criterion (icon_view, NULL, TRUE, TRUE);
}

static void
action_stretch_callback (GtkAction *action,
			 gpointer callback_data)
{
	g_assert (ATHENA_IS_ICON_VIEW (callback_data));

	athena_icon_container_show_stretch_handles
		(get_icon_container (ATHENA_ICON_VIEW (callback_data)));
}

static void
action_unstretch_callback (GtkAction *action,
			   gpointer callback_data)
{
	g_assert (ATHENA_IS_ICON_VIEW (callback_data));

	athena_icon_container_unstretch
		(get_icon_container (ATHENA_ICON_VIEW (callback_data)));
}

static void
athena_icon_view_clean_up (AthenaIconView *icon_view)
{
	AthenaIconContainer *icon_container;
	gboolean saved_sort_reversed;

	icon_container = get_icon_container (icon_view);

	/* Hardwire Clean Up to always be by name, in forward order */
	saved_sort_reversed = icon_view->details->sort_reversed;
	
	set_sort_reversed (icon_view, FALSE, FALSE);
	set_sort_criterion (icon_view, &sort_criteria[0], FALSE);

	athena_icon_container_sort (icon_container);
	athena_icon_container_freeze_icon_positions (icon_container);

	set_sort_reversed (icon_view, saved_sort_reversed, FALSE);
}

static void
action_clean_up_callback (GtkAction *action, gpointer callback_data)
{
	athena_icon_view_clean_up (ATHENA_ICON_VIEW (callback_data));
}

static gboolean
athena_icon_view_using_auto_layout (AthenaIconView *icon_view)
{
	return athena_icon_container_is_auto_layout 
		(get_icon_container (icon_view));
}

static void
action_sort_radio_callback (GtkAction *action,
			    GtkRadioAction *current,
			    AthenaIconView *view)
{
	AthenaFileSortType sort_type;
	
	sort_type = gtk_radio_action_get_current_value (current);
	
	/* Note that id might be a toggle item.
	 * Ignore non-sort ids so that they don't cause sorting.
	 */
	if (sort_type == ATHENA_FILE_SORT_NONE) {
		switch_to_manual_layout (view);
	} else {
		set_sort_criterion_by_sort_type (view, sort_type);
	}
}

static void
list_covers (AthenaIconData *data, gpointer callback_data)
{
	GSList **file_list;

	file_list = callback_data;

	*file_list = g_slist_prepend (*file_list, data);
}

static void
unref_cover (AthenaIconData *data, gpointer callback_data)
{
	athena_file_unref (ATHENA_FILE (data));
}

static void
athena_icon_view_clear (AthenaView *view)
{
	AthenaIconContainer *icon_container;
	GSList *file_list;
	
	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));

	icon_container = get_icon_container (ATHENA_ICON_VIEW (view));
	if (!icon_container)
		return;

	/* Clear away the existing icons. */
	file_list = NULL;
	athena_icon_container_for_each (icon_container, list_covers, &file_list);
	athena_icon_container_clear (icon_container);
	g_slist_foreach (file_list, (GFunc)unref_cover, NULL);
	g_slist_free (file_list);
}

static gboolean
should_show_file_on_screen (AthenaView *view, AthenaFile *file)
{
	char *screen_string;
	int screen_num;
	AthenaIconView *icon_view;
	GdkScreen *screen;

	icon_view = ATHENA_ICON_VIEW (view);

	if (!athena_view_should_show_file (view, file)) {
		return FALSE;
	}
	
	/* Get the screen for this icon from the metadata. */
	screen_string = athena_file_get_metadata
		(file, ATHENA_METADATA_KEY_SCREEN, "0");
	screen_num = atoi (screen_string);
	g_free (screen_string);
	screen = gtk_widget_get_screen (GTK_WIDGET (view));

	if (screen_num != gdk_screen_get_number (screen) &&
	    (screen_num < icon_view->details->num_screens ||
	     gdk_screen_get_number (screen) > 0)) {
		return FALSE;
	}

	return TRUE;
}

static void
athena_icon_view_remove_file (AthenaView *view, AthenaFile *file, AthenaDirectory *directory)
{
	AthenaIconView *icon_view;

	/* This used to assert that 'directory == athena_view_get_model (view)', but that
	 * resulted in a lot of crash reports (bug #352592). I don't see how that trace happens.
	 * It seems that somehow we get a files_changed event sent to the view from a directory
	 * that isn't the model, but the code disables the monitor and signal callback handlers when
	 * changing directories. Maybe we can get some more information when this happens.
	 * Further discussion in bug #368178.
	 */
	if (directory != athena_view_get_model (view)) {
		char *file_uri, *dir_uri, *model_uri;
		file_uri = athena_file_get_uri (file);
		dir_uri = athena_directory_get_uri (directory);
		model_uri = athena_directory_get_uri (athena_view_get_model (view));
		g_warning ("athena_icon_view_remove_file() - directory not icon view model, shouldn't happen.\n"
			   "file: %p:%s, dir: %p:%s, model: %p:%s, view loading: %d\n"
			   "If you see this, please add this info to http://bugzilla.gnome.org/show_bug.cgi?id=368178",
			   file, file_uri, directory, dir_uri, athena_view_get_model (view), model_uri, athena_view_get_loading (view));
		g_free (file_uri);
		g_free (dir_uri);
		g_free (model_uri);
	}
	
	icon_view = ATHENA_ICON_VIEW (view);

	if (athena_icon_container_remove (get_icon_container (icon_view),
					    ATHENA_ICON_CONTAINER_ICON_DATA (file))) {
		athena_file_unref (file);
	}
}

static void
athena_icon_view_add_file (AthenaView *view, AthenaFile *file, AthenaDirectory *directory)
{
	AthenaIconView *icon_view;
	AthenaIconContainer *icon_container;

	g_assert (directory == athena_view_get_model (view));
	
	icon_view = ATHENA_ICON_VIEW (view);
	icon_container = get_icon_container (icon_view);

	if (icon_view->details->filter_by_screen &&
	    !should_show_file_on_screen (view, file)) {
		return;
	}

	/* Reset scroll region for the first icon added when loading a directory. */
	if (athena_view_get_loading (view) && athena_icon_container_is_empty (icon_container)) {
		athena_icon_container_reset_scroll_region (icon_container);
	}

	if (athena_icon_container_add (icon_container,
					 ATHENA_ICON_CONTAINER_ICON_DATA (file))) {
		athena_file_ref (file);
	}
}

static void
athena_icon_view_file_changed (AthenaView *view, AthenaFile *file, AthenaDirectory *directory)
{
	AthenaIconView *icon_view;

	g_assert (directory == athena_view_get_model (view));
	
	g_return_if_fail (view != NULL);
	icon_view = ATHENA_ICON_VIEW (view);

	if (!icon_view->details->filter_by_screen) {
		athena_icon_container_request_update
			(get_icon_container (icon_view),
			 ATHENA_ICON_CONTAINER_ICON_DATA (file));
		return;
	}
	
	if (!should_show_file_on_screen (view, file)) {
		athena_icon_view_remove_file (view, file, directory);
	} else {

		athena_icon_container_request_update
			(get_icon_container (icon_view),
			 ATHENA_ICON_CONTAINER_ICON_DATA (file));
	}
}

static gboolean
athena_icon_view_supports_auto_layout (AthenaIconView *view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), FALSE);

	return view->details->supports_auto_layout;
}

static gboolean
athena_icon_view_supports_scaling (AthenaIconView *view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), FALSE);

	return view->details->supports_scaling;
}

static gboolean
athena_icon_view_supports_keep_aligned (AthenaIconView *view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), FALSE);

	return view->details->supports_keep_aligned;
}

static gboolean
athena_icon_view_supports_labels_beside_icons (AthenaIconView *view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), FALSE);

	return view->details->supports_labels_beside_icons;
}

static void
update_layout_menus (AthenaIconView *view)
{
	gboolean is_auto_layout;
	GtkAction *action;
	const char *action_name;
	AthenaFile *file;

	if (view->details->icon_action_group == NULL) {
		return;
	}

	is_auto_layout = athena_icon_view_using_auto_layout (view);
	file = athena_view_get_directory_as_file (ATHENA_VIEW (view));

	if (athena_icon_view_supports_auto_layout (view)) {
		/* Mark sort criterion. */
		action_name = is_auto_layout ? view->details->sort->action : ATHENA_ACTION_MANUAL_LAYOUT;
		action = gtk_action_group_get_action (view->details->icon_action_group,
						      action_name);
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

		action = gtk_action_group_get_action (view->details->icon_action_group,
						      ATHENA_ACTION_REVERSED_ORDER);
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
					      view->details->sort_reversed);
		gtk_action_set_sensitive (action, is_auto_layout);

		action = gtk_action_group_get_action (view->details->icon_action_group,
		                                      ATHENA_ACTION_SORT_TRASH_TIME);

		if (file != NULL && athena_file_is_in_trash (file)) {
			gtk_action_set_visible (action, TRUE);
		} else {
			gtk_action_set_visible (action, FALSE);
		}
	}

	action = gtk_action_group_get_action (view->details->icon_action_group,
					      ATHENA_ACTION_MANUAL_LAYOUT);
	gtk_action_set_visible (action,
				athena_icon_view_supports_manual_layout (view));

	/* Clean Up is only relevant for manual layout */
	action = gtk_action_group_get_action (view->details->icon_action_group,
					      ATHENA_ACTION_CLEAN_UP);
	gtk_action_set_sensitive (action, !is_auto_layout);	

	if (ATHENA_IS_DESKTOP_ICON_VIEW (view)) {
		gtk_action_set_label (action, _("_Organize Desktop by Name"));
	}

	action = gtk_action_group_get_action (view->details->icon_action_group,
					      ATHENA_ACTION_KEEP_ALIGNED);
	gtk_action_set_visible (action,
				athena_icon_view_supports_keep_aligned (view));
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
				      athena_icon_container_is_keep_aligned (get_icon_container (view)));
	gtk_action_set_sensitive (action, !is_auto_layout);
}


static char *
athena_icon_view_get_directory_sort_by (AthenaIconView *icon_view,
					  AthenaFile *file)
{
	const SortCriterion *default_sort_criterion;

	if (!athena_icon_view_supports_auto_layout (icon_view)) {
		return g_strdup ("name");
	}

	default_sort_criterion = get_sort_criterion_by_sort_type (get_default_sort_order (file, NULL));
	g_return_val_if_fail (default_sort_criterion != NULL, NULL);

	return athena_file_get_metadata
		(file, ATHENA_METADATA_KEY_ICON_VIEW_SORT_BY,
		 default_sort_criterion->metadata_text);
}

static AthenaFileSortType
get_default_sort_order (AthenaFile *file, gboolean *reversed)
{
	AthenaFileSortType retval, default_sort_order;
	gboolean default_sort_in_reverse_order;

	default_sort_order = g_settings_get_enum (athena_preferences,
						  ATHENA_PREFERENCES_DEFAULT_SORT_ORDER);
	default_sort_in_reverse_order = g_settings_get_boolean (athena_preferences,
								ATHENA_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER);

	retval = athena_file_get_default_sort_type (file, reversed);

	if (retval == ATHENA_FILE_SORT_NONE) {

		if (reversed != NULL) {
			*reversed = default_sort_in_reverse_order;
		}

		retval = CLAMP (default_sort_order, ATHENA_FILE_SORT_BY_DISPLAY_NAME,
				ATHENA_FILE_SORT_BY_MTIME);
	}

	return retval;
}

static void
athena_icon_view_set_directory_sort_by (AthenaIconView *icon_view, 
					  AthenaFile *file, 
					  const char *sort_by)
{
	const SortCriterion *default_sort_criterion;

	if (!athena_icon_view_supports_auto_layout (icon_view)) {
		return;
	}

	default_sort_criterion = get_sort_criterion_by_sort_type (get_default_sort_order (file, NULL));
	g_return_if_fail (default_sort_criterion != NULL);

	athena_file_set_metadata
		(file, ATHENA_METADATA_KEY_ICON_VIEW_SORT_BY,
		 default_sort_criterion->metadata_text,
		 sort_by);
}

static gboolean
athena_icon_view_get_directory_sort_reversed (AthenaIconView *icon_view,
						AthenaFile *file)
{
	gboolean reversed;

	if (!athena_icon_view_supports_auto_layout (icon_view)) {
		return FALSE;
	}

	get_default_sort_order (file, &reversed);
	return athena_file_get_boolean_metadata
		(file,
		 ATHENA_METADATA_KEY_ICON_VIEW_SORT_REVERSED,
		 reversed);
}

static void
athena_icon_view_set_directory_sort_reversed (AthenaIconView *icon_view,
						AthenaFile *file,
						gboolean sort_reversed)
{
	gboolean reversed;

	if (!athena_icon_view_supports_auto_layout (icon_view)) {
		return;
	}

	get_default_sort_order (file, &reversed);
	athena_file_set_boolean_metadata
		(file,
		 ATHENA_METADATA_KEY_ICON_VIEW_SORT_REVERSED,
		 reversed, sort_reversed);
}

static gboolean
get_default_directory_keep_aligned (void)
{
	return TRUE;
}

static gboolean
athena_icon_view_get_directory_keep_aligned (AthenaIconView *icon_view,
					       AthenaFile *file)
{
	if (!athena_icon_view_supports_keep_aligned (icon_view)) {
		return FALSE;
	}
	
	return  athena_file_get_boolean_metadata
		(file,
		 ATHENA_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED,
		 get_default_directory_keep_aligned ());
}

static void
athena_icon_view_set_directory_keep_aligned (AthenaIconView *icon_view,
					       AthenaFile *file,
					       gboolean keep_aligned)
{
	if (!athena_icon_view_supports_keep_aligned (icon_view)) {
		return;
	}

	athena_file_set_boolean_metadata
		(file, ATHENA_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED,
		 get_default_directory_keep_aligned (),
		 keep_aligned);
}

static gboolean
athena_icon_view_get_directory_auto_layout (AthenaIconView *icon_view,
					      AthenaFile *file)
{
	if (!athena_icon_view_supports_auto_layout (icon_view)) {
		return FALSE;
	}

	if (!athena_icon_view_supports_manual_layout (icon_view)) {
		return TRUE;
	}

	return athena_file_get_boolean_metadata
		(file, ATHENA_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT, TRUE);
}

static void
athena_icon_view_set_directory_auto_layout (AthenaIconView *icon_view,
					      AthenaFile *file,
					gboolean auto_layout)
{
	if (!athena_icon_view_supports_auto_layout (icon_view) ||
	    !athena_icon_view_supports_manual_layout (icon_view)) {
		return;
	}

	athena_file_set_boolean_metadata
		(file, ATHENA_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT,
		 TRUE,
		 auto_layout);
}

static gboolean
set_sort_reversed (AthenaIconView *icon_view,
		   gboolean new_value,
		   gboolean set_metadata)
{
	if (icon_view->details->sort_reversed == new_value) {
		return FALSE;
	}
	icon_view->details->sort_reversed = new_value;

	if (set_metadata) {
		/* Store the new sort setting. */
		athena_icon_view_set_directory_sort_reversed (icon_view, athena_view_get_directory_as_file (ATHENA_VIEW (icon_view)), new_value);
	}
	
	/* Update the layout menus to match the new sort-order setting. */
	update_layout_menus (icon_view);

	return TRUE;
}

static const SortCriterion *
get_sort_criterion_by_metadata_text (const char *metadata_text)
{
	guint i;

	/* Figure out what the new sort setting should be. */
	for (i = 0; i < G_N_ELEMENTS (sort_criteria); i++) {
		if (strcmp (sort_criteria[i].metadata_text, metadata_text) == 0) {
			return &sort_criteria[i];
		}
	}
	return NULL;
}

static const SortCriterion *
get_sort_criterion_by_sort_type (AthenaFileSortType sort_type)
{
	guint i;

	/* Figure out what the new sort setting should be. */
	for (i = 0; i < G_N_ELEMENTS (sort_criteria); i++) {
		if (sort_type == sort_criteria[i].sort_type) {
			return &sort_criteria[i];
		}
	}

	return &sort_criteria[0];
}

#define DEFAULT_ZOOM_LEVEL(icon_view) icon_view->details->compact ? default_compact_zoom_level : default_zoom_level

static AthenaZoomLevel
get_default_zoom_level (AthenaIconView *icon_view)
{
	AthenaZoomLevel default_zoom_level, default_compact_zoom_level;

	default_zoom_level = g_settings_get_enum (athena_icon_view_preferences,
						  ATHENA_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL);
	default_compact_zoom_level = g_settings_get_enum (athena_icon_view_preferences,
							  ATHENA_PREFERENCES_COMPACT_VIEW_DEFAULT_ZOOM_LEVEL);

	return CLAMP (DEFAULT_ZOOM_LEVEL(icon_view), ATHENA_ZOOM_LEVEL_SMALLEST, ATHENA_ZOOM_LEVEL_LARGEST);
}

static void
set_labels_beside_icons (AthenaIconView *icon_view)
{
	gboolean labels_beside;

	if (athena_icon_view_supports_labels_beside_icons (icon_view)) {
		labels_beside = athena_icon_view_is_compact (icon_view) ||
			g_settings_get_boolean (athena_icon_view_preferences,
						ATHENA_PREFERENCES_ICON_VIEW_LABELS_BESIDE_ICONS);

		if (labels_beside) {
			athena_icon_container_set_label_position
				(get_icon_container (icon_view),
				 ATHENA_ICON_LABEL_POSITION_BESIDE);
		} else {
			athena_icon_container_set_label_position
				(get_icon_container (icon_view),
				 ATHENA_ICON_LABEL_POSITION_UNDER);
		}
	}
}

static void
set_columns_same_width (AthenaIconView *icon_view)
{
	gboolean all_columns_same_width;

	if (athena_icon_view_is_compact (icon_view)) {
		all_columns_same_width = g_settings_get_boolean (athena_compact_view_preferences,
								 ATHENA_PREFERENCES_COMPACT_VIEW_ALL_COLUMNS_SAME_WIDTH);
		athena_icon_container_set_all_columns_same_width (get_icon_container (icon_view), all_columns_same_width);
	}
}

static void
athena_icon_view_begin_loading (AthenaView *view)
{
	AthenaIconView *icon_view;
	GtkWidget *icon_container;
	AthenaFile *file;
	int level;
	char *sort_name, *uri;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));

	icon_view = ATHENA_ICON_VIEW (view);
	file = athena_view_get_directory_as_file (view);
	uri = athena_file_get_uri (file);
	icon_container = GTK_WIDGET (get_icon_container (icon_view));

	athena_icon_container_begin_loading (ATHENA_ICON_CONTAINER (icon_container));

	athena_icon_container_set_allow_moves (ATHENA_ICON_CONTAINER (icon_container),
						 !eel_uri_is_search (uri));

	g_free (uri);

	/* Set up the zoom level from the metadata. */
	if (athena_view_supports_zooming (ATHENA_VIEW (icon_view))) {
		if (icon_view->details->compact) {
			level = athena_file_get_integer_metadata
				(file, 
				 ATHENA_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL, 
				 get_default_zoom_level (icon_view));
		} else {
			level = athena_file_get_integer_metadata
				(file, 
				 ATHENA_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL, 
				 get_default_zoom_level (icon_view));
		}

		athena_icon_view_set_zoom_level (icon_view, level, TRUE);
	}

	/* Set the sort mode.
	 * It's OK not to resort the icons because the
	 * container doesn't have any icons at this point.
	 */
	sort_name = athena_icon_view_get_directory_sort_by (icon_view, file);
	set_sort_criterion (icon_view, get_sort_criterion_by_metadata_text (sort_name), FALSE);
	g_free (sort_name);

	/* Set the sort direction from the metadata. */
	set_sort_reversed (icon_view, athena_icon_view_get_directory_sort_reversed (icon_view, file), FALSE);

	athena_icon_container_set_keep_aligned
		(get_icon_container (icon_view), 
		 athena_icon_view_get_directory_keep_aligned (icon_view, file));

	set_labels_beside_icons (icon_view);
	set_columns_same_width (icon_view);

	/* We must set auto-layout last, because it invokes the layout_changed 
	 * callback, which works incorrectly if the other layout criteria are
	 * not already set up properly (see bug 6500, e.g.)
	 */
	athena_icon_container_set_auto_layout
		(get_icon_container (icon_view), 
		 athena_icon_view_get_directory_auto_layout (icon_view, file));

	/* e.g. keep aligned may have changed */
	update_layout_menus (icon_view);
}

static void
icon_view_notify_clipboard_info (AthenaClipboardMonitor *monitor,
                                 AthenaClipboardInfo *info,
                                 AthenaIconView *icon_view)
{
	GList *icon_data;

	icon_data = NULL;
	if (info && info->cut) {
		icon_data = info->files;
	}

	athena_icon_container_set_highlighted_for_clipboard (
							       get_icon_container (icon_view), icon_data);
}

static void
athena_icon_view_end_loading (AthenaView *view,
			  gboolean all_files_seen)
{
	AthenaIconView *icon_view;
	GtkWidget *icon_container;
	AthenaClipboardMonitor *monitor;
	AthenaClipboardInfo *info;

	icon_view = ATHENA_ICON_VIEW (view);

	icon_container = GTK_WIDGET (get_icon_container (icon_view));
	athena_icon_container_end_loading (ATHENA_ICON_CONTAINER (icon_container), all_files_seen);

	monitor = athena_clipboard_monitor_get ();
	info = athena_clipboard_monitor_get_clipboard_info (monitor);

	icon_view_notify_clipboard_info (monitor, info, icon_view);
}

static AthenaZoomLevel
athena_icon_view_get_zoom_level (AthenaView *view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), ATHENA_ZOOM_LEVEL_STANDARD);
	
	return athena_icon_container_get_zoom_level (get_icon_container (ATHENA_ICON_VIEW (view)));
}

static void
athena_icon_view_set_zoom_level (AthenaIconView *view,
				   AthenaZoomLevel new_level,
				   gboolean always_emit)
{
	AthenaIconContainer *icon_container;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));
	g_return_if_fail (new_level >= ATHENA_ZOOM_LEVEL_SMALLEST &&
			  new_level <= ATHENA_ZOOM_LEVEL_LARGEST);

	icon_container = get_icon_container (view);
	if (athena_icon_container_get_zoom_level (icon_container) == new_level) {
		if (always_emit) {
			g_signal_emit_by_name (view, "zoom_level_changed");
		}
		return;
	}

	if (view->details->compact) {
		athena_file_set_integer_metadata
			(athena_view_get_directory_as_file (ATHENA_VIEW (view)), 
			 ATHENA_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL, 
			 get_default_zoom_level (view),
			 new_level);
	} else {
		athena_file_set_integer_metadata
			(athena_view_get_directory_as_file (ATHENA_VIEW (view)), 
			 ATHENA_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL, 
			 get_default_zoom_level (view),
			 new_level);
	}

	athena_icon_container_set_zoom_level (icon_container, new_level);

	g_signal_emit_by_name (view, "zoom_level_changed");
	
	if (athena_view_get_active (ATHENA_VIEW (view))) {
		athena_view_update_menus (ATHENA_VIEW (view));
	}
}

static void
athena_icon_view_bump_zoom_level (AthenaView *view, int zoom_increment)
{
	AthenaZoomLevel new_level;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));

	new_level = athena_icon_view_get_zoom_level (view) + zoom_increment;

	if (new_level >= ATHENA_ZOOM_LEVEL_SMALLEST &&
	    new_level <= ATHENA_ZOOM_LEVEL_LARGEST) {
		athena_view_zoom_to_level (view, new_level);
	}
}

static void
athena_icon_view_zoom_to_level (AthenaView *view,
			    AthenaZoomLevel zoom_level)
{
	AthenaIconView *icon_view;

	g_assert (ATHENA_IS_ICON_VIEW (view));

	icon_view = ATHENA_ICON_VIEW (view);
	athena_icon_view_set_zoom_level (icon_view, zoom_level, FALSE);
}

static void
athena_icon_view_restore_default_zoom_level (AthenaView *view)
{
	AthenaIconView *icon_view;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));

	icon_view = ATHENA_ICON_VIEW (view);
	athena_view_zoom_to_level
		(view, get_default_zoom_level (icon_view));
}

static gboolean 
athena_icon_view_can_zoom_in (AthenaView *view) 
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), FALSE);

	return athena_icon_view_get_zoom_level (view) 
		< ATHENA_ZOOM_LEVEL_LARGEST;
}

static gboolean 
athena_icon_view_can_zoom_out (AthenaView *view) 
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), FALSE);

	return athena_icon_view_get_zoom_level (view) 
		> ATHENA_ZOOM_LEVEL_SMALLEST;
}

static gboolean
athena_icon_view_is_empty (AthenaView *view)
{
	g_assert (ATHENA_IS_ICON_VIEW (view));

	return athena_icon_container_is_empty 
		(get_icon_container (ATHENA_ICON_VIEW (view)));
}

static GList *
athena_icon_view_get_selection (AthenaView *view)
{
	GList *list;

	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), NULL);

	list = athena_icon_container_get_selection
		(get_icon_container (ATHENA_ICON_VIEW (view)));
	athena_file_list_ref (list);
	return list;
}

static void
count_item (AthenaIconData *icon_data,
	    gpointer callback_data)
{
	guint *count;

	count = callback_data;
	(*count)++;
}

static guint
athena_icon_view_get_item_count (AthenaView *view)
{
	guint count;

	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), 0);

	count = 0;
	
	athena_icon_container_for_each
		(get_icon_container (ATHENA_ICON_VIEW (view)),
		 count_item, &count);

	return count;
}

static void
set_sort_criterion_by_sort_type (AthenaIconView *icon_view,
				 AthenaFileSortType  sort_type)
{
	const SortCriterion *sort;

	g_assert (ATHENA_IS_ICON_VIEW (icon_view));

	sort = get_sort_criterion_by_sort_type (sort_type);
	g_return_if_fail (sort != NULL);
	
	if (sort == icon_view->details->sort
	    && athena_icon_view_using_auto_layout (icon_view)) {
		return;
	}

	set_sort_criterion (icon_view, sort, TRUE);
	athena_icon_container_sort (get_icon_container (icon_view));
	athena_icon_view_reveal_selection (ATHENA_VIEW (icon_view));
}


static void
action_reversed_order_callback (GtkAction *action,
				gpointer user_data)
{
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (user_data);

	if (set_sort_reversed (icon_view,
			       gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)),
			       TRUE)) {
		athena_icon_container_sort (get_icon_container (icon_view));
		athena_icon_view_reveal_selection (ATHENA_VIEW (icon_view));
	}
}

static void
action_keep_aligned_callback (GtkAction *action,
			      gpointer user_data)
{
	AthenaIconView *icon_view;
	AthenaFile *file;
	gboolean keep_aligned;

	icon_view = ATHENA_ICON_VIEW (user_data);

	keep_aligned = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	file = athena_view_get_directory_as_file (ATHENA_VIEW (icon_view));
	athena_icon_view_set_directory_keep_aligned (icon_view,
						 file,
						 keep_aligned);
						      
	athena_icon_container_set_keep_aligned (get_icon_container (icon_view),
						  keep_aligned);
}

static void
switch_to_manual_layout (AthenaIconView *icon_view)
{
	if (!athena_icon_view_using_auto_layout (icon_view)) {
		return;
	}

	icon_view->details->sort = &sort_criteria[0];
	
	athena_icon_container_set_auto_layout
		(get_icon_container (icon_view), FALSE);
}

static void
layout_changed_callback (AthenaIconContainer *container,
			 AthenaIconView *icon_view)
{
	AthenaFile *file;

	g_assert (ATHENA_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	file = athena_view_get_directory_as_file (ATHENA_VIEW (icon_view));

	if (file != NULL) {
		athena_icon_view_set_directory_auto_layout
			(icon_view,
			 file,
			 athena_icon_view_using_auto_layout (icon_view));
	}

	update_layout_menus (icon_view);
}

static gboolean
athena_icon_view_can_rename_file (AthenaView *view, AthenaFile *file)
{
	if (!(athena_icon_view_get_zoom_level (view) > ATHENA_ZOOM_LEVEL_SMALLEST)) {
		return FALSE;
	}

	return ATHENA_VIEW_CLASS(athena_icon_view_parent_class)->can_rename_file (view, file);
}

static void
athena_icon_view_start_renaming_file (AthenaView *view,
				  AthenaFile *file,
				  gboolean select_all)
{
	/* call parent class to make sure the right icon is selected */
	ATHENA_VIEW_CLASS(athena_icon_view_parent_class)->start_renaming_file (view, file, select_all);
	
	/* start renaming */
	athena_icon_container_start_renaming_selected_item
		(get_icon_container (ATHENA_ICON_VIEW (view)), select_all);
}

static const GtkActionEntry icon_view_entries[] = {
  /* name, stock id, label */  { "Arrange Items", NULL, N_("Arran_ge Items") }, 
  /* name, stock id */         { "Stretch", NULL,
  /* label, accelerator */       N_("Resize Icon..."), NULL,
  /* tooltip */                  N_("Make the selected icon resizable"),
                                 G_CALLBACK (action_stretch_callback) },
  /* name, stock id */         { "Unstretch", NULL,
  /* label, accelerator */       N_("Restore Icons' Original Si_zes"), NULL,
  /* tooltip */                  N_("Restore each selected icon to its original size"),
                                 G_CALLBACK (action_unstretch_callback) },
  /* name, stock id */         { "Clean Up", NULL,
  /* label, accelerator */       N_("_Organize by Name"), NULL,
  /* tooltip */                  N_("Reposition icons to better fit in the window and avoid overlapping"),
                                 G_CALLBACK (action_clean_up_callback) },
};

static const GtkToggleActionEntry icon_view_toggle_entries[] = {
  /* name, stock id */      { "Reversed Order", NULL,
  /* label, accelerator */    N_("Re_versed Order"), NULL,
  /* tooltip */               N_("Display icons in the opposite order"),
                              G_CALLBACK (action_reversed_order_callback),
                              0 },
  /* name, stock id */      { "Keep Aligned", NULL,
  /* label, accelerator */    N_("_Keep Aligned"), NULL,
  /* tooltip */               N_("Keep icons lined up on a grid"),
                              G_CALLBACK (action_keep_aligned_callback),
                              0 },
};

static const GtkRadioActionEntry arrange_radio_entries[] = {
  { "Manual Layout", NULL,
    N_("_Manually"), NULL,
    N_("Leave icons wherever they are dropped"),
    ATHENA_FILE_SORT_NONE },
  { "Sort by Name", NULL,
    N_("By _Name"), NULL,
    N_("Keep icons sorted by name in rows"),
    ATHENA_FILE_SORT_BY_DISPLAY_NAME },
  { "Sort by Size", NULL,
    N_("By _Size"), NULL,
    N_("Keep icons sorted by size in rows"),
    ATHENA_FILE_SORT_BY_SIZE },
  { "Sort by Type", NULL,
    N_("By _Type"), NULL,
    N_("Keep icons sorted by type in rows"),
    ATHENA_FILE_SORT_BY_TYPE },
  { "Sort by Modification Date", NULL,
    N_("By Modification _Date"), NULL,
    N_("Keep icons sorted by modification date in rows"),
    ATHENA_FILE_SORT_BY_MTIME },
  { "Sort by Trash Time", NULL,
    N_("By T_rash Time"), NULL,
    N_("Keep icons sorted by trash time in rows"),
    ATHENA_FILE_SORT_BY_TRASHED_TIME },
};

static void
athena_icon_view_merge_menus (AthenaView *view)
{
	AthenaIconView *icon_view;
	GtkUIManager *ui_manager;
	GtkActionGroup *action_group;
	GtkAction *action;
	
        g_assert (ATHENA_IS_ICON_VIEW (view));

	ATHENA_VIEW_CLASS (athena_icon_view_parent_class)->merge_menus (view);

	icon_view = ATHENA_ICON_VIEW (view);

	ui_manager = athena_view_get_ui_manager (ATHENA_VIEW (icon_view));

	action_group = gtk_action_group_new ("IconViewActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	icon_view->details->icon_action_group = action_group;
	gtk_action_group_add_actions (action_group,
				      icon_view_entries, G_N_ELEMENTS (icon_view_entries),
				      icon_view);
	gtk_action_group_add_toggle_actions (action_group, 
					     icon_view_toggle_entries, G_N_ELEMENTS (icon_view_toggle_entries),
					     icon_view);
	gtk_action_group_add_radio_actions (action_group,
					    arrange_radio_entries,
					    G_N_ELEMENTS (arrange_radio_entries),
					    -1,
					    G_CALLBACK (action_sort_radio_callback),
					    icon_view);
 
	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
	g_object_unref (action_group); /* owned by ui manager */

	icon_view->details->icon_merge_id =
		gtk_ui_manager_add_ui_from_resource (ui_manager, "/com/solusos/athena/athena-icon-view-ui.xml", NULL);

	/* Do one-time state-setting here; context-dependent state-setting
	 * is done in update_menus.
	 */
	if (!athena_icon_view_supports_auto_layout (icon_view)) {
		action = gtk_action_group_get_action (action_group,
						      ATHENA_ACTION_ARRANGE_ITEMS);
		gtk_action_set_visible (action, FALSE);
	}

	if (athena_icon_view_supports_scaling (icon_view)) {
		gtk_ui_manager_add_ui (ui_manager,
				       icon_view->details->icon_merge_id,
				       POPUP_PATH_ICON_APPEARANCE,
				       ATHENA_ACTION_STRETCH,
				       ATHENA_ACTION_STRETCH,
				       GTK_UI_MANAGER_MENUITEM,
				       FALSE);
		gtk_ui_manager_add_ui (ui_manager,
				       icon_view->details->icon_merge_id,
				       POPUP_PATH_ICON_APPEARANCE,
				       ATHENA_ACTION_UNSTRETCH,
				       ATHENA_ACTION_UNSTRETCH,
				       GTK_UI_MANAGER_MENUITEM,
				       FALSE);
	}

	update_layout_menus (icon_view);
}

static void
athena_icon_view_unmerge_menus (AthenaView *view)
{
	AthenaIconView *icon_view;
	GtkUIManager *ui_manager;

	icon_view = ATHENA_ICON_VIEW (view);

	ATHENA_VIEW_CLASS (athena_icon_view_parent_class)->unmerge_menus (view);

	ui_manager = athena_view_get_ui_manager (view);
	if (ui_manager != NULL) {
		athena_ui_unmerge_ui (ui_manager,
					&icon_view->details->icon_merge_id,
					&icon_view->details->icon_action_group);
	}
}

static void
athena_icon_view_update_menus (AthenaView *view)
{
	AthenaIconView *icon_view;
        int selection_count;
	GtkAction *action;
        AthenaIconContainer *icon_container;
	gboolean editable;

        icon_view = ATHENA_ICON_VIEW (view);

	ATHENA_VIEW_CLASS (athena_icon_view_parent_class)->update_menus(view);

        selection_count = athena_view_get_selection_count (view);
        icon_container = get_icon_container (icon_view);

	action = gtk_action_group_get_action (icon_view->details->icon_action_group,
					      ATHENA_ACTION_STRETCH);
	gtk_action_set_sensitive (action,
				  selection_count == 1
				  && icon_container != NULL
				  && !athena_icon_container_has_stretch_handles (icon_container));

	gtk_action_set_visible (action,
				athena_icon_view_supports_scaling (icon_view));

	action = gtk_action_group_get_action (icon_view->details->icon_action_group,
					      ATHENA_ACTION_UNSTRETCH);
	g_object_set (action, "label",
		      (selection_count > 1)
		      ? _("Restore Icons' Original Si_zes")
		      : _("Restore Icon's Original Si_ze"),
		      NULL);
	gtk_action_set_sensitive (action,
				  icon_container != NULL
				  && athena_icon_container_is_stretched (icon_container));

	gtk_action_set_visible (action,
				athena_icon_view_supports_scaling (icon_view));

	editable = athena_view_is_editable (view);
	action = gtk_action_group_get_action (icon_view->details->icon_action_group,
					      ATHENA_ACTION_MANUAL_LAYOUT);
	gtk_action_set_sensitive (action, editable);
}

static void
athena_icon_view_reset_to_defaults (AthenaView *view)
{
	AthenaIconContainer *icon_container;
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (view);
	icon_container = get_icon_container (icon_view);

	clear_sort_criterion (icon_view);
	athena_icon_container_set_keep_aligned 
		(icon_container, get_default_directory_keep_aligned ());

	athena_icon_container_sort (icon_container);

	update_layout_menus (icon_view);

	athena_icon_view_restore_default_zoom_level (view);
}

static void
athena_icon_view_select_all (AthenaView *view)
{
	AthenaIconContainer *icon_container;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));

	icon_container = get_icon_container (ATHENA_ICON_VIEW (view));
        athena_icon_container_select_all (icon_container);
}

static void
athena_icon_view_reveal_selection (AthenaView *view)
{
	GList *selection;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));

        selection = athena_view_get_selection (view);

	/* Make sure at least one of the selected items is scrolled into view */
	if (selection != NULL) {
		athena_icon_container_reveal 
			(get_icon_container (ATHENA_ICON_VIEW (view)), 
			 selection->data);
	}

        athena_file_list_free (selection);
}

static GArray *
athena_icon_view_get_selected_icon_locations (AthenaView *view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), NULL);

	return athena_icon_container_get_selected_icon_locations
		(get_icon_container (ATHENA_ICON_VIEW (view)));
}


static void
athena_icon_view_set_selection (AthenaView *view, GList *selection)
{
	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));

	athena_icon_container_set_selection
		(get_icon_container (ATHENA_ICON_VIEW (view)), selection);
}

static void
athena_icon_view_invert_selection (AthenaView *view)
{
	g_return_if_fail (ATHENA_IS_ICON_VIEW (view));

	athena_icon_container_invert_selection
		(get_icon_container (ATHENA_ICON_VIEW (view)));
}

static gboolean
athena_icon_view_using_manual_layout (AthenaView *view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (view), FALSE);

	return !athena_icon_view_using_auto_layout (ATHENA_ICON_VIEW (view));
}

static void
athena_icon_view_widget_to_file_operation_position (AthenaView *view,
						GdkPoint *position)
{
	g_assert (ATHENA_IS_ICON_VIEW (view));

	athena_icon_container_widget_to_file_operation_position
		(get_icon_container (ATHENA_ICON_VIEW (view)), position);
}

static void
icon_container_activate_callback (AthenaIconContainer *container,
				  GList *file_list,
				  AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	athena_view_activate_files (ATHENA_VIEW (icon_view),
				      file_list, 
				      0, TRUE);
}

static void
icon_container_activate_previewer_callback (AthenaIconContainer *container,
					    GList *file_list,
					    GArray *locations,
					    AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	athena_view_preview_files (ATHENA_VIEW (icon_view),
				     file_list, locations);
}

/* this is called in one of these cases:
 * - we activate with enter holding shift
 * - we activate with space holding shift
 * - we double click an icon holding shift
 * - we middle click an icon
 *
 * If we don't open in new windows by default, the behavior should be
 * - middle click, shift + activate -> open in new tab
 * - shift + double click -> open in new window
 *
 * If we open in new windows by default, the behaviour should be
 * - middle click, or shift + activate, or shift + double-click -> close parent
 */
static void
icon_container_activate_alternate_callback (AthenaIconContainer *container,
					    GList *file_list,
					    AthenaIconView *icon_view)
{
	GdkEvent *event;
	GdkEventButton *button_event;
	GdkEventKey *key_event;
	gboolean open_in_tab, open_in_window, close_behind;
	AthenaWindowOpenFlags flags;

	g_assert (ATHENA_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	flags = 0;
	event = gtk_get_current_event ();
	open_in_tab = FALSE;
	open_in_window = FALSE;
	close_behind = FALSE;
	
	if (g_settings_get_boolean (athena_preferences,
				    ATHENA_PREFERENCES_ALWAYS_USE_BROWSER)) {
		if (event->type == GDK_BUTTON_PRESS ||
		    event->type == GDK_BUTTON_RELEASE ||
		    event->type == GDK_2BUTTON_PRESS ||
		    event->type == GDK_3BUTTON_PRESS) {
			button_event = (GdkEventButton *) event;
			open_in_window = ((button_event->state & GDK_SHIFT_MASK) != 0);
			open_in_tab = !open_in_window;
		} else if (event->type == GDK_KEY_PRESS ||
			   event->type == GDK_KEY_RELEASE) {
			key_event = (GdkEventKey *) event;
			open_in_tab = ((key_event->state & GDK_SHIFT_MASK) != 0);
		}
	} else {
		close_behind = TRUE;
	} 

	if (open_in_tab) {
		flags |= ATHENA_WINDOW_OPEN_FLAG_NEW_TAB;
	}

	if (open_in_window) {
		flags |= ATHENA_WINDOW_OPEN_FLAG_NEW_WINDOW;
	}

	if (close_behind) {
		flags |= ATHENA_WINDOW_OPEN_FLAG_CLOSE_BEHIND;
	}

	DEBUG ("Activate alternate, open in tab %d, close behind %d, new window %d\n",
	       open_in_tab, close_behind, open_in_window);

	athena_view_activate_files (ATHENA_VIEW (icon_view), 
				      file_list, 
				      flags,
				      TRUE);
}

static void
band_select_started_callback (AthenaIconContainer *container,
			      AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	athena_view_start_batching_selection_changes (ATHENA_VIEW (icon_view));
}

static void
band_select_ended_callback (AthenaIconContainer *container,
			    AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	athena_view_stop_batching_selection_changes (ATHENA_VIEW (icon_view));
}

int
athena_icon_view_compare_files (AthenaIconView   *icon_view,
				  AthenaFile *a,
				  AthenaFile *b)
{
	return athena_file_compare_for_sort
		(a, b, icon_view->details->sort->sort_type,
		 /* Use type-unsafe cast for performance */
		 athena_view_should_sort_directories_first ((AthenaView *)icon_view),
		 icon_view->details->sort_reversed);
}

static int
compare_files (AthenaView   *icon_view,
	       AthenaFile *a,
	       AthenaFile *b)
{
	return athena_icon_view_compare_files ((AthenaIconView *)icon_view, a, b);
}


void
athena_icon_view_filter_by_screen (AthenaIconView *icon_view,
				     gboolean filter)
{
	icon_view->details->filter_by_screen = filter;
	icon_view->details->num_screens = gdk_display_get_n_screens (gtk_widget_get_display (GTK_WIDGET (icon_view)));
}

static void
athena_icon_view_screen_changed (GtkWidget *widget,
				   GdkScreen *previous_screen)
{
	AthenaView *view;
	GList *files, *l;
	AthenaFile *file;
	AthenaDirectory *directory;
	AthenaIconContainer *icon_container;

	if (GTK_WIDGET_CLASS (athena_icon_view_parent_class)->screen_changed) {
		GTK_WIDGET_CLASS (athena_icon_view_parent_class)->screen_changed (widget, previous_screen);
	}

	view = ATHENA_VIEW (widget);
	if (ATHENA_ICON_VIEW (view)->details->filter_by_screen) {
		icon_container = get_icon_container (ATHENA_ICON_VIEW (view));

		directory = athena_view_get_model (view);
		files = athena_directory_get_file_list (directory);

		for (l = files; l != NULL; l = l->next) {
			file = l->data;
			
			if (!should_show_file_on_screen (view, file)) {
				athena_icon_view_remove_file (view, file, directory);
			} else {
				if (athena_icon_container_add (icon_container,
								 ATHENA_ICON_CONTAINER_ICON_DATA (file))) {
					athena_file_ref (file);
				}
			}
		}
		
		athena_file_list_unref (files);
		g_list_free (files);
	}
}

static gboolean
athena_icon_view_scroll_event (GtkWidget *widget,
			   GdkEventScroll *scroll_event)
{
	AthenaIconView *icon_view;
	GdkEvent *event_copy;
	GdkEventScroll *scroll_event_copy;
	gboolean ret;

	icon_view = ATHENA_ICON_VIEW (widget);

	if (icon_view->details->compact &&
	    (scroll_event->direction == GDK_SCROLL_UP ||
	     scroll_event->direction == GDK_SCROLL_DOWN ||
	     scroll_event->direction == GDK_SCROLL_SMOOTH)) {
		ret = athena_view_handle_scroll_event (ATHENA_VIEW (icon_view), scroll_event);
		if (!ret) {
			/* in column-wise layout, re-emit vertical mouse scroll events as horizontal ones,
			 * if they don't bump zoom */
			event_copy = gdk_event_copy ((GdkEvent *) scroll_event);
			scroll_event_copy = (GdkEventScroll *) event_copy;

			/* transform vertical integer smooth scroll events into horizontal events */
			if (scroll_event_copy->direction == GDK_SCROLL_SMOOTH &&
				   scroll_event_copy->delta_x == 0) {
				if (scroll_event_copy->delta_y == 1.0) {
					scroll_event_copy->direction = GDK_SCROLL_DOWN;
				} else if (scroll_event_copy->delta_y == -1.0) {
					scroll_event_copy->direction = GDK_SCROLL_UP;
				}
			}

			if (scroll_event_copy->direction == GDK_SCROLL_UP) {
				scroll_event_copy->direction = GDK_SCROLL_LEFT;
			} else if (scroll_event_copy->direction == GDK_SCROLL_DOWN) {
				scroll_event_copy->direction = GDK_SCROLL_RIGHT;
			}

			ret = GTK_WIDGET_CLASS (athena_icon_view_parent_class)->scroll_event (widget, scroll_event_copy);
			gdk_event_free (event_copy);
		}

		return ret;
	}

	return GTK_WIDGET_CLASS (athena_icon_view_parent_class)->scroll_event (widget, scroll_event);
}

static void
selection_changed_callback (AthenaIconContainer *container,
			    AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	athena_view_notify_selection_changed (ATHENA_VIEW (icon_view));
}

static void
icon_container_context_click_selection_callback (AthenaIconContainer *container,
						 GdkEventButton *event,
						 AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_ICON_CONTAINER (container));
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));

	athena_view_pop_up_selection_context_menu 
		(ATHENA_VIEW (icon_view), event);
}

static void
icon_container_context_click_background_callback (AthenaIconContainer *container,
						  GdkEventButton *event,
						  AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_ICON_CONTAINER (container));
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));

	athena_view_pop_up_background_context_menu 
		(ATHENA_VIEW (icon_view), event);
}

static gboolean
athena_icon_view_react_to_icon_change_idle_callback (gpointer data) 
{        
        AthenaIconView *icon_view;
        
        g_assert (ATHENA_IS_ICON_VIEW (data));
        
        icon_view = ATHENA_ICON_VIEW (data);
        icon_view->details->react_to_icon_change_idle_id = 0;
        
	/* Rebuild the menus since some of them (e.g. Restore Stretched Icons)
	 * may be different now.
	 */
	athena_view_update_menus (ATHENA_VIEW (icon_view));

        /* Don't call this again (unless rescheduled) */
        return FALSE;
}

static void
icon_position_changed_callback (AthenaIconContainer *container,
				AthenaFile *file,
				const AthenaIconPosition *position,
				AthenaIconView *icon_view)
{
	char *position_string;
	char scale_string[G_ASCII_DTOSTR_BUF_SIZE];

	g_assert (ATHENA_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));
	g_assert (ATHENA_IS_FILE (file));

	/* Schedule updating menus for the next idle. Doing it directly here
	 * noticeably slows down icon stretching.  The other work here to
	 * store the icon position and scale does not seem to noticeably
	 * slow down icon stretching. It would be trickier to move to an
	 * idle call, because we'd have to keep track of potentially multiple
	 * sets of file/geometry info.
	 */
	if (athena_view_get_active (ATHENA_VIEW (icon_view)) &&
	    icon_view->details->react_to_icon_change_idle_id == 0) {
                icon_view->details->react_to_icon_change_idle_id
                        = g_idle_add (athena_icon_view_react_to_icon_change_idle_callback,
				      icon_view);
	}

	/* Store the new position of the icon in the metadata. */
	if (!athena_icon_view_using_auto_layout (icon_view)) {
		position_string = g_strdup_printf
			("%d,%d", position->x, position->y);
		athena_file_set_metadata
			(file, ATHENA_METADATA_KEY_ICON_POSITION, 
			 NULL, position_string);
		g_free (position_string);
	}


	g_ascii_dtostr (scale_string, sizeof (scale_string), position->scale);
	athena_file_set_metadata
		(file, ATHENA_METADATA_KEY_ICON_SCALE,
		 "1.0", scale_string);
}

/* Attempt to change the filename to the new text.  Notify user if operation fails. */
static void
icon_rename_ended_cb (AthenaIconContainer *container,
		      AthenaFile *file,				    
		      const char *new_name,
		      AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_FILE (file));

	athena_view_set_is_renaming (ATHENA_VIEW (icon_view), FALSE);

	/* Don't allow a rename with an empty string. Revert to original 
	 * without notifying the user.
	 */
	if ((new_name == NULL) || (new_name[0] == '\0')) {
		return;
	}

	athena_rename_file (file, new_name, NULL, NULL);
}

static void
icon_rename_started_cb (AthenaIconContainer *container,
			GtkWidget *widget,
			gpointer callback_data)
{
	AthenaView *directory_view;

	directory_view = ATHENA_VIEW (callback_data);
	athena_clipboard_set_up_editable
		(GTK_EDITABLE (widget),
		 athena_view_get_ui_manager (directory_view),
		 FALSE);
}

static char *
get_icon_uri_callback (AthenaIconContainer *container,
		       AthenaFile *file,
		       AthenaIconView *icon_view)
{
	g_assert (ATHENA_IS_ICON_CONTAINER (container));
	g_assert (ATHENA_IS_FILE (file));
	g_assert (ATHENA_IS_ICON_VIEW (icon_view));

	return athena_file_get_uri (file);
}

static char *
get_icon_drop_target_uri_callback (AthenaIconContainer *container,
		       		   AthenaFile *file,
		       		   AthenaIconView *icon_view)
{
	g_return_val_if_fail (ATHENA_IS_ICON_CONTAINER (container), NULL);
	g_return_val_if_fail (ATHENA_IS_FILE (file), NULL);
	g_return_val_if_fail (ATHENA_IS_ICON_VIEW (icon_view), NULL);

	return athena_file_get_drop_target_uri (file);
}

/* Preferences changed callbacks */
static void
athena_icon_view_click_policy_changed (AthenaView *directory_view)
{
	g_assert (ATHENA_IS_ICON_VIEW (directory_view));

	athena_icon_view_update_click_mode (ATHENA_ICON_VIEW (directory_view));
}

static void
image_display_policy_changed_callback (gpointer callback_data)
{
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (callback_data);

	athena_icon_container_request_update_all (get_icon_container (icon_view));
}

static void
text_attribute_names_changed_callback (gpointer callback_data)
{
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (callback_data);

	athena_icon_container_request_update_all (get_icon_container (icon_view));
}

static void
default_sort_order_changed_callback (gpointer callback_data)
{
	AthenaIconView *icon_view;
	AthenaFile *file;
	char *sort_name;
	AthenaIconContainer *icon_container;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (callback_data));

	icon_view = ATHENA_ICON_VIEW (callback_data);

	file = athena_view_get_directory_as_file (ATHENA_VIEW (icon_view));
	sort_name = athena_icon_view_get_directory_sort_by (icon_view, file);
	set_sort_criterion (icon_view, get_sort_criterion_by_metadata_text (sort_name), FALSE);
	g_free (sort_name);

	icon_container = get_icon_container (icon_view);
	g_return_if_fail (ATHENA_IS_ICON_CONTAINER (icon_container));

	athena_icon_container_request_update_all (icon_container);
}

static void
default_sort_in_reverse_order_changed_callback (gpointer callback_data)
{
	AthenaIconView *icon_view;
	AthenaFile *file;
	AthenaIconContainer *icon_container;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (callback_data));

	icon_view = ATHENA_ICON_VIEW (callback_data);

	file = athena_view_get_directory_as_file (ATHENA_VIEW (icon_view));
	set_sort_reversed (icon_view, athena_icon_view_get_directory_sort_reversed (icon_view, file), FALSE);
	icon_container = get_icon_container (icon_view);
	g_return_if_fail (ATHENA_IS_ICON_CONTAINER (icon_container));

	athena_icon_container_request_update_all (icon_container);
}

static void
default_zoom_level_changed_callback (gpointer callback_data)
{
	AthenaIconView *icon_view;
	AthenaFile *file;
	int level;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (callback_data));

	icon_view = ATHENA_ICON_VIEW (callback_data);

	if (athena_view_supports_zooming (ATHENA_VIEW (icon_view))) {
		file = athena_view_get_directory_as_file (ATHENA_VIEW (icon_view));

		if (athena_icon_view_is_compact (icon_view)) {
			level = athena_file_get_integer_metadata (file, 
								    ATHENA_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL, 
								    get_default_zoom_level (icon_view));
		} else {
			level = athena_file_get_integer_metadata (file, 
								    ATHENA_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL, 
								    get_default_zoom_level (icon_view));
		}
		athena_view_zoom_to_level (ATHENA_VIEW (icon_view), level);
	}
}

static void
labels_beside_icons_changed_callback (gpointer callback_data)
{
	AthenaIconView *icon_view;

	g_return_if_fail (ATHENA_IS_ICON_VIEW (callback_data));

	icon_view = ATHENA_ICON_VIEW (callback_data);

	set_labels_beside_icons (icon_view);
}

static void
all_columns_same_width_changed_callback (gpointer callback_data)
{
	AthenaIconView *icon_view;

	g_assert (ATHENA_IS_ICON_VIEW (callback_data));

	icon_view = ATHENA_ICON_VIEW (callback_data);

	set_columns_same_width (icon_view);
}


static void
athena_icon_view_sort_directories_first_changed (AthenaView *directory_view)
{
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (directory_view);

	if (athena_icon_view_using_auto_layout (icon_view)) {
		athena_icon_container_sort 
			(get_icon_container (icon_view));
	}
}

static gboolean
icon_view_can_accept_item (AthenaIconContainer *container,
			   AthenaFile *target_item,
			   const char *item_uri,
			   AthenaView *view)
{
	return athena_drag_can_accept_item (target_item, item_uri);
}

static char *
icon_view_get_container_uri (AthenaIconContainer *container,
			     AthenaView *view)
{
	return athena_view_get_uri (view);
}

static void
icon_view_move_copy_items (AthenaIconContainer *container,
			   const GList *item_uris,
			   GArray *relative_item_points,
			   const char *target_dir,
			   int copy_action,
			   int x, int y,
			   AthenaView *view)
{
	athena_clipboard_clear_if_colliding_uris (GTK_WIDGET (view),
						    item_uris,
						    athena_view_get_copied_files_atom (view));
	athena_view_move_copy_items (view, item_uris, relative_item_points, target_dir,
				       copy_action, x, y);
}

static void
athena_icon_view_update_click_mode (AthenaIconView *icon_view)
{
	AthenaIconContainer	*icon_container;
	int			click_mode;

	icon_container = get_icon_container (icon_view);
	g_assert (icon_container != NULL);

	click_mode = g_settings_get_enum (athena_preferences, ATHENA_PREFERENCES_CLICK_POLICY);

	athena_icon_container_set_single_click_mode (icon_container,
						       click_mode == ATHENA_CLICK_POLICY_SINGLE);
}

static gboolean
get_stored_layout_timestamp (AthenaIconContainer *container,
			     AthenaIconData *icon_data,
			     time_t *timestamp,
			     AthenaIconView *view)
{
	AthenaFile *file;
	AthenaDirectory *directory;

	if (icon_data == NULL) {
		directory = athena_view_get_model (ATHENA_VIEW (view));
		if (directory == NULL) {
			return FALSE;
		}

		file = athena_directory_get_corresponding_file (directory);
		*timestamp = athena_file_get_time_metadata (file,
							      ATHENA_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP);
		athena_file_unref (file);
	} else {
		*timestamp = athena_file_get_time_metadata (ATHENA_FILE (icon_data),
							      ATHENA_METADATA_KEY_ICON_POSITION_TIMESTAMP);
	}

	return TRUE;
}

static gboolean
store_layout_timestamp (AthenaIconContainer *container,
			AthenaIconData *icon_data,
			const time_t *timestamp,
			AthenaIconView *view)
{
	AthenaFile *file;
	AthenaDirectory *directory;

	if (icon_data == NULL) {
		directory = athena_view_get_model (ATHENA_VIEW (view));
		if (directory == NULL) {
			return FALSE;
		}

		file = athena_directory_get_corresponding_file (directory);
		athena_file_set_time_metadata (file,
						 ATHENA_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP,
						 (time_t) *timestamp);
		athena_file_unref (file);
	} else {
		athena_file_set_time_metadata (ATHENA_FILE (icon_data),
						 ATHENA_METADATA_KEY_ICON_POSITION_TIMESTAMP,
						 (time_t) *timestamp);
	}

	return TRUE;
}

static gboolean
focus_in_event_callback (GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
	AthenaWindowSlot *slot;
	AthenaIconView *icon_view = ATHENA_ICON_VIEW (user_data);
	
	/* make the corresponding slot (and the pane that contains it) active */
	slot = athena_view_get_athena_window_slot (ATHENA_VIEW (icon_view));
	athena_window_slot_make_hosting_pane_active (slot);

	return FALSE; 
}

static AthenaIconContainer *
create_icon_container (AthenaIconView *icon_view)
{
	AthenaIconContainer *icon_container;

	icon_container = athena_icon_view_container_new (icon_view);
	icon_view->details->icon_container = GTK_WIDGET (icon_container);
	g_object_add_weak_pointer (G_OBJECT (icon_container),
				   (gpointer *) &icon_view->details->icon_container);
	
	gtk_widget_set_can_focus (GTK_WIDGET (icon_container), TRUE);
	
	g_signal_connect_object (icon_container, "focus_in_event",
				 G_CALLBACK (focus_in_event_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "activate",	
				 G_CALLBACK (icon_container_activate_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "activate_alternate",	
				 G_CALLBACK (icon_container_activate_alternate_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "activate_previewer",
				 G_CALLBACK (icon_container_activate_previewer_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "band_select_started",
				 G_CALLBACK (band_select_started_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "band_select_ended",
				 G_CALLBACK (band_select_ended_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "context_click_selection",
				 G_CALLBACK (icon_container_context_click_selection_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "context_click_background",
				 G_CALLBACK (icon_container_context_click_background_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "icon_position_changed",
				 G_CALLBACK (icon_position_changed_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "selection_changed",
				 G_CALLBACK (selection_changed_callback), icon_view, 0);
	/* FIXME: many of these should move into fm-icon-container as virtual methods */
	g_signal_connect_object (icon_container, "get_icon_uri",
				 G_CALLBACK (get_icon_uri_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "get_icon_drop_target_uri",
				 G_CALLBACK (get_icon_drop_target_uri_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "move_copy_items",
				 G_CALLBACK (icon_view_move_copy_items), icon_view, 0);
	g_signal_connect_object (icon_container, "get_container_uri",
				 G_CALLBACK (icon_view_get_container_uri), icon_view, 0);
	g_signal_connect_object (icon_container, "can_accept_item",
				 G_CALLBACK (icon_view_can_accept_item), icon_view, 0);
	g_signal_connect_object (icon_container, "get_stored_icon_position",
				 G_CALLBACK (get_stored_icon_position_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "layout_changed",
				 G_CALLBACK (layout_changed_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "icon_rename_started",
				 G_CALLBACK (icon_rename_started_cb), icon_view, 0);
	g_signal_connect_object (icon_container, "icon_rename_ended",
				 G_CALLBACK (icon_rename_ended_cb), icon_view, 0);
	g_signal_connect_object (icon_container, "icon_stretch_started",
				 G_CALLBACK (athena_view_update_menus), icon_view,
				 G_CONNECT_SWAPPED);
	g_signal_connect_object (icon_container, "icon_stretch_ended",
				 G_CALLBACK (athena_view_update_menus), icon_view,
				 G_CONNECT_SWAPPED);

	g_signal_connect_object (icon_container, "get_stored_layout_timestamp",
				 G_CALLBACK (get_stored_layout_timestamp), icon_view, 0);
	g_signal_connect_object (icon_container, "store_layout_timestamp",
				 G_CALLBACK (store_layout_timestamp), icon_view, 0);

	gtk_container_add (GTK_CONTAINER (icon_view),
			   GTK_WIDGET (icon_container));

	athena_icon_view_update_click_mode (icon_view);

	gtk_widget_show (GTK_WIDGET (icon_container));

	return icon_container;
}

/* Handles an URL received from Mozilla */
static void
icon_view_handle_netscape_url (AthenaIconContainer *container, const char *encoded_url,
			       const char *target_uri,
			       GdkDragAction action, int x, int y, AthenaIconView *view)
{
	athena_view_handle_netscape_url_drop (ATHENA_VIEW (view),
						encoded_url, target_uri, action, x, y);
}

static void
icon_view_handle_uri_list (AthenaIconContainer *container, const char *item_uris,
			   const char *target_uri,
			   GdkDragAction action, int x, int y, AthenaIconView *view)
{
	athena_view_handle_uri_list_drop (ATHENA_VIEW (view),
					    item_uris, target_uri, action, x, y);
}

static void
icon_view_handle_text (AthenaIconContainer *container, const char *text,
		       const char *target_uri,
		       GdkDragAction action, int x, int y, AthenaIconView *view)
{
	athena_view_handle_text_drop (ATHENA_VIEW (view),
					text, target_uri, action, x, y);
}

static void
icon_view_handle_raw (AthenaIconContainer *container, const char *raw_data,
		      int length, const char *target_uri, const char *direct_save_uri,
		      GdkDragAction action, int x, int y, AthenaIconView *view)
{
	athena_view_handle_raw_drop (ATHENA_VIEW (view),
				       raw_data, length, target_uri, direct_save_uri, action, x, y);
}

static char *
icon_view_get_first_visible_file (AthenaView *view)
{
	AthenaFile *file;
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (view);

	file = ATHENA_FILE (athena_icon_container_get_first_visible_icon (get_icon_container (icon_view)));

	if (file) {
		return athena_file_get_uri (file);
	}
	
	return NULL;
}

static void
icon_view_scroll_to_file (AthenaView *view,
			  const char *uri)
{
	AthenaFile *file;
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (view);
	
	if (uri != NULL) {
		/* Only if existing, since we don't want to add the file to
		   the directory if it has been removed since then */
		file = athena_file_get_existing_by_uri (uri);
		if (file != NULL) {
			athena_icon_container_scroll_to_icon (get_icon_container (icon_view),
								ATHENA_ICON_CONTAINER_ICON_DATA (file));
			athena_file_unref (file);
		}
	}
}

static const char *
athena_icon_view_get_id (AthenaView *view)
{
	if (athena_icon_view_is_compact (ATHENA_ICON_VIEW (view))) {
		return FM_COMPACT_VIEW_ID;
	}

	return ATHENA_ICON_VIEW_ID;
}

static void
athena_icon_view_set_property (GObject         *object,
			   guint            prop_id,
			   const GValue    *value,
			   GParamSpec      *pspec)
{
	AthenaIconView *icon_view;
  
	icon_view = ATHENA_ICON_VIEW (object);

	switch (prop_id)  {
	case PROP_COMPACT:
		icon_view->details->compact = g_value_get_boolean (value);
		if (icon_view->details->compact) {
			athena_icon_container_set_layout_mode (get_icon_container (icon_view),
								 gtk_widget_get_direction (GTK_WIDGET(icon_view)) == GTK_TEXT_DIR_RTL ?
								 ATHENA_ICON_LAYOUT_T_B_R_L :
								 ATHENA_ICON_LAYOUT_T_B_L_R);
			athena_icon_container_set_forced_icon_size (get_icon_container (icon_view),
								      ATHENA_ICON_SIZE_SMALLEST);
		}
		break;
	case PROP_SUPPORTS_AUTO_LAYOUT:
		icon_view->details->supports_auto_layout = g_value_get_boolean (value);
		break;
	case PROP_SUPPORTS_SCALING:
		icon_view->details->supports_scaling = g_value_get_boolean (value);
		break;
	case PROP_SUPPORTS_KEEP_ALIGNED:
		icon_view->details->supports_keep_aligned = g_value_get_boolean (value);
		break;
	case PROP_SUPPORTS_LABELS_BESIDE_ICONS:
		icon_view->details->supports_labels_beside_icons = g_value_get_boolean (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
athena_icon_view_finalize (GObject *object)
{
	AthenaIconView *icon_view;

	icon_view = ATHENA_ICON_VIEW (object);

	g_free (icon_view->details);

	g_signal_handlers_disconnect_by_func (athena_preferences,
					      default_sort_order_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (athena_preferences,
					      default_sort_in_reverse_order_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (athena_preferences,
					      image_display_policy_changed_callback,
					      icon_view);

	g_signal_handlers_disconnect_by_func (athena_icon_view_preferences,
					      default_zoom_level_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (athena_icon_view_preferences,
					      labels_beside_icons_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (athena_icon_view_preferences,
					      text_attribute_names_changed_callback,
					      icon_view);

	g_signal_handlers_disconnect_by_func (athena_compact_view_preferences,
					      default_zoom_level_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (athena_compact_view_preferences,
					      all_columns_same_width_changed_callback,
					      icon_view);

	G_OBJECT_CLASS (athena_icon_view_parent_class)->finalize (object);
}

static void
athena_icon_view_class_init (AthenaIconViewClass *klass)
{
	AthenaViewClass *athena_view_class;
	GObjectClass *oclass;

	athena_view_class = ATHENA_VIEW_CLASS (klass);
	oclass = G_OBJECT_CLASS (klass);

	oclass->set_property = athena_icon_view_set_property;
	oclass->finalize = athena_icon_view_finalize;

	GTK_WIDGET_CLASS (klass)->destroy = athena_icon_view_destroy;
	GTK_WIDGET_CLASS (klass)->screen_changed = athena_icon_view_screen_changed;
	GTK_WIDGET_CLASS (klass)->scroll_event = athena_icon_view_scroll_event;
	
	athena_view_class->add_file = athena_icon_view_add_file;
	athena_view_class->begin_loading = athena_icon_view_begin_loading;
	athena_view_class->bump_zoom_level = athena_icon_view_bump_zoom_level;
	athena_view_class->can_rename_file = athena_icon_view_can_rename_file;
	athena_view_class->can_zoom_in = athena_icon_view_can_zoom_in;
	athena_view_class->can_zoom_out = athena_icon_view_can_zoom_out;
	athena_view_class->clear = athena_icon_view_clear;
	athena_view_class->end_loading = athena_icon_view_end_loading;
	athena_view_class->file_changed = athena_icon_view_file_changed;
	athena_view_class->get_selected_icon_locations = athena_icon_view_get_selected_icon_locations;
	athena_view_class->get_selection = athena_icon_view_get_selection;
	athena_view_class->get_selection_for_file_transfer = athena_icon_view_get_selection;
	athena_view_class->get_item_count = athena_icon_view_get_item_count;
	athena_view_class->is_empty = athena_icon_view_is_empty;
	athena_view_class->remove_file = athena_icon_view_remove_file;
	athena_view_class->reset_to_defaults = athena_icon_view_reset_to_defaults;
	athena_view_class->restore_default_zoom_level = athena_icon_view_restore_default_zoom_level;
	athena_view_class->reveal_selection = athena_icon_view_reveal_selection;
	athena_view_class->select_all = athena_icon_view_select_all;
	athena_view_class->set_selection = athena_icon_view_set_selection;
	athena_view_class->invert_selection = athena_icon_view_invert_selection;
	athena_view_class->compare_files = compare_files;
	athena_view_class->zoom_to_level = athena_icon_view_zoom_to_level;
	athena_view_class->get_zoom_level = athena_icon_view_get_zoom_level;
        athena_view_class->click_policy_changed = athena_icon_view_click_policy_changed;
        athena_view_class->merge_menus = athena_icon_view_merge_menus;
        athena_view_class->unmerge_menus = athena_icon_view_unmerge_menus;
        athena_view_class->sort_directories_first_changed = athena_icon_view_sort_directories_first_changed;
        athena_view_class->start_renaming_file = athena_icon_view_start_renaming_file;
        athena_view_class->update_menus = athena_icon_view_update_menus;
	athena_view_class->using_manual_layout = athena_icon_view_using_manual_layout;
	athena_view_class->widget_to_file_operation_position = athena_icon_view_widget_to_file_operation_position;
	athena_view_class->get_view_id = athena_icon_view_get_id;
	athena_view_class->get_first_visible_file = icon_view_get_first_visible_file;
	athena_view_class->scroll_to_file = icon_view_scroll_to_file;

	properties[PROP_COMPACT] =
		g_param_spec_boolean ("compact",
				      "Compact",
				      "Whether this view provides a compact listing",
				      FALSE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);
	properties[PROP_SUPPORTS_AUTO_LAYOUT] =
		g_param_spec_boolean ("supports-auto-layout",
				      "Supports auto layout",
				      "Whether this view supports auto layout",
				      TRUE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);
	properties[PROP_SUPPORTS_SCALING] =
		g_param_spec_boolean ("supports-scaling",
				      "Supports scaling",
				      "Whether this view supports scaling",
				      FALSE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);
	properties[PROP_SUPPORTS_KEEP_ALIGNED] =
		g_param_spec_boolean ("supports-keep-aligned",
				      "Supports keep aligned",
				      "Whether this view supports keep aligned",
				      FALSE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);
	properties[PROP_SUPPORTS_LABELS_BESIDE_ICONS] =
		g_param_spec_boolean ("supports-labels-beside-icons",
				      "Supports labels beside icons",
				      "Whether this view supports labels beside icons",
				      TRUE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);

	g_object_class_install_properties (oclass, NUM_PROPERTIES, properties);
}

static void
athena_icon_view_init (AthenaIconView *icon_view)
{
	AthenaIconContainer *icon_container;

        g_return_if_fail (gtk_bin_get_child (GTK_BIN (icon_view)) == NULL);

	icon_view->details = g_new0 (AthenaIconViewDetails, 1);
	icon_view->details->sort = &sort_criteria[0];
	icon_view->details->filter_by_screen = FALSE;

	icon_container = create_icon_container (icon_view);

	/* Set our default layout mode */
	athena_icon_container_set_layout_mode (icon_container,
						 gtk_widget_get_direction (GTK_WIDGET(icon_container)) == GTK_TEXT_DIR_RTL ?
						 ATHENA_ICON_LAYOUT_R_L_T_B :
						 ATHENA_ICON_LAYOUT_L_R_T_B);

	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_DEFAULT_SORT_ORDER,
				  G_CALLBACK (default_sort_order_changed_callback),
				  icon_view);
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER,
				  G_CALLBACK (default_sort_in_reverse_order_changed_callback),
				  icon_view);
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_SHOW_IMAGE_FILE_THUMBNAILS,
				  G_CALLBACK (image_display_policy_changed_callback),
				  icon_view);

	g_signal_connect_swapped (athena_icon_view_preferences,
				  "changed::" ATHENA_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL,
				  G_CALLBACK (default_zoom_level_changed_callback),
				  icon_view);
	g_signal_connect_swapped (athena_icon_view_preferences,
				  "changed::" ATHENA_PREFERENCES_ICON_VIEW_LABELS_BESIDE_ICONS,
				  G_CALLBACK (labels_beside_icons_changed_callback),
				  icon_view);
	g_signal_connect_swapped (athena_icon_view_preferences,
				  "changed::" ATHENA_PREFERENCES_ICON_VIEW_CAPTIONS,
				  G_CALLBACK (text_attribute_names_changed_callback),
				  icon_view);

	g_signal_connect_swapped (athena_compact_view_preferences,
				  "changed::" ATHENA_PREFERENCES_COMPACT_VIEW_DEFAULT_ZOOM_LEVEL,
				  G_CALLBACK (default_zoom_level_changed_callback),
				  icon_view);
	g_signal_connect_swapped (athena_compact_view_preferences,
				  "changed::" ATHENA_PREFERENCES_COMPACT_VIEW_ALL_COLUMNS_SAME_WIDTH,
				  G_CALLBACK (all_columns_same_width_changed_callback),
				  icon_view);

	g_signal_connect_object (get_icon_container (icon_view), "handle_netscape_url",
				 G_CALLBACK (icon_view_handle_netscape_url), icon_view, 0);
	g_signal_connect_object (get_icon_container (icon_view), "handle_uri_list",
				 G_CALLBACK (icon_view_handle_uri_list), icon_view, 0);
	g_signal_connect_object (get_icon_container (icon_view), "handle_text",
				 G_CALLBACK (icon_view_handle_text), icon_view, 0);
	g_signal_connect_object (get_icon_container (icon_view), "handle_raw",
				 G_CALLBACK (icon_view_handle_raw), icon_view, 0);

	icon_view->details->clipboard_handler_id =
		g_signal_connect (athena_clipboard_monitor_get (),
		                  "clipboard_info",
		                  G_CALLBACK (icon_view_notify_clipboard_info), icon_view);
}

static AthenaView *
athena_icon_view_create (AthenaWindowSlot *slot)
{
	AthenaIconView *view;

	view = g_object_new (ATHENA_TYPE_ICON_VIEW,
			     "window-slot", slot,
			     "compact", FALSE,
			     NULL);
	return ATHENA_VIEW (view);
}

static AthenaView *
athena_compact_view_create (AthenaWindowSlot *slot)
{
	AthenaIconView *view;

	view = g_object_new (ATHENA_TYPE_ICON_VIEW,
			     "window-slot", slot,
			     "compact", TRUE,
			     NULL);
	return ATHENA_VIEW (view);
}

static gboolean
athena_icon_view_supports_uri (const char *uri,
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

#define TRANSLATE_VIEW_INFO(view_info)					\
	view_info.view_combo_label = _(view_info.view_combo_label);	\
	view_info.view_menu_label_with_mnemonic = _(view_info.view_menu_label_with_mnemonic); \
	view_info.error_label = _(view_info.error_label);		\
	view_info.startup_error_label = _(view_info.startup_error_label); \
	view_info.display_location_label = _(view_info.display_location_label); \
	

static AthenaViewInfo athena_icon_view = {
	ATHENA_ICON_VIEW_ID,
	/* translators: this is used in the view selection dropdown
	 * of navigation windows and in the preferences dialog */
	N_("Icon View"),
	/* translators: this is used in the view menu */
	N_("_Icons"),
	N_("The icon view encountered an error."),
	N_("The icon view encountered an error while starting up."),
	N_("Display this location with the icon view."),
	athena_icon_view_create,
	athena_icon_view_supports_uri
};

static AthenaViewInfo athena_compact_view = {
	FM_COMPACT_VIEW_ID,
	/* translators: this is used in the view selection dropdown
	 * of navigation windows and in the preferences dialog */
	N_("Compact View"),
	/* translators: this is used in the view menu */
	N_("_Compact"),
	N_("The compact view encountered an error."),
	N_("The compact view encountered an error while starting up."),
	N_("Display this location with the compact view."),
	athena_compact_view_create,
	athena_icon_view_supports_uri
};

gboolean
athena_icon_view_is_compact (AthenaIconView *view)
{
	return view->details->compact;
}

void
athena_icon_view_register (void)
{
	TRANSLATE_VIEW_INFO (athena_icon_view)
		athena_view_factory_register (&athena_icon_view);
}

void
athena_icon_view_compact_register (void)
{
	TRANSLATE_VIEW_INFO (athena_compact_view)
		athena_view_factory_register (&athena_compact_view);
}

