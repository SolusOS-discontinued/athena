/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-desktop-icon-file.c: Subclass of AthenaFile to help implement the
   virtual desktop icons.
 
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
#include "athena-desktop-icon-file.h"

#include "athena-desktop-metadata.h"
#include "athena-desktop-directory-file.h"
#include "athena-directory-notify.h"
#include "athena-directory-private.h"
#include "athena-file-attributes.h"
#include "athena-file-private.h"
#include "athena-file-utilities.h"
#include "athena-file-operations.h"
#include <eel/eel-glib-extensions.h>
#include "athena-desktop-directory.h"
#include <glib/gi18n.h>
#include <string.h>
#include <gio/gio.h>

struct AthenaDesktopIconFileDetails {
	AthenaDesktopLink *link;
};

G_DEFINE_TYPE(AthenaDesktopIconFile, athena_desktop_icon_file, ATHENA_TYPE_FILE)


static void
desktop_icon_file_monitor_add (AthenaFile *file,
			       gconstpointer client,
			       AthenaFileAttributes attributes)
{
	athena_directory_monitor_add_internal
		(file->details->directory, file,
		 client, TRUE, attributes, NULL, NULL);
}

static void
desktop_icon_file_monitor_remove (AthenaFile *file,
				  gconstpointer client)
{
	athena_directory_monitor_remove_internal
		(file->details->directory, file, client);
}

static void
desktop_icon_file_call_when_ready (AthenaFile *file,
				   AthenaFileAttributes attributes,
				   AthenaFileCallback callback,
				   gpointer callback_data)
{
	athena_directory_call_when_ready_internal
		(file->details->directory, file,
		 attributes, FALSE, NULL, callback, callback_data);
}

static void
desktop_icon_file_cancel_call_when_ready (AthenaFile *file,
					  AthenaFileCallback callback,
					  gpointer callback_data)
{
	athena_directory_cancel_callback_internal
		(file->details->directory, file,
		 NULL, callback, callback_data);
}

static gboolean
desktop_icon_file_check_if_ready (AthenaFile *file,
				  AthenaFileAttributes attributes)
{
	return athena_directory_check_if_ready_internal
		(file->details->directory, file,
		 attributes);
}

static gboolean
desktop_icon_file_get_item_count (AthenaFile *file, 
				  guint *count,
				  gboolean *count_unreadable)
{
	if (count != NULL) {
		*count = 0;
	}
	if (count_unreadable != NULL) {
		*count_unreadable = FALSE;
	}
	return TRUE;
}

static AthenaRequestStatus
desktop_icon_file_get_deep_counts (AthenaFile *file,
				   guint *directory_count,
				   guint *file_count,
				   guint *unreadable_directory_count,
				   goffset *total_size)
{
	if (directory_count != NULL) {
		*directory_count = 0;
	}
	if (file_count != NULL) {
		*file_count = 0;
	}
	if (unreadable_directory_count != NULL) {
		*unreadable_directory_count = 0;
	}
	if (total_size != NULL) {
		*total_size = 0;
	}

	return ATHENA_REQUEST_DONE;
}

static gboolean
desktop_icon_file_get_date (AthenaFile *file,
			    AthenaDateType date_type,
			    time_t *date)
{
	AthenaDesktopIconFile *desktop_file;

	desktop_file = ATHENA_DESKTOP_ICON_FILE (file);

	return athena_desktop_link_get_date (desktop_file->details->link,
					       date_type, date);
}

static char *
desktop_icon_file_get_where_string (AthenaFile *file)
{
	return g_strdup (_("on the desktop"));
}

static void
athena_desktop_icon_file_init (AthenaDesktopIconFile *desktop_file)
{
	desktop_file->details =	G_TYPE_INSTANCE_GET_PRIVATE (desktop_file,
							     ATHENA_TYPE_DESKTOP_ICON_FILE,
							     AthenaDesktopIconFileDetails);
}

static void
update_info_from_link (AthenaDesktopIconFile *icon_file)
{
	AthenaFile *file;
	AthenaDesktopLink *link;
	char *display_name;
	GMount *mount;
	
	file = ATHENA_FILE (icon_file);
	
	link = icon_file->details->link;

	if (link == NULL) {
		return;
	}

	eel_ref_str_unref (file->details->mime_type);
	file->details->mime_type = eel_ref_str_get_unique ("application/x-athena-link");
	file->details->type = G_FILE_TYPE_SHORTCUT;
	file->details->size = 0;
	file->details->has_permissions = FALSE;
	file->details->can_read = TRUE;
	file->details->can_write = TRUE;

	file->details->can_mount = FALSE;
	file->details->can_unmount = FALSE;
	file->details->can_eject = FALSE;
	if (file->details->mount) {
		g_object_unref (file->details->mount);
	}
	mount = athena_desktop_link_get_mount (link);
	file->details->mount = mount;
	if (mount) {
		file->details->can_unmount = g_mount_can_unmount (mount);
		file->details->can_eject = g_mount_can_eject (mount);
	}
	
	file->details->file_info_is_up_to_date = TRUE;

	display_name = athena_desktop_link_get_display_name (link);
	athena_file_set_display_name (file,
					display_name, NULL, TRUE);
	g_free (display_name);

	if (file->details->icon != NULL) {
		g_object_unref (file->details->icon);
	}
	file->details->icon = athena_desktop_link_get_icon (link);
	g_free (file->details->activation_uri);
	file->details->activation_uri = athena_desktop_link_get_activation_uri (link);
	file->details->got_link_info = TRUE;
	file->details->link_info_is_up_to_date = TRUE;

	file->details->directory_count = 0;
	file->details->got_directory_count = TRUE;
	file->details->directory_count_is_up_to_date = TRUE;
}

void
athena_desktop_icon_file_update (AthenaDesktopIconFile *icon_file)
{
	AthenaFile *file;
	
	update_info_from_link (icon_file);
	file = ATHENA_FILE (icon_file);
	athena_file_changed (file);
}

void
athena_desktop_icon_file_remove (AthenaDesktopIconFile *icon_file)
{
	AthenaFile *file;
	GList list;

	icon_file->details->link = NULL;

	file = ATHENA_FILE (icon_file);
	
	/* ref here because we might be removing the last ref when we
	 * mark the file gone below, but we need to keep a ref at
	 * least long enough to send the change notification. 
	 */
	athena_file_ref (file);
	
	file->details->is_gone = TRUE;
	
	list.data = file;
	list.next = NULL;
	list.prev = NULL;
	
	athena_directory_remove_file (file->details->directory, file);
	athena_directory_emit_change_signals (file->details->directory, &list);
	
	athena_file_unref (file);
}

AthenaDesktopIconFile *
athena_desktop_icon_file_new (AthenaDesktopLink *link)
{
	AthenaFile *file;
	AthenaDirectory *directory;
	AthenaDesktopIconFile *icon_file;
	GList list;
	char *name;

	directory = athena_directory_get_by_uri (EEL_DESKTOP_URI);

	file = ATHENA_FILE (g_object_new (ATHENA_TYPE_DESKTOP_ICON_FILE, NULL));

#ifdef ATHENA_FILE_DEBUG_REF
	printf("%10p ref'd\n", file);
	eazel_dump_stack_trace ("\t", 10);
#endif

	file->details->directory = directory;

	icon_file = ATHENA_DESKTOP_ICON_FILE (file);
	icon_file->details->link = link;

	name = athena_desktop_link_get_file_name (link);
	file->details->name = eel_ref_str_new (name);
	g_free (name);

	update_info_from_link (icon_file);

	athena_desktop_update_metadata_from_keyfile (file, file->details->name);

	athena_directory_add_file (directory, file);

	list.data = file;
	list.next = NULL;
	list.prev = NULL;
	athena_directory_emit_files_added (directory, &list);

	return icon_file;
}

/* Note: This can return NULL if the link was recently removed (i.e. unmounted) */
AthenaDesktopLink *
athena_desktop_icon_file_get_link (AthenaDesktopIconFile *icon_file)
{
	if (icon_file->details->link)
		return g_object_ref (icon_file->details->link);
	else
		return NULL;
}

static void
athena_desktop_icon_file_unmount (AthenaFile                   *file,
				    GMountOperation                *mount_op,
				    GCancellable                   *cancellable,
				    AthenaFileOperationCallback   callback,
				    gpointer                        callback_data)
{
	AthenaDesktopIconFile *desktop_file;
	GMount *mount;
	
	desktop_file = ATHENA_DESKTOP_ICON_FILE (file);
	if (desktop_file) {
		mount = athena_desktop_link_get_mount (desktop_file->details->link);
		if (mount != NULL) {
			athena_file_operations_unmount_mount (NULL, mount, FALSE, TRUE);
		}
	}
	
}

static void
athena_desktop_icon_file_eject (AthenaFile                   *file,
				  GMountOperation                *mount_op,
				  GCancellable                   *cancellable,
				  AthenaFileOperationCallback   callback,
				  gpointer                        callback_data)
{
	AthenaDesktopIconFile *desktop_file;
	GMount *mount;
	
	desktop_file = ATHENA_DESKTOP_ICON_FILE (file);
	if (desktop_file) {
		mount = athena_desktop_link_get_mount (desktop_file->details->link);
		if (mount != NULL) {
			athena_file_operations_unmount_mount (NULL, mount, TRUE, TRUE);
		}
	}
}

static void
athena_desktop_icon_file_set_metadata (AthenaFile           *file,
					 const char             *key,
					 const char             *value)
{
	athena_desktop_set_metadata_string (file, file->details->name, key, value);
}

static void
athena_desktop_icon_file_set_metadata_as_list (AthenaFile           *file,
						 const char             *key,
						 char                  **value)
{
	athena_desktop_set_metadata_stringv (file, file->details->name, key, (const gchar **) value);
}

static void
athena_desktop_icon_file_class_init (AthenaDesktopIconFileClass *klass)
{
	GObjectClass *object_class;
	AthenaFileClass *file_class;

	object_class = G_OBJECT_CLASS (klass);
	file_class = ATHENA_FILE_CLASS (klass);

	file_class->default_file_type = G_FILE_TYPE_DIRECTORY;
	
	file_class->monitor_add = desktop_icon_file_monitor_add;
	file_class->monitor_remove = desktop_icon_file_monitor_remove;
	file_class->call_when_ready = desktop_icon_file_call_when_ready;
	file_class->cancel_call_when_ready = desktop_icon_file_cancel_call_when_ready;
	file_class->check_if_ready = desktop_icon_file_check_if_ready;
	file_class->get_item_count = desktop_icon_file_get_item_count;
	file_class->get_deep_counts = desktop_icon_file_get_deep_counts;
	file_class->get_date = desktop_icon_file_get_date;
	file_class->get_where_string = desktop_icon_file_get_where_string;
	file_class->set_metadata = athena_desktop_icon_file_set_metadata;
	file_class->set_metadata_as_list = athena_desktop_icon_file_set_metadata_as_list;
	file_class->unmount = athena_desktop_icon_file_unmount;
	file_class->eject = athena_desktop_icon_file_eject;

	g_type_class_add_private (object_class, sizeof(AthenaDesktopIconFileDetails));
}