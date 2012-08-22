/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Athena
 *
 * Copyright (C) 2000 Eazel, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Authors: Darin Adler <darin@bentspoon.com>
 */

#include <config.h>
#include "athena-desktop-window.h"
#include "athena-window-private.h"
#include "athena-actions.h"

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib/gi18n.h>

#include <eel/eel-vfs-extensions.h>
#include <libathena-private/athena-file-utilities.h>
#include <libathena-private/athena-icon-names.h>
#include <libathena-private/athena-global-preferences.h>

struct AthenaDesktopWindowDetails {
	gulong size_changed_id;

	gboolean loaded;
};

G_DEFINE_TYPE (AthenaDesktopWindow, athena_desktop_window, 
	       ATHENA_TYPE_WINDOW);

static void
athena_desktop_window_dispose (GObject *obj)
{
	AthenaDesktopWindow *window = ATHENA_DESKTOP_WINDOW (obj);

	g_signal_handlers_disconnect_by_func (athena_preferences,
					      athena_desktop_window_update_directory,
					      window);

	G_OBJECT_CLASS (athena_desktop_window_parent_class)->dispose (obj);
}

static void
athena_desktop_window_constructed (GObject *obj)
{
	GtkActionGroup *action_group;
	GtkAction *action;
	AtkObject *accessible;
	AthenaDesktopWindow *window = ATHENA_DESKTOP_WINDOW (obj);
	AthenaWindow *nwindow = ATHENA_WINDOW (obj);

	G_OBJECT_CLASS (athena_desktop_window_parent_class)->constructed (obj);
	
	gtk_widget_hide (nwindow->details->statusbar);
	gtk_widget_hide (nwindow->details->menubar);

	action_group = athena_window_get_main_action_group (nwindow);

	/* Don't allow close action on desktop */
	action = gtk_action_group_get_action (action_group,
					      ATHENA_ACTION_CLOSE);
	gtk_action_set_sensitive (action, FALSE);

	/* Don't allow new tab on desktop */
	action = gtk_action_group_get_action (action_group,
					      ATHENA_ACTION_NEW_TAB);
	gtk_action_set_sensitive (action, FALSE);

	/* Set the accessible name so that it doesn't inherit the cryptic desktop URI. */
	accessible = gtk_widget_get_accessible (GTK_WIDGET (window));

	if (accessible) {
		atk_object_set_name (accessible, _("Desktop"));
	}

	/* Monitor the preference to have the desktop */
	/* point to the Unix home folder */
	g_signal_connect_swapped (athena_preferences, "changed::" ATHENA_PREFERENCES_DESKTOP_IS_HOME_DIR,
				  G_CALLBACK (athena_desktop_window_update_directory),
				  window);
}

static void
athena_desktop_window_init (AthenaDesktopWindow *window)
{
	window->details = G_TYPE_INSTANCE_GET_PRIVATE (window, ATHENA_TYPE_DESKTOP_WINDOW,
						       AthenaDesktopWindowDetails);

	gtk_window_move (GTK_WINDOW (window), 0, 0);

	/* shouldn't really be needed given our semantic type
	 * of _NET_WM_TYPE_DESKTOP, but why not
	 */
	gtk_window_set_resizable (GTK_WINDOW (window),
				  FALSE);

	g_object_set_data (G_OBJECT (window), "is_desktop_window", 
			   GINT_TO_POINTER (1));
}

static gint
athena_desktop_window_delete_event (AthenaDesktopWindow *window)
{
	/* Returning true tells GTK+ not to delete the window. */
	return TRUE;
}

void
athena_desktop_window_update_directory (AthenaDesktopWindow *window)
{
	GFile *location;

	g_assert (ATHENA_IS_DESKTOP_WINDOW (window));

	window->details->loaded = FALSE;
	location = g_file_new_for_uri (EEL_DESKTOP_URI);
	athena_window_go_to (ATHENA_WINDOW (window), location);
	window->details->loaded = TRUE;

	g_object_unref (location);
}

static void
athena_desktop_window_screen_size_changed (GdkScreen             *screen,
					     AthenaDesktopWindow *window)
{
	int width_request, height_request;

	width_request = gdk_screen_get_width (screen);
	height_request = gdk_screen_get_height (screen);
	
	g_object_set (window,
		      "width_request", width_request,
		      "height_request", height_request,
		      NULL);
}

AthenaDesktopWindow *
athena_desktop_window_new (GdkScreen *screen)
{
	AthenaDesktopWindow *window;
	int width_request, height_request;

	width_request = gdk_screen_get_width (screen);
	height_request = gdk_screen_get_height (screen);

	window = g_object_new (ATHENA_TYPE_DESKTOP_WINDOW,
			       "disable-chrome", TRUE,
			       "width_request", width_request,
			       "height_request", height_request,
			       "screen", screen,
			       NULL);

	/* Special sawmill setting*/
	gtk_window_set_wmclass (GTK_WINDOW (window), "desktop_window", "Athena");

	g_signal_connect (window, "delete_event", G_CALLBACK (athena_desktop_window_delete_event), NULL);

	/* Point window at the desktop folder.
	 * Note that athena_desktop_window_init is too early to do this.
	 */
	athena_desktop_window_update_directory (window);

	return window;
}

static void
map (GtkWidget *widget)
{
	/* Chain up to realize our children */
	GTK_WIDGET_CLASS (athena_desktop_window_parent_class)->map (widget);
	gdk_window_lower (gtk_widget_get_window (widget));
}

static void
unrealize (GtkWidget *widget)
{
	AthenaDesktopWindow *window;
	AthenaDesktopWindowDetails *details;
	GdkWindow *root_window;

	window = ATHENA_DESKTOP_WINDOW (widget);
	details = window->details;

	root_window = gdk_screen_get_root_window (
				gtk_window_get_screen (GTK_WINDOW (window)));

	gdk_property_delete (root_window,
			     gdk_atom_intern ("ATHENA_DESKTOP_WINDOW_ID", TRUE));

	if (details->size_changed_id != 0) {
		g_signal_handler_disconnect (gtk_window_get_screen (GTK_WINDOW (window)),
					     details->size_changed_id);
		details->size_changed_id = 0;
	}

	GTK_WIDGET_CLASS (athena_desktop_window_parent_class)->unrealize (widget);
}

static void
set_wmspec_desktop_hint (GdkWindow *window)
{
	GdkAtom atom;

	atom = gdk_atom_intern ("_NET_WM_WINDOW_TYPE_DESKTOP", FALSE);
        
	gdk_property_change (window,
			     gdk_atom_intern ("_NET_WM_WINDOW_TYPE", FALSE),
			     gdk_x11_xatom_to_atom (XA_ATOM), 32,
			     GDK_PROP_MODE_REPLACE, (guchar *) &atom, 1);
}

static void
set_desktop_window_id (AthenaDesktopWindow *window,
		       GdkWindow             *gdkwindow)
{
	/* Tuck the desktop windows xid in the root to indicate we own the desktop.
	 */
	Window window_xid;
	GdkWindow *root_window;

	root_window = gdk_screen_get_root_window (
				gtk_window_get_screen (GTK_WINDOW (window)));

	window_xid = GDK_WINDOW_XID (gdkwindow);

	gdk_property_change (root_window,
			     gdk_atom_intern ("ATHENA_DESKTOP_WINDOW_ID", FALSE),
			     gdk_x11_xatom_to_atom (XA_WINDOW), 32,
			     GDK_PROP_MODE_REPLACE, (guchar *) &window_xid, 1);
}

static void
realize (GtkWidget *widget)
{
	AthenaDesktopWindow *window;
	AthenaDesktopWindowDetails *details;

	window = ATHENA_DESKTOP_WINDOW (widget);
	details = window->details;

	/* Make sure we get keyboard events */
	gtk_widget_set_events (widget, gtk_widget_get_events (widget) 
			      | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
			      
	/* Do the work of realizing. */
	GTK_WIDGET_CLASS (athena_desktop_window_parent_class)->realize (widget);

	/* This is the new way to set up the desktop window */
	set_wmspec_desktop_hint (gtk_widget_get_window (widget));

	set_desktop_window_id (window, gtk_widget_get_window (widget));

	details->size_changed_id =
		g_signal_connect (gtk_window_get_screen (GTK_WINDOW (window)), "size_changed",
				  G_CALLBACK (athena_desktop_window_screen_size_changed), window);
}

static AthenaIconInfo *
real_get_icon (AthenaWindow *window,
	       AthenaWindowSlot *slot)
{
	return athena_icon_info_lookup_from_name (ATHENA_ICON_DESKTOP, 48);
}

static void
real_sync_title (AthenaWindow *window,
		 AthenaWindowSlot *slot)
{
	/* hardcode "Desktop" */
	gtk_window_set_title (GTK_WINDOW (window), _("Desktop"));
}

static void
real_window_close (AthenaWindow *window)
{
	/* stub, does nothing */
	return;
}

static void
athena_desktop_window_class_init (AthenaDesktopWindowClass *klass)
{
	GtkWidgetClass *wclass = GTK_WIDGET_CLASS (klass);
	AthenaWindowClass *nclass = ATHENA_WINDOW_CLASS (klass);
	GObjectClass *oclass = G_OBJECT_CLASS (klass);

	oclass->constructed = athena_desktop_window_constructed;
	oclass->dispose = athena_desktop_window_dispose;

	wclass->realize = realize;
	wclass->unrealize = unrealize;
	wclass->map = map;

	nclass->sync_title = real_sync_title;
	nclass->get_icon = real_get_icon;
	nclass->close = real_window_close;

	g_type_class_add_private (klass, sizeof (AthenaDesktopWindowDetails));
}

gboolean
athena_desktop_window_loaded (AthenaDesktopWindow *window)
{
	return window->details->loaded;
}
