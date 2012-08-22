/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-file-utilities.h - interface for file manipulation routines.

   Copyright (C) 1999, 2000, 2001 Eazel, Inc.

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

#ifndef ATHENA_FILE_UTILITIES_H
#define ATHENA_FILE_UTILITIES_H

#include <gio/gio.h>
#include <gtk/gtk.h>

#define ATHENA_SAVED_SEARCH_EXTENSION ".savedSearch"
#define ATHENA_SAVED_SEARCH_MIMETYPE "application/x-gnome-saved-search"

/* These functions all return something something that needs to be
 * freed with g_free, is not NULL, and is guaranteed to exist.
 */
char *   athena_get_xdg_dir                        (const char *type);
char *   athena_get_user_directory                 (void);
char *   athena_get_desktop_directory              (void);
GFile *  athena_get_desktop_location               (void);
char *   athena_get_desktop_directory_uri          (void);
char *   athena_get_home_directory_uri             (void);
gboolean athena_is_desktop_directory_file          (GFile *dir,
						      const char *filename);
gboolean athena_is_root_directory                  (GFile *dir);
gboolean athena_is_desktop_directory               (GFile *dir);
gboolean athena_is_home_directory                  (GFile *dir);
gboolean athena_is_home_directory_file             (GFile *dir,
						      const char *filename);
gboolean athena_is_in_system_dir                   (GFile *location);
char *   athena_get_gmc_desktop_directory          (void);

gboolean athena_should_use_templates_directory     (void);
char *   athena_get_templates_directory            (void);
char *   athena_get_templates_directory_uri        (void);
void     athena_create_templates_directory         (void);

char *   athena_get_searches_directory             (void);

char *	 athena_compute_title_for_location	     (GFile *file);

/* This function returns something that needs to be freed with g_free,
 * is not NULL, but is not garaunteed to exist */
char *   athena_get_desktop_directory_uri_no_create (void);

/* Locate a file in either the uers directory or the datadir. */
char *   athena_get_data_file_path                 (const char *partial_path);

gboolean athena_is_file_roller_installed           (void);

/* Inhibit/Uninhibit GNOME Power Manager */
int    athena_inhibit_power_manager                (const char *message) G_GNUC_WARN_UNUSED_RESULT;
void     athena_uninhibit_power_manager            (int cookie);

/* Return an allocated file name that is guranteed to be unique, but
 * tries to make the name readable to users.
 * This isn't race-free, so don't use for security-related things
 */
char *   athena_ensure_unique_file_name            (const char *directory_uri,
						      const char *base_name,
			                              const char *extension);
char *   athena_unique_temporary_file_name         (void);

GFile *  athena_find_existing_uri_in_hierarchy     (GFile *location);

GFile *
athena_find_file_insensitive (GFile *parent, const gchar *name);

char * athena_get_accel_map_file (void);

GHashTable * athena_trashed_files_get_original_directories (GList *files,
							      GList **unhandled_files);
void athena_restore_files_from_trash (GList *files,
					GtkWindow *parent_window);

typedef void (*AthenaMountGetContent) (const char **content, gpointer user_data);

char ** athena_get_cached_x_content_types_for_mount (GMount *mount);
void athena_get_x_content_types_for_mount_async (GMount *mount,
						   AthenaMountGetContent callback,
						   GCancellable *cancellable,
						   gpointer user_data);

#endif /* ATHENA_FILE_UTILITIES_H */
