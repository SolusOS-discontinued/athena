/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * athena-application: main Athena application class.
 *
 * Copyright (C) 1999, 2000 Red Hat, Inc.
 * Copyright (C) 2000, 2001 Eazel, Inc.
 * Copyright (C) 2010, Cosimo Cecchi <cosimoc@gnome.org>
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
 * Authors: Elliot Lee <sopwith@redhat.com>,
 *          Darin Adler <darin@bentspoon.com>
 *          Cosimo Cecchi <cosimoc@gnome.org>
 *
 */

#include <config.h>

#include "athena-application.h"

#if ENABLE_EMPTY_VIEW
#include "athena-empty-view.h"
#endif /* ENABLE_EMPTY_VIEW */

#include "athena-desktop-icon-view.h"
#include "athena-desktop-window.h"
#include "athena-freedesktop-dbus.h"
#include "athena-icon-view.h"
#include "athena-image-properties-page.h"
#include "athena-list-view.h"
#include "athena-previewer.h"
#include "athena-progress-ui-handler.h"
#include "athena-self-check-functions.h"
#include "athena-window.h"
#include "athena-window-bookmarks.h"
#include "athena-window-manage-views.h"
#include "athena-window-private.h"
#include "athena-window-slot.h"

#include <libathena-private/athena-dbus-manager.h>
#include <libathena-private/athena-desktop-link-monitor.h>
#include <libathena-private/athena-directory-private.h>
#include <libathena-private/athena-file-utilities.h>
#include <libathena-private/athena-file-operations.h>
#include <libathena-private/athena-global-preferences.h>
#include <libathena-private/athena-lib-self-check-functions.h>
#include <libathena-private/athena-module.h>
#include <libathena-private/athena-signaller.h>
#include <libathena-private/athena-ui-utilities.h>
#include <libathena-private/athena-undo-manager.h>
#include <libathena-extension/athena-menu-provider.h>

#define DEBUG_FLAG ATHENA_DEBUG_APPLICATION
#include <libathena-private/athena-debug.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <eel/eel-gtk-extensions.h>
#include <eel/eel-stock-dialogs.h>
#include <libnotify/notify.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

/* Keep window from shrinking down ridiculously small; numbers are somewhat arbitrary */
#define APPLICATION_WINDOW_MIN_WIDTH	300
#define APPLICATION_WINDOW_MIN_HEIGHT	100

#define START_STATE_CONFIG "start-state"

#define ATHENA_ACCEL_MAP_SAVE_DELAY 30

static AthenaApplication *singleton = NULL;

/* Keeps track of all the desktop windows. */
static GList *athena_application_desktop_windows;

/* The saving of the accelerator map was requested  */
static gboolean save_of_accel_map_requested = FALSE;

static void     desktop_changed_callback          (gpointer                  user_data);
static void     mount_removed_callback            (GVolumeMonitor            *monitor,
						   GMount                    *mount,
						   AthenaApplication       *application);
static void     mount_added_callback              (GVolumeMonitor            *monitor,
						   GMount                    *mount,
						   AthenaApplication       *application);

G_DEFINE_TYPE (AthenaApplication, athena_application, GTK_TYPE_APPLICATION);

struct _AthenaApplicationPriv {
	GVolumeMonitor *volume_monitor;
	AthenaProgressUIHandler *progress_handler;

	gboolean no_desktop;
	gchar *geometry;
};

static gboolean
check_required_directories (AthenaApplication *application)
{
	char *user_directory;
	char *desktop_directory;
	GSList *directories;
	gboolean ret;

	g_assert (ATHENA_IS_APPLICATION (application));

	ret = TRUE;

	user_directory = athena_get_user_directory ();
	desktop_directory = athena_get_desktop_directory ();

	directories = NULL;

	if (!g_file_test (user_directory, G_FILE_TEST_IS_DIR)) {
		directories = g_slist_prepend (directories, user_directory);
	}

	if (!g_file_test (desktop_directory, G_FILE_TEST_IS_DIR)) {
		directories = g_slist_prepend (directories, desktop_directory);
	}

	if (directories != NULL) {
		int failed_count;
		GString *directories_as_string;
		GSList *l;
		char *error_string;
		const char *detail_string;
		GtkDialog *dialog;

		ret = FALSE;

		failed_count = g_slist_length (directories);

		directories_as_string = g_string_new ((const char *)directories->data);
		for (l = directories->next; l != NULL; l = l->next) {
			g_string_append_printf (directories_as_string, ", %s", (const char *)l->data);
		}

		if (failed_count == 1) {
			error_string = g_strdup_printf (_("Athena could not create the required folder \"%s\"."),
							directories_as_string->str);
			detail_string = _("Before running Athena, please create the following folder, or "
					  "set permissions such that Athena can create it.");
		} else {
			error_string = g_strdup_printf (_("Athena could not create the following required folders: "
							  "%s."), directories_as_string->str);
			detail_string = _("Before running Athena, please create these folders, or "
					  "set permissions such that Athena can create them.");
		}

		dialog = eel_show_error_dialog (error_string, detail_string, NULL);
		/* We need the main event loop so the user has a chance to see the dialog. */
		gtk_application_add_window (GTK_APPLICATION (application),
					    GTK_WINDOW (dialog));

		g_string_free (directories_as_string, TRUE);
		g_free (error_string);
	}

	g_slist_free (directories);
	g_free (user_directory);
	g_free (desktop_directory);

	return ret;
}

static void
menu_provider_items_updated_handler (AthenaMenuProvider *provider, GtkWidget* parent_window, gpointer data)
{

	g_signal_emit_by_name (athena_signaller_get_current (),
			       "popup_menu_changed");
}

static void
menu_provider_init_callback (void)
{
        GList *providers;
        GList *l;

        providers = athena_module_get_extensions_for_type (ATHENA_TYPE_MENU_PROVIDER);

        for (l = providers; l != NULL; l = l->next) {
                AthenaMenuProvider *provider = ATHENA_MENU_PROVIDER (l->data);

		g_signal_connect_after (G_OBJECT (provider), "items_updated",
                           (GCallback)menu_provider_items_updated_handler,
                           NULL);
        }

        athena_module_extension_list_free (providers);
}

static void
mark_desktop_files_trusted (void)
{
	char *do_once_file;
	GFile *f, *c;
	GFileEnumerator *e;
	GFileInfo *info;
	const char *name;
	int fd;
	
	do_once_file = g_build_filename (g_get_user_data_dir (),
					 ".converted-launchers", NULL);

	if (g_file_test (do_once_file, G_FILE_TEST_EXISTS)) {
		goto out;
	}

	f = athena_get_desktop_location ();
	e = g_file_enumerate_children (f,
				       G_FILE_ATTRIBUTE_STANDARD_TYPE ","
				       G_FILE_ATTRIBUTE_STANDARD_NAME ","
				       G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE
				       ,
				       G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
				       NULL, NULL);
	if (e == NULL) {
		goto out2;
	}
	
	while ((info = g_file_enumerator_next_file (e, NULL, NULL)) != NULL) {
		name = g_file_info_get_name (info);
		
		if (g_str_has_suffix (name, ".desktop") &&
		    !g_file_info_get_attribute_boolean (info, G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE)) {
			c = g_file_get_child (f, name);
			athena_file_mark_desktop_file_trusted (c,
								 NULL, FALSE,
								 NULL, NULL);
			g_object_unref (c);
		}
		g_object_unref (info);
	}
	
	g_object_unref (e);
 out2:
	fd = g_creat (do_once_file, 0666);
	close (fd);
	
	g_object_unref (f);
 out:	
	g_free (do_once_file);
}

static void
do_upgrades_once (AthenaApplication *self)
{
	char *metafile_dir, *updated, *athena_dir, *xdg_dir;
	const gchar *message;
	int fd, res;

	if (!self->priv->no_desktop) {
		mark_desktop_files_trusted ();
	}

	metafile_dir = g_build_filename (g_get_home_dir (),
					 ".athena/metafiles", NULL);
	if (g_file_test (metafile_dir, G_FILE_TEST_IS_DIR)) {
		updated = g_build_filename (metafile_dir, "migrated-to-gvfs", NULL);
		if (!g_file_test (updated, G_FILE_TEST_EXISTS)) {
			g_spawn_command_line_async (LIBEXECDIR"/athena-convert-metadata --quiet", NULL);
			fd = g_creat (updated, 0600);
			if (fd != -1) {
				close (fd);
			}
		}
		g_free (updated);
	}
	g_free (metafile_dir);

	athena_dir = g_build_filename (g_get_home_dir (),
					 ".athena", NULL);
	xdg_dir = athena_get_user_directory ();
	if (g_file_test (athena_dir, G_FILE_TEST_IS_DIR)) {
		/* test if we already attempted to migrate first */
		updated = g_build_filename (athena_dir, "DEPRECATED-DIRECTORY", NULL);
		message = _("Athena 3.0 deprecated this directory and tried migrating "
			    "this configuration to ~/.config/athena");
		if (!g_file_test (updated, G_FILE_TEST_EXISTS)) {
			/* rename() works fine if the destination directory is
			 * empty.
			 */
			res = g_rename (athena_dir, xdg_dir);

			if (res == -1) {
				fd = g_creat (updated, 0600);
				if (fd != -1) {
					res = write (fd, message, strlen (message));
					close (fd);
				}
			}
		}

		g_free (updated);
	}

	g_free (athena_dir);
	g_free (xdg_dir);
}

static void 
selection_get_cb (GtkWidget          *widget,
		  GtkSelectionData   *selection_data,
		  guint               info,
		  guint               time)
{
	/* No extra targets atm */
}

static GtkWidget *
get_desktop_manager_selection (GdkDisplay *display, int screen)
{
	char selection_name[32];
	GdkAtom selection_atom;
	Window selection_owner;
	GtkWidget *selection_widget;

	g_snprintf (selection_name, sizeof (selection_name), "_NET_DESKTOP_MANAGER_S%d", screen);
	selection_atom = gdk_atom_intern (selection_name, FALSE);

	selection_owner = XGetSelectionOwner (GDK_DISPLAY_XDISPLAY (display),
					      gdk_x11_atom_to_xatom_for_display (display, 
										 selection_atom));
	if (selection_owner != None) {
		return NULL;
	}
	
	selection_widget = gtk_invisible_new_for_screen (gdk_display_get_screen (display, screen));
	/* We need this for gdk_x11_get_server_time() */
	gtk_widget_add_events (selection_widget, GDK_PROPERTY_CHANGE_MASK);

	if (gtk_selection_owner_set_for_display (display,
						 selection_widget,
						 selection_atom,
						 gdk_x11_get_server_time (gtk_widget_get_window (selection_widget)))) {
		
		g_signal_connect (selection_widget, "selection_get",
				  G_CALLBACK (selection_get_cb), NULL);
		return selection_widget;
	}

	gtk_widget_destroy (selection_widget);
	
	return NULL;
}

static void
desktop_unrealize_cb (GtkWidget        *widget,
		      GtkWidget        *selection_widget)
{
	gtk_widget_destroy (selection_widget);
}

static gboolean
selection_clear_event_cb (GtkWidget	        *widget,
			  GdkEventSelection     *event,
			  AthenaDesktopWindow *window)
{
	gtk_widget_destroy (GTK_WIDGET (window));
	
	athena_application_desktop_windows =
		g_list_remove (athena_application_desktop_windows, window);

	return TRUE;
}

static void
athena_application_create_desktop_windows (AthenaApplication *application)
{
	GdkDisplay *display;
	AthenaDesktopWindow *window;
	GtkWidget *selection_widget;
	int screens, i;

	display = gdk_display_get_default ();
	screens = gdk_display_get_n_screens (display);

	for (i = 0; i < screens; i++) {

		DEBUG ("Creating a desktop window for screen %d", i);
		
		selection_widget = get_desktop_manager_selection (display, i);
		if (selection_widget != NULL) {
			window = athena_desktop_window_new (gdk_display_get_screen (display, i));

			g_signal_connect (selection_widget, "selection_clear_event",
					  G_CALLBACK (selection_clear_event_cb), window);
			
			g_signal_connect (window, "unrealize",
					  G_CALLBACK (desktop_unrealize_cb), selection_widget);
			
			/* We realize it immediately so that the ATHENA_DESKTOP_WINDOW_ID
			   property is set so gnome-settings-daemon doesn't try to set the
			   background. And we do a gdk_flush() to be sure X gets it. */
			gtk_widget_realize (GTK_WIDGET (window));
			gdk_flush ();

			athena_application_desktop_windows =
				g_list_prepend (athena_application_desktop_windows, window);

			gtk_application_add_window (GTK_APPLICATION (application),
						    GTK_WINDOW (window));
		}
	}
}

static void
athena_application_open_desktop (AthenaApplication *application)
{
	if (athena_application_desktop_windows == NULL) {
		athena_application_create_desktop_windows (application);
	}
}

static void
athena_application_close_desktop (void)
{
	if (athena_application_desktop_windows != NULL) {
		g_list_foreach (athena_application_desktop_windows,
				(GFunc) gtk_widget_destroy, NULL);
		g_list_free (athena_application_desktop_windows);
		athena_application_desktop_windows = NULL;
	}
}

void
athena_application_close_all_windows (AthenaApplication *self)
{
	GList *list_copy;
	GList *l;
	
	list_copy = g_list_copy (gtk_application_get_windows (GTK_APPLICATION (self)));
	for (l = list_copy; l != NULL; l = l->next) {
		AthenaWindow *window;
		
		window = ATHENA_WINDOW (l->data);
		athena_window_close (window);
	}
	g_list_free (list_copy);
}

static gboolean
athena_window_delete_event_callback (GtkWidget *widget,
				       GdkEvent *event,
				       gpointer user_data)
{
	AthenaWindow *window;

	window = ATHENA_WINDOW (widget);
	athena_window_close (window);

	return TRUE;
}				       


static AthenaWindow *
create_window (AthenaApplication *application,
	       GdkScreen *screen)
{
	AthenaWindow *window;
	
	g_return_val_if_fail (ATHENA_IS_APPLICATION (application), NULL);
	
	window = g_object_new (ATHENA_TYPE_WINDOW,
			       "screen", screen,
			       NULL);

	g_signal_connect_data (window, "delete_event",
			       G_CALLBACK (athena_window_delete_event_callback), NULL, NULL,
			       G_CONNECT_AFTER);

	gtk_application_add_window (GTK_APPLICATION (application),
				    GTK_WINDOW (window));

	/* Do not yet show the window. It will be shown later on if it can
	 * successfully display its initial URI. Otherwise it will be destroyed
	 * without ever having seen the light of day.
	 */

	return window;
}

static gboolean
another_navigation_window_already_showing (AthenaApplication *application,
					   AthenaWindow *the_window)
{
	GList *list, *item;
	
	list = gtk_application_get_windows (GTK_APPLICATION (application));
	for (item = list; item != NULL; item = item->next) {
		if (item->data != the_window) {
			return TRUE;
		}
	}
	
	return FALSE;
}

AthenaWindow *
athena_application_create_window (AthenaApplication *application,
				    GdkScreen           *screen)
{
	AthenaWindow *window;
	char *geometry_string;
	gboolean maximized;

	g_return_val_if_fail (ATHENA_IS_APPLICATION (application), NULL);

	window = create_window (application, screen);

	maximized = g_settings_get_boolean
		(athena_window_state, ATHENA_WINDOW_STATE_MAXIMIZED);
	if (maximized) {
		gtk_window_maximize (GTK_WINDOW (window));
	} else {
		gtk_window_unmaximize (GTK_WINDOW (window));
	}

	geometry_string = g_settings_get_string
		(athena_window_state, ATHENA_WINDOW_STATE_GEOMETRY);
	if (geometry_string != NULL &&
	    geometry_string[0] != 0) {
		/* Ignore saved window position if a window with the same
		 * location is already showing. That way the two windows
		 * wont appear at the exact same location on the screen.
		 */
		eel_gtk_window_set_initial_geometry_from_string 
			(GTK_WINDOW (window), 
			 geometry_string,
			 ATHENA_WINDOW_MIN_WIDTH,
			 ATHENA_WINDOW_MIN_HEIGHT,
			 another_navigation_window_already_showing (application, window));
	}
	g_free (geometry_string);

	DEBUG ("Creating a new navigation window");
	
	return window;
}

/* callback for showing or hiding the desktop based on the user's preference */
static void
desktop_changed_callback (gpointer user_data)
{
	AthenaApplication *application;

	application = ATHENA_APPLICATION (user_data);
	if (g_settings_get_boolean (gnome_background_preferences, ATHENA_PREFERENCES_SHOW_DESKTOP)) {
		athena_application_open_desktop (application);
	} else {
		athena_application_close_desktop ();
	}
}

static gboolean
window_can_be_closed (AthenaWindow *window)
{
	if (!ATHENA_IS_DESKTOP_WINDOW (window)) {
		return TRUE;
	}
	
	return FALSE;
}

static void
mount_added_callback (GVolumeMonitor *monitor,
		      GMount *mount,
		      AthenaApplication *application)
{
	AthenaDirectory *directory;
	GFile *root;
	gchar *uri;
		
	root = g_mount_get_root (mount);
	uri = g_file_get_uri (root);

	DEBUG ("Added mount at uri %s", uri);
	g_free (uri);
	
	directory = athena_directory_get_existing (root);
	g_object_unref (root);
	if (directory != NULL) {
		athena_directory_force_reload (directory);
		athena_directory_unref (directory);
	}
}

/* Called whenever a mount is unmounted. Check and see if there are
 * any windows open displaying contents on the mount. If there are,
 * close them.  It would also be cool to save open window and position
 * info.
 */
static void
mount_removed_callback (GVolumeMonitor *monitor,
			GMount *mount,
			AthenaApplication *application)
{
	GList *window_list, *node, *close_list;
	AthenaWindow *window;
	AthenaWindowSlot *slot;
	AthenaWindowSlot *force_no_close_slot;
	GFile *root, *computer;
	gchar *uri;
	gint n_slots;

	close_list = NULL;
	force_no_close_slot = NULL;
	n_slots = 0;

	/* Check and see if any of the open windows are displaying contents from the unmounted mount */
	window_list = gtk_application_get_windows (GTK_APPLICATION (application));

	root = g_mount_get_root (mount);
	uri = g_file_get_uri (root);

	DEBUG ("Removed mount at uri %s", uri);
	g_free (uri);

	/* Construct a list of windows to be closed. Do not add the non-closable windows to the list. */
	for (node = window_list; node != NULL; node = node->next) {
		window = ATHENA_WINDOW (node->data);
		if (window != NULL && window_can_be_closed (window)) {
			GList *l;
			GList *lp;

			for (lp = window->details->panes; lp != NULL; lp = lp->next) {
				AthenaWindowPane *pane;
				pane = (AthenaWindowPane*) lp->data;
				for (l = pane->slots; l != NULL; l = l->next) {
					slot = l->data;
					n_slots++;
					if (athena_window_slot_should_close_with_mount (slot, mount)) {
						close_list = g_list_prepend (close_list, slot);
					}
				} /* for all slots */
			} /* for all panes */
		}
	}

	if ((athena_application_desktop_windows == NULL) &&
	    (close_list != NULL) &&
	    (g_list_length (close_list) == n_slots)) {
		/* We are trying to close all open slots. Keep one navigation slot open. */
		force_no_close_slot = close_list->data;
	}

	/* Handle the windows in the close list. */
	for (node = close_list; node != NULL; node = node->next) {
		slot = node->data;

		if (slot != force_no_close_slot) {
			athena_window_pane_slot_close (slot->pane, slot);
		} else {
			computer = g_file_new_for_path (g_get_home_dir ());
			athena_window_slot_go_to (slot, computer, FALSE);
			g_object_unref(computer);
		}
	}

	g_list_free (close_list);
}

static void
open_window (AthenaApplication *application,
	     GFile *location, GdkScreen *screen, const char *geometry)
{
	AthenaWindow *window;
	gchar *uri;

	uri = g_file_get_uri (location);
	DEBUG ("Opening new window at uri %s", uri);

	window = athena_application_create_window (application,
						     screen);
	athena_window_go_to (window, location);

	if (geometry != NULL && !gtk_widget_get_visible (GTK_WIDGET (window))) {
		/* never maximize windows opened from shell if a
		 * custom geometry has been requested.
		 */
		gtk_window_unmaximize (GTK_WINDOW (window));
		eel_gtk_window_set_initial_geometry_from_string (GTK_WINDOW (window),
								 geometry,
								 APPLICATION_WINDOW_MIN_WIDTH,
								 APPLICATION_WINDOW_MIN_HEIGHT,
								 FALSE);
	}

	g_free (uri);
}

static void
open_windows (AthenaApplication *application,
	      GFile **files,
	      gint n_files,
	      GdkScreen *screen,
	      const char *geometry)
{
	guint i;

	if (files == NULL || files[0] == NULL) {
		/* Open a window pointing at the default location. */
		open_window (application, NULL, screen, geometry);
	} else {
		/* Open windows at each requested location. */
		for (i = 0; i < n_files; i++) {
			open_window (application, files[i], screen, geometry);
		}
	}
}

void
athena_application_open_location (AthenaApplication *application,
				    GFile *location,
				    GFile *selection,
				    const char *startup_id)
{
	AthenaWindow *window;
	GList *sel_list = NULL;

	window = athena_application_create_window (application, gdk_screen_get_default ());
	gtk_window_set_startup_id (GTK_WINDOW (window), startup_id);

	if (selection != NULL) {
		sel_list = g_list_prepend (sel_list, athena_file_get (selection));
	}

	athena_window_slot_open_location (athena_window_get_active_slot (window),
					    location,
					    0,
					    sel_list);

	if (sel_list != NULL) {
		athena_file_list_free (sel_list);
	}
}

static void
athena_application_open (GApplication *app,
			   GFile **files,
			   gint n_files,
			   const gchar *hint)
{
	AthenaApplication *self = ATHENA_APPLICATION (app);

	DEBUG ("Open called on the GApplication instance; %d files", n_files);

	open_windows (self, files, n_files,
		      gdk_screen_get_default (),
		      self->priv->geometry);
}

static GObject *
athena_application_constructor (GType type,
				  guint n_construct_params,
				  GObjectConstructParam *construct_params)
{
        GObject *retval;

        if (singleton != NULL) {
                return G_OBJECT (singleton);
        }

        retval = G_OBJECT_CLASS (athena_application_parent_class)->constructor
                (type, n_construct_params, construct_params);

        singleton = ATHENA_APPLICATION (retval);
        g_object_add_weak_pointer (retval, (gpointer) &singleton);

        return retval;
}

static void
athena_application_init (AthenaApplication *application)
{
	GSimpleAction *action;

	application->priv =
		G_TYPE_INSTANCE_GET_PRIVATE (application, ATHENA_TYPE_APPLICATION,
					     AthenaApplicationPriv);

	action = g_simple_action_new ("quit", NULL);

        g_action_map_add_action (G_ACTION_MAP (application), G_ACTION (action));

	g_signal_connect_swapped (action, "activate",
				  G_CALLBACK (athena_application_quit), application);

	g_object_unref (action);
}

static void
athena_application_finalize (GObject *object)
{
	AthenaApplication *application;

	application = ATHENA_APPLICATION (object);

	athena_bookmarks_exiting ();

	g_clear_object (&application->undo_manager);
	g_clear_object (&application->priv->volume_monitor);
	g_clear_object (&application->priv->progress_handler);

	g_free (application->priv->geometry);

	athena_dbus_manager_stop ();
	athena_freedesktop_dbus_stop ();
	notify_uninit ();

        G_OBJECT_CLASS (athena_application_parent_class)->finalize (object);
}

static gboolean
do_cmdline_sanity_checks (AthenaApplication *self,
			  gboolean perform_self_check,
			  gboolean version,
			  gboolean kill_shell,
			  gchar **remaining)
{
	gboolean retval = FALSE;

	if (perform_self_check && (remaining != NULL || kill_shell)) {
		g_printerr ("%s\n",
			    _("--check cannot be used with other options."));
		goto out;
	}

	if (kill_shell && remaining != NULL) {
		g_printerr ("%s\n",
			    _("--quit cannot be used with URIs."));
		goto out;
	}

	if (self->priv->geometry != NULL &&
	    remaining != NULL && remaining[0] != NULL && remaining[1] != NULL) {
		g_printerr ("%s\n",
			    _("--geometry cannot be used with more than one URI."));
		goto out;
	}

	retval = TRUE;

 out:
	return retval;
}

static void
do_perform_self_checks (gint *exit_status)
{
#ifndef ATHENA_OMIT_SELF_CHECK
	/* Run the checks (each twice) for athena and libathena-private. */

	athena_run_self_checks ();
	athena_run_lib_self_checks ();
	eel_exit_if_self_checks_failed ();

	athena_run_self_checks ();
	athena_run_lib_self_checks ();
	eel_exit_if_self_checks_failed ();
#endif

	*exit_status = EXIT_SUCCESS;
}

void
athena_application_quit (AthenaApplication *self)
{
	GApplication *app = G_APPLICATION (self);
	GList *windows;

	windows = gtk_application_get_windows (GTK_APPLICATION (app));
	g_list_foreach (windows, (GFunc) gtk_widget_destroy, NULL);
}

static gboolean
athena_application_local_command_line (GApplication *application,
					 gchar ***arguments,
					 gint *exit_status)
{
	gboolean perform_self_check = FALSE;
	gboolean version = FALSE;
	gboolean browser = FALSE;
	gboolean kill_shell = FALSE;
	gboolean no_default_window = FALSE;
	gchar **remaining = NULL;
	AthenaApplication *self = ATHENA_APPLICATION (application);

	const GOptionEntry options[] = {
#ifndef ATHENA_OMIT_SELF_CHECK
		{ "check", 'c', 0, G_OPTION_ARG_NONE, &perform_self_check, 
		  N_("Perform a quick set of self-check tests."), NULL },
#endif
		/* dummy, only for compatibility reasons */
		{ "browser", '\0', G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &browser,
		  NULL, NULL },
		{ "version", '\0', 0, G_OPTION_ARG_NONE, &version,
		  N_("Show the version of the program."), NULL },
		{ "geometry", 'g', 0, G_OPTION_ARG_STRING, &self->priv->geometry,
		  N_("Create the initial window with the given geometry."), N_("GEOMETRY") },
		{ "no-default-window", 'n', 0, G_OPTION_ARG_NONE, &no_default_window,
		  N_("Only create windows for explicitly specified URIs."), NULL },
		{ "no-desktop", '\0', 0, G_OPTION_ARG_NONE, &self->priv->no_desktop,
		  N_("Do not manage the desktop (ignore the preference set in the preferences dialog)."), NULL },
		{ "quit", 'q', 0, G_OPTION_ARG_NONE, &kill_shell, 
		  N_("Quit Athena."), NULL },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &remaining, NULL,  N_("[URI...]") },

		{ NULL }
	};
	GOptionContext *context;
	GError *error = NULL;
	gint argc = 0;
	gchar **argv = NULL;

	*exit_status = EXIT_SUCCESS;

	context = g_option_context_new (_("\n\nBrowse the file system with the file manager"));
	g_option_context_add_main_entries (context, options, NULL);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));

	argv = *arguments;
	argc = g_strv_length (argv);

	if (!g_option_context_parse (context, &argc, &argv, &error)) {
		g_printerr ("Could not parse arguments: %s\n", error->message);
		g_error_free (error);

		*exit_status = EXIT_FAILURE;
		goto out;
	}

	if (version) {
		g_print ("GNOME athena " PACKAGE_VERSION "\n");
		goto out;
	}

	if (!do_cmdline_sanity_checks (self, perform_self_check,
				       version, kill_shell, remaining)) {
		*exit_status = EXIT_FAILURE;
		goto out;
	}

	if (perform_self_check) {
		do_perform_self_checks (exit_status);
		goto out;
	}

	DEBUG ("Parsing local command line, no_default_window %d, quit %d, "
	       "self checks %d, no_desktop %d",
	       no_default_window, kill_shell, perform_self_check, self->priv->no_desktop);

	g_application_register (application, NULL, &error);

	if (error != NULL) {
		g_printerr ("Could not register the application: %s\n", error->message);
		g_error_free (error);

		*exit_status = EXIT_FAILURE;
		goto out;
	}

	if (kill_shell) {
		DEBUG ("Killing application, as requested");
		g_action_group_activate_action (G_ACTION_GROUP (application),
						"quit", NULL);
		goto out;
	}

	GFile **files;
	gint idx, len;

	len = 0;
	files = NULL;

	/* Convert args to GFiles */
	if (remaining != NULL) {
		GFile *file;
		GPtrArray *file_array;

		file_array = g_ptr_array_new ();

		for (idx = 0; remaining[idx] != NULL; idx++) {
			file = g_file_new_for_commandline_arg (remaining[idx]);
			if (file != NULL) {
				g_ptr_array_add (file_array, file);
			}
		}

		len = file_array->len;
		files = (GFile **) g_ptr_array_free (file_array, FALSE);
		g_strfreev (remaining);
	}

	if (files == NULL && !no_default_window) {
		files = g_malloc0 (2 * sizeof (GFile *));
		len = 1;

		files[0] = g_file_new_for_path (g_get_home_dir ());
		files[1] = NULL;
	}

	/* Invoke "Open" to create new windows */
	if (len > 0) {
		g_application_open (application, files, len, "");
	}

	for (idx = 0; idx < len; idx++) {
		g_object_unref (files[idx]);
	}
	g_free (files);

 out:
	g_option_context_free (context);

	return TRUE;	
}

static void
init_icons_and_styles (void)
{
	/* initialize search path for custom icons */
	gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
					   ATHENA_DATADIR G_DIR_SEPARATOR_S "icons");
}

static void
init_desktop (AthenaApplication *self)
{
	/* Initialize the desktop link monitor singleton */
	athena_desktop_link_monitor_get ();

	if (!self->priv->no_desktop &&
	    !g_settings_get_boolean (gnome_background_preferences,
				     ATHENA_PREFERENCES_SHOW_DESKTOP)) {
		self->priv->no_desktop = TRUE;
	}

	if (!self->priv->no_desktop) {
		athena_application_open_desktop (self);
	}

	/* Monitor the preference to show or hide the desktop */
	g_signal_connect_swapped (gnome_background_preferences, "changed::" ATHENA_PREFERENCES_SHOW_DESKTOP,
				  G_CALLBACK (desktop_changed_callback),
				  self);
}

static gboolean 
athena_application_save_accel_map (gpointer data)
{
	if (save_of_accel_map_requested) {
		char *accel_map_filename;
	 	accel_map_filename = athena_get_accel_map_file ();
	 	if (accel_map_filename) {
	 		gtk_accel_map_save (accel_map_filename);
	 		g_free (accel_map_filename);
	 	}
		save_of_accel_map_requested = FALSE;
	}

	return FALSE;
}

static void 
queue_accel_map_save_callback (GtkAccelMap *object, gchar *accel_path,
		guint accel_key, GdkModifierType accel_mods,
		gpointer user_data)
{
	if (!save_of_accel_map_requested) {
		save_of_accel_map_requested = TRUE;
		g_timeout_add_seconds (ATHENA_ACCEL_MAP_SAVE_DELAY, 
				athena_application_save_accel_map, NULL);
	}
}

static void
init_gtk_accels (void)
{
	char *accel_map_filename;

	/* load accelerator map, and register save callback */
	accel_map_filename = athena_get_accel_map_file ();
	if (accel_map_filename) {
		gtk_accel_map_load (accel_map_filename);
		g_free (accel_map_filename);
	}

	g_signal_connect (gtk_accel_map_get (), "changed",
			  G_CALLBACK (queue_accel_map_save_callback), NULL);
}

static void
athena_application_startup (GApplication *app)
{
	AthenaApplication *self = ATHENA_APPLICATION (app);

	/* chain up to the GTK+ implementation early, so gtk_init()
	 * is called for us.
	 */
	G_APPLICATION_CLASS (athena_application_parent_class)->startup (app);

	/* initialize the previewer singleton */
	athena_previewer_get_singleton ();

	/* create an undo manager */
	self->undo_manager = athena_undo_manager_new ();

	/* create DBus manager */
	athena_dbus_manager_start (app);
	athena_freedesktop_dbus_start (self);

	/* initialize preferences and create the global GSettings objects */
	athena_global_preferences_init ();

	/* register views */
	athena_icon_view_register ();
	athena_desktop_icon_view_register ();
	athena_list_view_register ();
	athena_icon_view_compact_register ();
#if ENABLE_EMPTY_VIEW
	athena_empty_view_register ();
#endif

	/* register property pages */
	athena_image_properties_page_register ();

	/* initialize theming */
	init_icons_and_styles ();
	init_gtk_accels ();
	
	/* initialize athena modules */
	athena_module_setup ();

	/* attach menu-provider module callback */
	menu_provider_init_callback ();
	
	/* Initialize the UI handler singleton for file operations */
	notify_init (GETTEXT_PACKAGE);
	self->priv->progress_handler = athena_progress_ui_handler_new ();

	/* Watch for unmounts so we can close open windows */
	/* TODO-gio: This should be using the UNMOUNTED feature of GFileMonitor instead */
	self->priv->volume_monitor = g_volume_monitor_get ();
	g_signal_connect_object (self->priv->volume_monitor, "mount_removed",
				 G_CALLBACK (mount_removed_callback), self, 0);
	g_signal_connect_object (self->priv->volume_monitor, "mount_added",
				 G_CALLBACK (mount_added_callback), self, 0);

	/* Check the user's ~/.athena directories and post warnings
	 * if there are problems.
	 */
	check_required_directories (self);
	init_desktop (self);

	do_upgrades_once (self);
}

static void
athena_application_quit_mainloop (GApplication *app)
{
	DEBUG ("Quitting mainloop");

	athena_icon_info_clear_caches ();
 	athena_application_save_accel_map (NULL);

	G_APPLICATION_CLASS (athena_application_parent_class)->quit_mainloop (app);
}

static void
athena_application_window_removed (GtkApplication *app,
				     GtkWindow *window)
{
	AthenaPreviewer *previewer;

	/* chain to parent */
	GTK_APPLICATION_CLASS (athena_application_parent_class)->window_removed (app, window);

	/* if this was the last window, close the previewer */
	if (g_list_length (gtk_application_get_windows (app)) == 0) {
		previewer = athena_previewer_get_singleton ();
		athena_previewer_call_close (previewer);
	}
}

static void
athena_application_class_init (AthenaApplicationClass *class)
{
        GObjectClass *object_class;
	GApplicationClass *application_class;
	GtkApplicationClass *gtkapp_class;

        object_class = G_OBJECT_CLASS (class);
	object_class->constructor = athena_application_constructor;
        object_class->finalize = athena_application_finalize;

	application_class = G_APPLICATION_CLASS (class);
	application_class->startup = athena_application_startup;
	application_class->quit_mainloop = athena_application_quit_mainloop;
	application_class->open = athena_application_open;
	application_class->local_command_line = athena_application_local_command_line;

	gtkapp_class = GTK_APPLICATION_CLASS (class);
	gtkapp_class->window_removed = athena_application_window_removed;

	g_type_class_add_private (class, sizeof (AthenaApplicationPriv));
}

AthenaApplication *
athena_application_get_singleton (void)
{
	return g_object_new (ATHENA_TYPE_APPLICATION,
			     "application-id", "com.solusos.AthenaApplication",
			     "flags", G_APPLICATION_HANDLES_OPEN,
			     NULL);
}
