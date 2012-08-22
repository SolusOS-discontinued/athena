/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-directory-notify.h: Athena directory notify calls.
 
   Copyright (C) 2000, 2001 Eazel, Inc.
  
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
  
   Author: Darin Adler <darin@bentspoon.com>
*/

#include <gdk/gdk.h>
#include <libathena-private/athena-file.h>

typedef struct {
	char *from_uri;
	char *to_uri;
} URIPair;

typedef struct {
	GFile *from;
	GFile *to;
} GFilePair;

typedef struct {
	GFile *location;
	gboolean set;
	GdkPoint point;
	int screen;
} AthenaFileChangesQueuePosition;

/* Almost-public change notification calls */
void athena_directory_notify_files_added   (GList *files);
void athena_directory_notify_files_moved   (GList *file_pairs);
void athena_directory_notify_files_changed (GList *files);
void athena_directory_notify_files_removed (GList *files);

void athena_directory_schedule_metadata_copy   (GList        *file_pairs);
void athena_directory_schedule_metadata_move   (GList        *file_pairs);
void athena_directory_schedule_metadata_remove (GList        *files);

/* Deprecated URI versions: to be converted */
void athena_directory_notify_files_added_by_uri      (GList        *uris);
void athena_directory_notify_files_changed_by_uri    (GList        *uris);
void athena_directory_notify_files_moved_by_uri      (GList        *uri_pairs);
void athena_directory_notify_files_removed_by_uri    (GList        *uris);

void athena_directory_schedule_metadata_copy_by_uri   (GList        *uri_pairs);
void athena_directory_schedule_metadata_move_by_uri   (GList        *uri_pairs);
void athena_directory_schedule_metadata_remove_by_uri (GList        *uris);
void athena_directory_schedule_position_set    (GList        *position_setting_list);

/* Change notification hack.
 * This is called when code modifies the file and it needs to trigger
 * a notification. Eventually this should become private, but for now
 * it needs to be used for code like the thumbnail generation.
 */
void athena_file_changed                       (AthenaFile *file);
