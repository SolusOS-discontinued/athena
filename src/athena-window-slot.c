/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-window-slot.c: Athena window slot
 
   Copyright (C) 2008 Free Software Foundation, Inc.
  
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
  
   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
  
   Author: Christian Neumair <cneumair@gnome.org>
*/
#include "athena-window-slot.h"

#include "athena-desktop-window.h"
#include "athena-floating-bar.h"
#include "athena-window-private.h"
#include "athena-window-manage-views.h"

#include <glib/gi18n.h>

#include <libathena-private/athena-file.h>
#include <libathena-private/athena-file-utilities.h>
#include <libathena-private/athena-global-preferences.h>

#include <eel/eel-string.h>

G_DEFINE_TYPE (AthenaWindowSlot, athena_window_slot, GTK_TYPE_BOX);

enum {
	ACTIVE,
	INACTIVE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
query_editor_changed_callback (AthenaSearchBar *bar,
			       AthenaQuery *query,
			       gboolean reload,
			       AthenaWindowSlot *slot)
{
	AthenaDirectory *directory;

	g_assert (ATHENA_IS_FILE (slot->viewed_file));

	directory = athena_directory_get_for_file (slot->viewed_file);
	g_assert (ATHENA_IS_SEARCH_DIRECTORY (directory));

	athena_search_directory_set_query (ATHENA_SEARCH_DIRECTORY (directory),
					     query);
	if (reload) {
		athena_window_slot_reload (slot);
	}

	athena_directory_unref (directory);
}

static void
real_update_query_editor (AthenaWindowSlot *slot)
{
	AthenaDirectory *directory;
	AthenaSearchDirectory *search_directory;
	AthenaQuery *query;
	GtkWidget *query_editor;
	gboolean slot_is_active;
	AthenaWindow *window;

	window = athena_window_slot_get_window (slot);
	query_editor = NULL;
	slot_is_active = (slot == athena_window_get_active_slot (window));

	directory = athena_directory_get (slot->location);
	if (ATHENA_IS_SEARCH_DIRECTORY (directory)) {
		search_directory = ATHENA_SEARCH_DIRECTORY (directory);

		if (athena_search_directory_is_saved_search (search_directory)) {
			query_editor = athena_query_editor_new (TRUE);
			athena_window_pane_sync_search_widgets (slot->pane);
		} else {
			query_editor = athena_query_editor_new_with_bar (FALSE,
									   slot_is_active,
									   ATHENA_SEARCH_BAR (slot->pane->search_bar),
									   slot);
		}
	}

	slot->query_editor = ATHENA_QUERY_EDITOR (query_editor);

	if (query_editor != NULL) {
		g_signal_connect_object (query_editor, "changed",
					 G_CALLBACK (query_editor_changed_callback), slot, 0);
		
		query = athena_search_directory_get_query (search_directory);
		if (query != NULL) {
			athena_query_editor_set_query (ATHENA_QUERY_EDITOR (query_editor),
							 query);
			g_object_unref (query);
		} else {
			athena_query_editor_set_default_query (ATHENA_QUERY_EDITOR (query_editor));
		}

		athena_window_slot_add_extra_location_widget (slot, query_editor);
		gtk_widget_show (query_editor);
		athena_query_editor_grab_focus (ATHENA_QUERY_EDITOR (query_editor));
	}

	athena_directory_unref (directory);
}

static void
real_active (AthenaWindowSlot *slot)
{
	AthenaWindow *window;
	AthenaWindowPane *pane;
	int page_num;

	window = athena_window_slot_get_window (slot);
	pane = slot->pane;
	page_num = gtk_notebook_page_num (GTK_NOTEBOOK (pane->notebook),
					  GTK_WIDGET (slot));
	g_assert (page_num >= 0);

	gtk_notebook_set_current_page (GTK_NOTEBOOK (pane->notebook), page_num);

	/* sync window to new slot */
	athena_window_sync_status (window);
	athena_window_sync_allow_stop (window, slot);
	athena_window_sync_title (window, slot);
	athena_window_sync_zoom_widgets (window);
	athena_window_pane_sync_location_widgets (slot->pane);
	athena_window_pane_sync_search_widgets (slot->pane);

	if (slot->viewed_file != NULL) {
		athena_window_load_view_as_menus (window);
		athena_window_load_extension_menus (window);
	}
}

static void
real_inactive (AthenaWindowSlot *slot)
{
	AthenaWindow *window;

	window = athena_window_slot_get_window (slot);
	g_assert (slot == athena_window_get_active_slot (window));
}

static void
floating_bar_action_cb (AthenaFloatingBar *floating_bar,
			gint action,
			AthenaWindowSlot *slot)
{
	if (action == ATHENA_FLOATING_BAR_ACTION_ID_STOP) {
		athena_window_slot_stop_loading (slot);
	}
}

static void
athena_window_slot_init (AthenaWindowSlot *slot)
{
	GtkWidget *extras_vbox;

	gtk_orientable_set_orientation (GTK_ORIENTABLE (slot),
					GTK_ORIENTATION_VERTICAL);
	gtk_widget_show (GTK_WIDGET (slot));

	extras_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
	slot->extra_location_widgets = extras_vbox;
	gtk_box_pack_start (GTK_BOX (slot), extras_vbox, FALSE, FALSE, 0);
	gtk_widget_show (extras_vbox);

	slot->view_overlay = gtk_overlay_new ();
	gtk_widget_add_events (slot->view_overlay,
			       GDK_ENTER_NOTIFY_MASK |
			       GDK_LEAVE_NOTIFY_MASK);
	gtk_box_pack_start (GTK_BOX (slot), slot->view_overlay, TRUE, TRUE, 0);
	gtk_widget_show (slot->view_overlay);

	slot->floating_bar = athena_floating_bar_new ("", FALSE);
	gtk_widget_set_halign (slot->floating_bar, GTK_ALIGN_END);
	gtk_widget_set_valign (slot->floating_bar, GTK_ALIGN_END);
	gtk_overlay_add_overlay (GTK_OVERLAY (slot->view_overlay),
				 slot->floating_bar);

	g_signal_connect (slot->floating_bar, "action",
			  G_CALLBACK (floating_bar_action_cb), slot);

	slot->title = g_strdup (_("Loading..."));
}

static void
athena_window_slot_dispose (GObject *object)
{
	AthenaWindowSlot *slot;
	GtkWidget *widget;

	slot = ATHENA_WINDOW_SLOT (object);

	athena_window_slot_clear_forward_list (slot);
	athena_window_slot_clear_back_list (slot);

	if (slot->content_view) {
		widget = GTK_WIDGET (slot->content_view);
		gtk_widget_destroy (widget);
		g_object_unref (slot->content_view);
		slot->content_view = NULL;
	}

	if (slot->new_content_view) {
		widget = GTK_WIDGET (slot->new_content_view);
		gtk_widget_destroy (widget);
		g_object_unref (slot->new_content_view);
		slot->new_content_view = NULL;
	}

	if (slot->set_status_timeout_id != 0) {
		g_source_remove (slot->set_status_timeout_id);
		slot->set_status_timeout_id = 0;
	}

	if (slot->loading_timeout_id != 0) {
		g_source_remove (slot->loading_timeout_id);
		slot->loading_timeout_id = 0;
	}

	athena_window_slot_set_viewed_file (slot, NULL);
	/* TODO? why do we unref here? the file is NULL.
	 * It was already here before the slot move, though */
	athena_file_unref (slot->viewed_file);

	if (slot->location) {
		/* TODO? why do we ref here, instead of unreffing?
		 * It was already here before the slot migration, though */
		g_object_ref (slot->location);
	}

	g_list_free_full (slot->pending_selection, g_object_unref);
	slot->pending_selection = NULL;

	g_clear_object (&slot->current_location_bookmark);
	g_clear_object (&slot->last_location_bookmark);

	if (slot->find_mount_cancellable != NULL) {
		g_cancellable_cancel (slot->find_mount_cancellable);
		slot->find_mount_cancellable = NULL;
	}

	slot->pane = NULL;

	g_free (slot->title);
	slot->title = NULL;

	g_free (slot->status_text);
	slot->status_text = NULL;

	G_OBJECT_CLASS (athena_window_slot_parent_class)->dispose (object);
}

static void
athena_window_slot_class_init (AthenaWindowSlotClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);

	klass->active = real_active;
	klass->inactive = real_inactive;

	oclass->dispose = athena_window_slot_dispose;

	signals[ACTIVE] =
		g_signal_new ("active",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (AthenaWindowSlotClass, active),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);

	signals[INACTIVE] =
		g_signal_new ("inactive",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (AthenaWindowSlotClass, inactive),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);
}

GFile *
athena_window_slot_get_location (AthenaWindowSlot *slot)
{
	g_assert (slot != NULL);

	if (slot->location != NULL) {
		return g_object_ref (slot->location);
	}
	return NULL;
}

char *
athena_window_slot_get_location_uri (AthenaWindowSlot *slot)
{
	g_assert (ATHENA_IS_WINDOW_SLOT (slot));

	if (slot->location) {
		return g_file_get_uri (slot->location);
	}
	return NULL;
}

void
athena_window_slot_make_hosting_pane_active (AthenaWindowSlot *slot)
{
	g_assert (ATHENA_IS_WINDOW_PANE (slot->pane));
	
	athena_window_set_active_slot (athena_window_slot_get_window (slot),
					 slot);
}

AthenaWindow *
athena_window_slot_get_window (AthenaWindowSlot *slot)
{
	g_assert (ATHENA_IS_WINDOW_SLOT (slot));
	return slot->pane->window;
}

/* athena_window_slot_update_title:
 * 
 * Re-calculate the slot title.
 * Called when the location or view has changed.
 * @slot: The AthenaWindowSlot in question.
 * 
 */
void
athena_window_slot_update_title (AthenaWindowSlot *slot)
{
	AthenaWindow *window;
	char *title;
	gboolean do_sync = FALSE;

	title = athena_compute_title_for_location (slot->location);
	window = athena_window_slot_get_window (slot);

	if (g_strcmp0 (title, slot->title) != 0) {
		do_sync = TRUE;

		g_free (slot->title);
		slot->title = title;
		title = NULL;
	}

	if (strlen (slot->title) > 0 &&
	    slot->current_location_bookmark != NULL) {
		do_sync = TRUE;
	}

	if (do_sync) {
		athena_window_sync_title (window, slot);
	}

	if (title != NULL) {
		g_free (title);
	}
}

/* athena_window_slot_update_icon:
 * 
 * Re-calculate the slot icon
 * Called when the location or view or icon set has changed.
 * @slot: The AthenaWindowSlot in question.
 */
void
athena_window_slot_update_icon (AthenaWindowSlot *slot)
{
	AthenaWindow *window;
	AthenaIconInfo *info;
	const char *icon_name;
	GdkPixbuf *pixbuf;

	window = athena_window_slot_get_window (slot);
	info = ATHENA_WINDOW_CLASS (G_OBJECT_GET_CLASS (window))->get_icon (window, slot);

	icon_name = NULL;
	if (info) {
		icon_name = athena_icon_info_get_used_name (info);
		if (icon_name != NULL) {
			/* Gtk+ doesn't short circuit this (yet), so avoid lots of work
			 * if we're setting to the same icon. This happens a lot e.g. when
			 * the trash directory changes due to the file count changing.
			 */
			if (g_strcmp0 (icon_name, gtk_window_get_icon_name (GTK_WINDOW (window))) != 0) {			
				gtk_window_set_icon_name (GTK_WINDOW (window), icon_name);
			}
		} else {
			pixbuf = athena_icon_info_get_pixbuf_nodefault (info);
			
			if (pixbuf) {
				gtk_window_set_icon (GTK_WINDOW (window), pixbuf);
				g_object_unref (pixbuf);
			} 
		}
		
		g_object_unref (info);
	}
}

void
athena_window_slot_set_content_view_widget (AthenaWindowSlot *slot,
					      AthenaView *new_view)
{
	AthenaWindow *window;
	GtkWidget *widget;

	window = athena_window_slot_get_window (slot);

	if (slot->content_view != NULL) {
		/* disconnect old view */
		athena_window_disconnect_content_view (window, slot->content_view);

		widget = GTK_WIDGET (slot->content_view);
		gtk_widget_destroy (widget);
		g_object_unref (slot->content_view);
		slot->content_view = NULL;
	}

	if (new_view != NULL) {
		widget = GTK_WIDGET (new_view);
		gtk_container_add (GTK_CONTAINER (slot->view_overlay), widget);
		gtk_widget_show (widget);

		slot->content_view = new_view;
		g_object_ref (slot->content_view);

		/* connect new view */
		athena_window_connect_content_view (window, new_view);
	}
}

void
athena_window_slot_set_allow_stop (AthenaWindowSlot *slot,
				     gboolean allow)
{
	AthenaWindow *window;

	g_assert (ATHENA_IS_WINDOW_SLOT (slot));

	slot->allow_stop = allow;

	window = athena_window_slot_get_window (slot);
	athena_window_sync_allow_stop (window, slot);
}

static void
real_slot_set_short_status (AthenaWindowSlot *slot,
			    const gchar *status)
{
	
	gboolean show_statusbar;
	gboolean disable_chrome;

	athena_floating_bar_cleanup_actions (ATHENA_FLOATING_BAR (slot->floating_bar));
	athena_floating_bar_set_show_spinner (ATHENA_FLOATING_BAR (slot->floating_bar),
						FALSE);

	show_statusbar = g_settings_get_boolean (athena_window_state,
						 ATHENA_WINDOW_STATE_START_WITH_STATUS_BAR);

	g_object_get (athena_window_slot_get_window (slot),
		      "disable-chrome", &disable_chrome,
		      NULL);

	if (status == NULL || show_statusbar || disable_chrome) {
		gtk_widget_hide (slot->floating_bar);
		return;
	}

	athena_floating_bar_set_label (ATHENA_FLOATING_BAR (slot->floating_bar), status);
	gtk_widget_show (slot->floating_bar);
}

typedef struct {
	gchar *status;
	AthenaWindowSlot *slot;
} SetStatusData;

static void
set_status_data_free (gpointer data)
{
	SetStatusData *status_data = data;

	g_free (status_data->status);

	g_slice_free (SetStatusData, data);
}

static gboolean
set_status_timeout_cb (gpointer data)
{
	SetStatusData *status_data = data;

	status_data->slot->set_status_timeout_id = 0;
	real_slot_set_short_status (status_data->slot, status_data->status);

	return FALSE;
}

static void
set_floating_bar_status (AthenaWindowSlot *slot,
			 const gchar *status)
{
	GtkSettings *settings;
	gint double_click_time;
	SetStatusData *status_data;

	if (slot->set_status_timeout_id != 0) {
		g_source_remove (slot->set_status_timeout_id);
		slot->set_status_timeout_id = 0;
	}

	settings = gtk_settings_get_for_screen (gtk_widget_get_screen (GTK_WIDGET (slot->content_view)));
	g_object_get (settings,
		      "gtk-double-click-time", &double_click_time,
		      NULL);

	status_data = g_slice_new0 (SetStatusData);
	status_data->status = g_strdup (status);
	status_data->slot = slot;

	/* waiting for half of the double-click-time before setting
	 * the status seems to be a good approximation of not setting it
	 * too often and not delaying the statusbar too much.
	 */
	slot->set_status_timeout_id =
		g_timeout_add_full (G_PRIORITY_DEFAULT,
				    (guint) (double_click_time / 2),
				    set_status_timeout_cb,
				    status_data,
				    set_status_data_free);
}

void
athena_window_slot_set_status (AthenaWindowSlot *slot,
				 const char *status,
				 const char *short_status)
{
	AthenaWindow *window;

	g_assert (ATHENA_IS_WINDOW_SLOT (slot));

	g_free (slot->status_text);
	slot->status_text = g_strdup (status);

	if (slot->content_view != NULL) {
		set_floating_bar_status (slot, short_status);
	}

	window = athena_window_slot_get_window (slot);
	if (slot == athena_window_get_active_slot (window)) {
		athena_window_sync_status (window);
	}
}

/* athena_window_slot_update_query_editor:
 * 
 * Update the query editor.
 * Called when the location has changed.
 *
 * @slot: The AthenaWindowSlot in question.
 */
void
athena_window_slot_update_query_editor (AthenaWindowSlot *slot)
{
	if (slot->query_editor != NULL) {
		gtk_widget_destroy (GTK_WIDGET (slot->query_editor));
		g_assert (slot->query_editor == NULL);
	}

	real_update_query_editor (slot);

	eel_add_weak_pointer (&slot->query_editor);
}

static void
remove_all (GtkWidget *widget,
	    gpointer data)
{
	GtkContainer *container;
	container = GTK_CONTAINER (data);

	gtk_container_remove (container, widget);
}

void
athena_window_slot_remove_extra_location_widgets (AthenaWindowSlot *slot)
{
	gtk_container_foreach (GTK_CONTAINER (slot->extra_location_widgets),
			       remove_all,
			       slot->extra_location_widgets);
	gtk_widget_hide (slot->extra_location_widgets);
}

void
athena_window_slot_add_extra_location_widget (AthenaWindowSlot *slot,
						GtkWidget *widget)
{
	gtk_box_pack_start (GTK_BOX (slot->extra_location_widgets),
			    widget, TRUE, TRUE, 0);
	gtk_widget_show (slot->extra_location_widgets);
}

/* returns either the pending or the actual current uri */
char *
athena_window_slot_get_current_uri (AthenaWindowSlot *slot)
{
	if (slot->pending_location != NULL) {
		return g_file_get_uri (slot->pending_location);
	}

	if (slot->location != NULL) {
		return g_file_get_uri (slot->location);
	}

	g_assert_not_reached ();
	return NULL;
}

AthenaView *
athena_window_slot_get_current_view (AthenaWindowSlot *slot)
{
	if (slot->content_view != NULL) {
		return slot->content_view;
	} else if (slot->new_content_view) {
		return slot->new_content_view;
	}

	return NULL;
}

void
athena_window_slot_go_home (AthenaWindowSlot *slot,
			      gboolean new_tab)
{			      
	GFile *home;
	AthenaWindowOpenFlags flags;

	g_return_if_fail (ATHENA_IS_WINDOW_SLOT (slot));

	if (new_tab) {
		flags = ATHENA_WINDOW_OPEN_FLAG_NEW_TAB;
	} else {
		flags = 0;
	}

	home = g_file_new_for_path (g_get_home_dir ());
	athena_window_slot_open_location_full (slot, home, 
						 flags, NULL, NULL, NULL);
	g_object_unref (home);
}

void
athena_window_slot_go_up (AthenaWindowSlot *slot,
			    gboolean close_behind,
			    gboolean new_tab)
{
	GFile *parent;
	AthenaWindowOpenFlags flags;

	if (slot->location == NULL) {
		return;
	}

	parent = g_file_get_parent (slot->location);
	if (parent == NULL) {
		return;
	}

	flags = 0;
	if (close_behind) {
		flags |= ATHENA_WINDOW_OPEN_FLAG_CLOSE_BEHIND;
	}
	if (new_tab) {
		flags |= ATHENA_WINDOW_OPEN_FLAG_NEW_TAB;
	}

	athena_window_slot_open_location (slot, parent,
					    flags,
					    NULL);

	g_object_unref (parent);
}

void
athena_window_slot_clear_forward_list (AthenaWindowSlot *slot)
{
	g_assert (ATHENA_IS_WINDOW_SLOT (slot));

	g_list_free_full (slot->forward_list, g_object_unref);
	slot->forward_list = NULL;
}

void
athena_window_slot_clear_back_list (AthenaWindowSlot *slot)
{
	g_assert (ATHENA_IS_WINDOW_SLOT (slot));

	g_list_free_full (slot->back_list, g_object_unref);
	slot->back_list = NULL;
}

gboolean
athena_window_slot_should_close_with_mount (AthenaWindowSlot *slot,
					      GMount *mount)
{
	GFile *mount_location;
	gboolean close_with_mount;

	mount_location = g_mount_get_root (mount);
	close_with_mount = 
		g_file_has_prefix (ATHENA_WINDOW_SLOT (slot)->location, mount_location) ||
		g_file_equal (ATHENA_WINDOW_SLOT (slot)->location, mount_location);

	g_object_unref (mount_location);

	return close_with_mount;
}

AthenaWindowSlot *
athena_window_slot_new (AthenaWindowPane *pane)
{
	AthenaWindowSlot *slot;

	slot = g_object_new (ATHENA_TYPE_WINDOW_SLOT, NULL);
	slot->pane = pane;

	return slot;
}
