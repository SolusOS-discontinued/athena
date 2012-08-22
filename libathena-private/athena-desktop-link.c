/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-desktop-link.c: Class that handles the links on the desktop
    
   Copyright (C) 2003 Red Hat, Inc.
  
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
  
   Author: Alexander Larsson <alexl@redhat.com>
*/

#include <config.h>
#include "athena-desktop-link.h"
#include "athena-desktop-link-monitor.h"
#include "athena-desktop-icon-file.h"
#include "athena-directory-private.h"
#include "athena-desktop-directory.h"
#include "athena-icon-names.h"
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <libathena-private/athena-file-utilities.h>
#include <libathena-private/athena-trash-monitor.h>
#include <libathena-private/athena-global-preferences.h>
#include <string.h>

struct AthenaDesktopLinkDetails {
	AthenaDesktopLinkType type;
        char *filename;
	char *display_name;
	GFile *activation_location;
	GIcon *icon;

	AthenaDesktopIconFile *icon_file;
	
	GObject *signal_handler_obj;
	gulong signal_handler;

	/* Just for mount icons: */
	GMount *mount;
};

G_DEFINE_TYPE(AthenaDesktopLink, athena_desktop_link, G_TYPE_OBJECT)

static void
create_icon_file (AthenaDesktopLink *link)
{
	link->details->icon_file = athena_desktop_icon_file_new (link);
}

static void
athena_desktop_link_changed (AthenaDesktopLink *link)
{
	if (link->details->icon_file != NULL) {
		athena_desktop_icon_file_update (link->details->icon_file);
	}
}

static void
mount_changed_callback (GMount *mount, AthenaDesktopLink *link)
{
	g_free (link->details->display_name);
	if (link->details->activation_location) {
		g_object_unref (link->details->activation_location);
	}
	if (link->details->icon) {
		g_object_unref (link->details->icon);
	}
	
	link->details->display_name = g_mount_get_name (mount);
	link->details->activation_location = g_mount_get_default_location (mount);
	link->details->icon = g_mount_get_icon (mount);
	
	athena_desktop_link_changed (link);
}

static void
trash_state_changed_callback (AthenaTrashMonitor *trash_monitor,
			      gboolean state,
			      gpointer callback_data)
{
	AthenaDesktopLink *link;

	link = ATHENA_DESKTOP_LINK (callback_data);
	g_assert (link->details->type == ATHENA_DESKTOP_LINK_TRASH);

	if (link->details->icon) {
		g_object_unref (link->details->icon);
	}
	link->details->icon = athena_trash_monitor_get_icon ();

	athena_desktop_link_changed (link);
}

static void
home_name_changed (gpointer callback_data)
{
	AthenaDesktopLink *link;

	link = ATHENA_DESKTOP_LINK (callback_data);
	g_assert (link->details->type == ATHENA_DESKTOP_LINK_HOME);

	g_free (link->details->display_name);
	link->details->display_name = g_settings_get_string (athena_desktop_preferences,
							     ATHENA_PREFERENCES_DESKTOP_HOME_NAME);
	if (link->details->display_name[0] == 0) {
		g_free (link->details->display_name);
		link->details->display_name = g_strdup (_("Home"));
	}

	athena_desktop_link_changed (link);
}

static void
computer_name_changed (gpointer callback_data)
{
	AthenaDesktopLink *link;

	link = ATHENA_DESKTOP_LINK (callback_data);
	g_assert (link->details->type == ATHENA_DESKTOP_LINK_COMPUTER);

	g_free (link->details->display_name);
	link->details->display_name = g_settings_get_string (athena_desktop_preferences,
							     ATHENA_PREFERENCES_DESKTOP_COMPUTER_NAME);

	athena_desktop_link_changed (link);
}

static void
trash_name_changed (gpointer callback_data)
{
	AthenaDesktopLink *link;

	link = ATHENA_DESKTOP_LINK (callback_data);
	g_assert (link->details->type == ATHENA_DESKTOP_LINK_TRASH);

	g_free (link->details->display_name);
	link->details->display_name = g_settings_get_string (athena_desktop_preferences,
							     ATHENA_PREFERENCES_DESKTOP_TRASH_NAME);
	athena_desktop_link_changed (link);
}

static void
network_name_changed (gpointer callback_data)
{
	AthenaDesktopLink *link;

	link = ATHENA_DESKTOP_LINK (callback_data);
	g_assert (link->details->type == ATHENA_DESKTOP_LINK_NETWORK);

	g_free (link->details->display_name);
	link->details->display_name = g_settings_get_string (athena_desktop_preferences,
							     ATHENA_PREFERENCES_DESKTOP_NETWORK_NAME);
	athena_desktop_link_changed (link);
}

AthenaDesktopLink *
athena_desktop_link_new (AthenaDesktopLinkType type)
{
	AthenaDesktopLink *link;

	link = ATHENA_DESKTOP_LINK (g_object_new (ATHENA_TYPE_DESKTOP_LINK, NULL));

	link->details->type = type;
	switch (type) {
	case ATHENA_DESKTOP_LINK_HOME:
		link->details->filename = g_strdup ("home");
		link->details->display_name = g_settings_get_string (athena_desktop_preferences,
								     ATHENA_PREFERENCES_DESKTOP_HOME_NAME);
		link->details->activation_location = g_file_new_for_path (g_get_home_dir ());
		link->details->icon = g_themed_icon_new (ATHENA_ICON_HOME);

		g_signal_connect_swapped (athena_desktop_preferences,
					  "changed::" ATHENA_PREFERENCES_DESKTOP_HOME_NAME,
					  G_CALLBACK (home_name_changed),
					  link);
		break;

	case ATHENA_DESKTOP_LINK_COMPUTER:
		link->details->filename = g_strdup ("computer");
		link->details->display_name = g_settings_get_string (athena_desktop_preferences,
								     ATHENA_PREFERENCES_DESKTOP_COMPUTER_NAME);
		link->details->activation_location = g_file_new_for_uri ("computer:///");
		/* TODO: This might need a different icon: */
		link->details->icon = g_themed_icon_new (ATHENA_ICON_COMPUTER);

		g_signal_connect_swapped (athena_desktop_preferences,
					  "changed::" ATHENA_PREFERENCES_DESKTOP_COMPUTER_NAME,
					  G_CALLBACK (computer_name_changed),
					  link);
		break;

	case ATHENA_DESKTOP_LINK_TRASH:
		link->details->filename = g_strdup ("trash");
		link->details->display_name = g_settings_get_string (athena_desktop_preferences,
								     ATHENA_PREFERENCES_DESKTOP_TRASH_NAME);
		link->details->activation_location = g_file_new_for_uri (EEL_TRASH_URI);
		link->details->icon = athena_trash_monitor_get_icon ();

		g_signal_connect_swapped (athena_desktop_preferences,
					  "changed::" ATHENA_PREFERENCES_DESKTOP_TRASH_NAME,
					  G_CALLBACK (trash_name_changed),
					  link);
		link->details->signal_handler_obj = G_OBJECT (athena_trash_monitor_get ());
		link->details->signal_handler =
			g_signal_connect_object (athena_trash_monitor_get (), "trash_state_changed",
						 G_CALLBACK (trash_state_changed_callback), link, 0);
		break;

	case ATHENA_DESKTOP_LINK_NETWORK:
		link->details->filename = g_strdup ("network");
		link->details->display_name = g_settings_get_string (athena_desktop_preferences,
								     ATHENA_PREFERENCES_DESKTOP_NETWORK_NAME);
		link->details->activation_location = g_file_new_for_uri ("network:///");
		link->details->icon = g_themed_icon_new (ATHENA_ICON_NETWORK);

		g_signal_connect_swapped (athena_desktop_preferences,
					  "changed::" ATHENA_PREFERENCES_DESKTOP_NETWORK_NAME,
					  G_CALLBACK (network_name_changed),
					  link);
		break;

	default:
	case ATHENA_DESKTOP_LINK_MOUNT:
		g_assert_not_reached();
	}

	create_icon_file (link);

	return link;
}

AthenaDesktopLink *
athena_desktop_link_new_from_mount (GMount *mount)
{
	AthenaDesktopLink *link;
	GVolume *volume;
	char *name, *filename;

	link = ATHENA_DESKTOP_LINK (g_object_new (ATHENA_TYPE_DESKTOP_LINK, NULL));

	link->details->type = ATHENA_DESKTOP_LINK_MOUNT;

	link->details->mount = g_object_ref (mount);

	/* We try to use the drive name to get somewhat stable filenames
	   for metadata */
	volume = g_mount_get_volume (mount);
	if (volume != NULL) {
		name = g_volume_get_name (volume);
		g_object_unref (volume);
	} else {
		name = g_mount_get_name (mount);
	}

	/* Replace slashes in name */
	filename = g_strconcat (g_strdelimit (name, "/", '-'), ".volume", NULL);
	link->details->filename =
		athena_desktop_link_monitor_make_filename_unique (athena_desktop_link_monitor_get (),
								    filename);
	g_free (filename);
	g_free (name);
	
	link->details->display_name = g_mount_get_name (mount);
	
	link->details->activation_location = g_mount_get_default_location (mount);
	link->details->icon = g_mount_get_icon (mount);
	
	link->details->signal_handler_obj = G_OBJECT (mount);
	link->details->signal_handler =
		g_signal_connect (mount, "changed",
				  G_CALLBACK (mount_changed_callback), link);
	
	create_icon_file (link);

	return link;
}

GMount *
athena_desktop_link_get_mount (AthenaDesktopLink *link)
{
	if (link->details->mount) {
		return g_object_ref (link->details->mount);
	}
	return NULL;
}

AthenaDesktopLinkType
athena_desktop_link_get_link_type (AthenaDesktopLink *link)
{
	return link->details->type;
}

char *
athena_desktop_link_get_file_name (AthenaDesktopLink *link)
{
	return g_strdup (link->details->filename);
}

char *
athena_desktop_link_get_display_name (AthenaDesktopLink *link)
{
	return g_strdup (link->details->display_name);
}

GIcon *
athena_desktop_link_get_icon (AthenaDesktopLink *link)
{
	if (link->details->icon != NULL) {
		return g_object_ref (link->details->icon);
	}
	return NULL;
}

GFile *
athena_desktop_link_get_activation_location (AthenaDesktopLink *link)
{
	if (link->details->activation_location) {
		return g_object_ref (link->details->activation_location);
	}
	return NULL;
}

char *
athena_desktop_link_get_activation_uri (AthenaDesktopLink *link)
{
	if (link->details->activation_location) {
		return g_file_get_uri (link->details->activation_location);
	}
	return NULL;
}


gboolean
athena_desktop_link_get_date (AthenaDesktopLink *link,
				AthenaDateType     date_type,
				time_t               *date)
{
	return FALSE;
}

gboolean
athena_desktop_link_can_rename (AthenaDesktopLink     *link)
{
	return (link->details->type == ATHENA_DESKTOP_LINK_HOME ||
		link->details->type == ATHENA_DESKTOP_LINK_TRASH ||
		link->details->type == ATHENA_DESKTOP_LINK_NETWORK ||
		link->details->type == ATHENA_DESKTOP_LINK_COMPUTER);
}

gboolean
athena_desktop_link_rename (AthenaDesktopLink     *link,
			      const char              *name)
{
	switch (link->details->type) {
	case ATHENA_DESKTOP_LINK_HOME:
		g_settings_set_string (athena_desktop_preferences,
				       ATHENA_PREFERENCES_DESKTOP_HOME_NAME,
				       name);
		break;
	case ATHENA_DESKTOP_LINK_COMPUTER:
		g_settings_set_string (athena_desktop_preferences,
				       ATHENA_PREFERENCES_DESKTOP_COMPUTER_NAME,
				       name);
		break;
	case ATHENA_DESKTOP_LINK_TRASH:
		g_settings_set_string (athena_desktop_preferences,
				       ATHENA_PREFERENCES_DESKTOP_TRASH_NAME,
				       name);
		break;
	case ATHENA_DESKTOP_LINK_NETWORK:
		g_settings_set_string (athena_desktop_preferences,
				       ATHENA_PREFERENCES_DESKTOP_NETWORK_NAME,
				       name);
		break;
	default:
		g_assert_not_reached ();
		/* FIXME: Do we want volume renaming?
		 * We didn't support that before. */
		break;
	}

	return TRUE;
}

static void
athena_desktop_link_init (AthenaDesktopLink *link)
{
	link->details = G_TYPE_INSTANCE_GET_PRIVATE (link,
						     ATHENA_TYPE_DESKTOP_LINK,
						     AthenaDesktopLinkDetails);
}

static void
desktop_link_finalize (GObject *object)
{
	AthenaDesktopLink *link;

	link = ATHENA_DESKTOP_LINK (object);

	if (link->details->signal_handler != 0) {
		g_signal_handler_disconnect (link->details->signal_handler_obj,
					     link->details->signal_handler);
	}

	if (link->details->icon_file != NULL) {
		athena_desktop_icon_file_remove (link->details->icon_file);
		athena_file_unref (ATHENA_FILE (link->details->icon_file));
		link->details->icon_file = NULL;
	}

	if (link->details->type == ATHENA_DESKTOP_LINK_HOME) {
		g_signal_handlers_disconnect_by_func (athena_desktop_preferences,
						 home_name_changed,
						 link);
	}

	if (link->details->type == ATHENA_DESKTOP_LINK_COMPUTER) {
		g_signal_handlers_disconnect_by_func (athena_desktop_preferences,
						      computer_name_changed,
						      link);
	}

	if (link->details->type == ATHENA_DESKTOP_LINK_TRASH) {
		g_signal_handlers_disconnect_by_func (athena_desktop_preferences,
						      trash_name_changed,
						      link);
	}

	if (link->details->type == ATHENA_DESKTOP_LINK_NETWORK) {
		g_signal_handlers_disconnect_by_func (athena_desktop_preferences,
						      network_name_changed,
						      link);
	}

	if (link->details->type == ATHENA_DESKTOP_LINK_MOUNT) {
		g_object_unref (link->details->mount);
	}

	g_free (link->details->filename);
	g_free (link->details->display_name);
	if (link->details->activation_location) {
		g_object_unref (link->details->activation_location);
	}
	if (link->details->icon) {
		g_object_unref (link->details->icon);
	}

	G_OBJECT_CLASS (athena_desktop_link_parent_class)->finalize (object);
}

static void
athena_desktop_link_class_init (AthenaDesktopLinkClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = desktop_link_finalize;

	g_type_class_add_private (object_class, sizeof(AthenaDesktopLinkDetails));
}
