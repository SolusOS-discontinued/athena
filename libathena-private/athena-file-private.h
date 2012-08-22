/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-file-private.h:
 
   Copyright (C) 1999, 2000, 2001 Eazel, Inc.
  
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

#ifndef ATHENA_FILE_PRIVATE_H
#define ATHENA_FILE_PRIVATE_H

#include <libathena-private/athena-directory.h>
#include <libathena-private/athena-file.h>
#include <libathena-private/athena-monitor.h>
#include <libathena-private/athena-file-undo-operations.h>
#include <eel/eel-glib-extensions.h>
#include <eel/eel-string.h>

#define ATHENA_FILE_LARGE_TOP_LEFT_TEXT_MAXIMUM_CHARACTERS_PER_LINE 80
#define ATHENA_FILE_LARGE_TOP_LEFT_TEXT_MAXIMUM_LINES               24
#define ATHENA_FILE_LARGE_TOP_LEFT_TEXT_MAXIMUM_BYTES               10000

#define ATHENA_FILE_TOP_LEFT_TEXT_MAXIMUM_CHARACTERS_PER_LINE 10
#define ATHENA_FILE_TOP_LEFT_TEXT_MAXIMUM_LINES               5
#define ATHENA_FILE_TOP_LEFT_TEXT_MAXIMUM_BYTES               1024

#define ATHENA_FILE_DEFAULT_ATTRIBUTES				\
	"standard::*,access::*,mountable::*,time::*,unix::*,owner::*,selinux::*,thumbnail::*,id::filesystem,trash::orig-path,trash::deletion-date,metadata::*"

/* These are in the typical sort order. Known things come first, then
 * things where we can't know, finally things where we don't yet know.
 */
typedef enum {
	KNOWN,
	UNKNOWABLE,
	UNKNOWN
} Knowledge;

struct AthenaFileDetails
{
	AthenaDirectory *directory;
	
	eel_ref_str name;

	/* File info: */
	GFileType type;

	eel_ref_str display_name;
	char *display_name_collation_key;
	eel_ref_str edit_name;

	goffset size; /* -1 is unknown */
	
	int sort_order;
	
	guint32 permissions;
	int uid; /* -1 is none */
	int gid; /* -1 is none */

	eel_ref_str owner;
	eel_ref_str owner_real;
	eel_ref_str group;
	
	time_t atime; /* 0 is unknown */
	time_t mtime; /* 0 is unknown */
	time_t ctime; /* 0 is unknown */
	
	char *symlink_name;
	
	eel_ref_str mime_type;
	
	char *selinux_context;
	char *description;
	
	GError *get_info_error;
	
	guint directory_count;

	guint deep_directory_count;
	guint deep_file_count;
	guint deep_unreadable_count;
	goffset deep_size;

	GIcon *icon;
	
	char *thumbnail_path;
	GdkPixbuf *thumbnail;
	time_t thumbnail_mtime;
	
	GList *mime_list; /* If this is a directory, the list of MIME types in it. */
	char *top_left_text;

	/* Info you might get from a link (.desktop, .directory or athena link) */
	GIcon *custom_icon;
	char *activation_uri;

	/* used during DND, for checking whether source and destination are on
	 * the same file system.
	 */
	eel_ref_str filesystem_id;

	char *trash_orig_path;

	/* The following is for file operations in progress. Since
	 * there are normally only a few of these, we can move them to
	 * a separate hash table or something if required to keep the
	 * file objects small.
	 */
	GList *operations_in_progress;

	/* AthenaInfoProviders that need to be run for this file */
	GList *pending_info_providers;

	/* Emblems provided by extensions */
	GList *extension_emblems;
	GList *pending_extension_emblems;

	/* Attributes provided by extensions */
	GHashTable *extension_attributes;
	GHashTable *pending_extension_attributes;

	GHashTable *metadata;

	/* Mount for mountpoint or the references GMount for a "mountable" */
	GMount *mount;
	
	/* boolean fields: bitfield to save space, since there can be
           many AthenaFile objects. */

	eel_boolean_bit unconfirmed                   : 1;
	eel_boolean_bit is_gone                       : 1;
	/* Set when emitting files_added on the directory to make sure we
	   add a file, and only once */
	eel_boolean_bit is_added                      : 1;
	/* Set by the AthenaDirectory while it's loading the file
	 * list so the file knows not to do redundant I/O.
	 */
	eel_boolean_bit loading_directory             : 1;
	eel_boolean_bit got_file_info                 : 1;
	eel_boolean_bit get_info_failed               : 1;
	eel_boolean_bit file_info_is_up_to_date       : 1;
	
	eel_boolean_bit got_directory_count           : 1;
	eel_boolean_bit directory_count_failed        : 1;
	eel_boolean_bit directory_count_is_up_to_date : 1;

	eel_boolean_bit deep_counts_status      : 2; /* AthenaRequestStatus */
	/* no deep_counts_are_up_to_date field; since we expose
           intermediate values for this attribute, we do actually
           forget it rather than invalidating. */

	eel_boolean_bit got_mime_list                 : 1;
	eel_boolean_bit mime_list_failed              : 1;
	eel_boolean_bit mime_list_is_up_to_date       : 1;

	eel_boolean_bit mount_is_up_to_date           : 1;
	
	eel_boolean_bit got_top_left_text             : 1;
	eel_boolean_bit got_large_top_left_text       : 1;
	eel_boolean_bit top_left_text_is_up_to_date   : 1;

	eel_boolean_bit got_link_info                 : 1;
	eel_boolean_bit link_info_is_up_to_date       : 1;
	eel_boolean_bit got_custom_display_name       : 1;
	eel_boolean_bit got_custom_activation_uri     : 1;

	eel_boolean_bit thumbnail_is_up_to_date       : 1;
	eel_boolean_bit thumbnail_wants_original      : 1;
	eel_boolean_bit thumbnail_tried_original      : 1;
	eel_boolean_bit thumbnailing_failed           : 1;
	
	eel_boolean_bit is_thumbnailing               : 1;

	/* TRUE if the file is open in a spatial window */
	eel_boolean_bit has_open_window               : 1;

	eel_boolean_bit is_launcher                   : 1;
	eel_boolean_bit is_trusted_link               : 1;
	eel_boolean_bit is_foreign_link               : 1;
	eel_boolean_bit is_symlink                    : 1;
	eel_boolean_bit is_mountpoint                 : 1;
	eel_boolean_bit is_hidden                     : 1;

	eel_boolean_bit has_permissions               : 1;
	
	eel_boolean_bit can_read                      : 1;
	eel_boolean_bit can_write                     : 1;
	eel_boolean_bit can_execute                   : 1;
	eel_boolean_bit can_delete                    : 1;
	eel_boolean_bit can_trash                     : 1;
	eel_boolean_bit can_rename                    : 1;
	eel_boolean_bit can_mount                     : 1;
	eel_boolean_bit can_unmount                   : 1;
	eel_boolean_bit can_eject                     : 1;
	eel_boolean_bit can_start                     : 1;
	eel_boolean_bit can_start_degraded            : 1;
	eel_boolean_bit can_stop                      : 1;
	eel_boolean_bit start_stop_type               : 3; /* GDriveStartStopType */
	eel_boolean_bit can_poll_for_media            : 1;
	eel_boolean_bit is_media_check_automatic      : 1;

	eel_boolean_bit filesystem_readonly           : 1;
	eel_boolean_bit filesystem_use_preview        : 2; /* GFilesystemPreviewType */
	eel_boolean_bit filesystem_info_is_up_to_date : 1;

	time_t trash_time; /* 0 is unknown */

	guint64 free_space; /* (guint)-1 for unknown */
	time_t free_space_read; /* The time free_space was updated, or 0 for never */
};

typedef struct {
	AthenaFile *file;
	GCancellable *cancellable;
	AthenaFileOperationCallback callback;
	gpointer callback_data;
	gboolean is_rename;
	
	gpointer data;
	GDestroyNotify free_data;
	AthenaFileUndoInfo *undo_info;
} AthenaFileOperation;

AthenaFile *athena_file_new_from_info                  (AthenaDirectory      *directory,
							    GFileInfo              *info);
void          athena_file_emit_changed                   (AthenaFile           *file);
void          athena_file_mark_gone                      (AthenaFile           *file);
char *        athena_extract_top_left_text               (const char             *text,
							    gboolean                large,
							    int                     length);
void          athena_file_set_directory                  (AthenaFile           *file,
							    AthenaDirectory      *directory);
gboolean      athena_file_get_date                       (AthenaFile           *file,
							    AthenaDateType        date_type,
							    time_t                 *date);
void          athena_file_updated_deep_count_in_progress (AthenaFile           *file);


void          athena_file_clear_info                     (AthenaFile           *file);
/* Compare file's state with a fresh file info struct, return FALSE if
 * no change, update file and return TRUE if the file info contains
 * new state.  */
gboolean      athena_file_update_info                    (AthenaFile           *file,
							    GFileInfo              *info);
gboolean      athena_file_update_name                    (AthenaFile           *file,
							    const char             *name);
gboolean      athena_file_update_metadata_from_info      (AthenaFile           *file,
							    GFileInfo              *info);

gboolean      athena_file_update_name_and_directory      (AthenaFile           *file,
							    const char             *name,
							    AthenaDirectory      *directory);

gboolean      athena_file_set_display_name               (AthenaFile           *file,
							    const char             *display_name,
							    const char             *edit_name,
							    gboolean                custom);
void          athena_file_set_mount                      (AthenaFile           *file,
							    GMount                 *mount);

/* Return true if the top lefts of files in this directory should be
 * fetched, according to the preference settings.
 */
gboolean      athena_file_should_get_top_left_text       (AthenaFile           *file);

/* Mark specified attributes for this file out of date without canceling current
 * I/O or kicking off new I/O.
 */
void                   athena_file_invalidate_attributes_internal     (AthenaFile           *file,
									 AthenaFileAttributes  file_attributes);
AthenaFileAttributes athena_file_get_all_attributes                 (void);
gboolean               athena_file_is_self_owned                      (AthenaFile           *file);
void                   athena_file_invalidate_count_and_mime_list     (AthenaFile           *file);
gboolean               athena_file_rename_in_progress                 (AthenaFile           *file);
void                   athena_file_invalidate_extension_info_internal (AthenaFile           *file);
void                   athena_file_info_providers_done                (AthenaFile           *file);


/* Thumbnailing: */
void          athena_file_set_is_thumbnailing            (AthenaFile           *file,
							    gboolean                is_thumbnailing);

AthenaFileOperation *athena_file_operation_new      (AthenaFile                  *file,
							 AthenaFileOperationCallback  callback,
							 gpointer                       callback_data);
void                   athena_file_operation_free     (AthenaFileOperation         *op);
void                   athena_file_operation_complete (AthenaFileOperation         *op,
							 GFile                         *result_location,
							 GError                        *error);
void                   athena_file_operation_cancel   (AthenaFileOperation         *op);

#endif
