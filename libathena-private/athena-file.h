/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-file.h: Athena file model.
 
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

#ifndef ATHENA_FILE_H
#define ATHENA_FILE_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <libathena-private/athena-file-attributes.h>
#include <libathena-private/athena-icon-info.h>

/* AthenaFile is an object used to represent a single element of a
 * AthenaDirectory. It's lightweight and relies on AthenaDirectory
 * to do most of the work.
 */

/* AthenaFile is defined both here and in athena-directory.h. */
#ifndef ATHENA_FILE_DEFINED
#define ATHENA_FILE_DEFINED
typedef struct AthenaFile AthenaFile;
#endif

#define ATHENA_TYPE_FILE athena_file_get_type()
#define ATHENA_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_FILE, AthenaFile))
#define ATHENA_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_FILE, AthenaFileClass))
#define ATHENA_IS_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_FILE))
#define ATHENA_IS_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_FILE))
#define ATHENA_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_FILE, AthenaFileClass))

typedef enum {
	ATHENA_FILE_SORT_NONE,
	ATHENA_FILE_SORT_BY_DISPLAY_NAME,
	ATHENA_FILE_SORT_BY_SIZE,
	ATHENA_FILE_SORT_BY_TYPE,
	ATHENA_FILE_SORT_BY_MTIME,
        ATHENA_FILE_SORT_BY_ATIME,
	ATHENA_FILE_SORT_BY_TRASHED_TIME
} AthenaFileSortType;	

typedef enum {
	ATHENA_REQUEST_NOT_STARTED,
	ATHENA_REQUEST_IN_PROGRESS,
	ATHENA_REQUEST_DONE
} AthenaRequestStatus;

typedef enum {
	ATHENA_FILE_ICON_FLAGS_NONE = 0,
	ATHENA_FILE_ICON_FLAGS_USE_THUMBNAILS = (1<<0),
	ATHENA_FILE_ICON_FLAGS_IGNORE_VISITING = (1<<1),
	ATHENA_FILE_ICON_FLAGS_EMBEDDING_TEXT = (1<<2),
	ATHENA_FILE_ICON_FLAGS_FOR_DRAG_ACCEPT = (1<<3),
	ATHENA_FILE_ICON_FLAGS_FOR_OPEN_FOLDER = (1<<4),
	/* whether the thumbnail size must match the display icon size */
	ATHENA_FILE_ICON_FLAGS_FORCE_THUMBNAIL_SIZE = (1<<5),
	/* uses the icon of the mount if present */
	ATHENA_FILE_ICON_FLAGS_USE_MOUNT_ICON = (1<<6),
	/* render the mount icon as an emblem over the regular one */
	ATHENA_FILE_ICON_FLAGS_USE_MOUNT_ICON_AS_EMBLEM = (1<<7)
} AthenaFileIconFlags;	

/* Emblems sometimes displayed for AthenaFiles. Do not localize. */ 
#define ATHENA_FILE_EMBLEM_NAME_SYMBOLIC_LINK "symbolic-link"
#define ATHENA_FILE_EMBLEM_NAME_CANT_READ "noread"
#define ATHENA_FILE_EMBLEM_NAME_CANT_WRITE "nowrite"
#define ATHENA_FILE_EMBLEM_NAME_TRASH "trash"
#define ATHENA_FILE_EMBLEM_NAME_NOTE "note"

typedef void (*AthenaFileCallback)          (AthenaFile  *file,
				               gpointer       callback_data);
typedef void (*AthenaFileListCallback)      (GList         *file_list,
				               gpointer       callback_data);
typedef void (*AthenaFileOperationCallback) (AthenaFile  *file,
					       GFile         *result_location,
					       GError        *error,
					       gpointer       callback_data);
typedef int (*AthenaWidthMeasureCallback)   (const char    *string,
					       void	     *context);
typedef char * (*AthenaTruncateCallback)    (const char    *string,
					       int	      width,
					       void	     *context);


#define ATHENA_FILE_ATTRIBUTES_FOR_ICON (ATHENA_FILE_ATTRIBUTE_INFO | ATHENA_FILE_ATTRIBUTE_LINK_INFO | ATHENA_FILE_ATTRIBUTE_THUMBNAIL)

typedef void AthenaFileListHandle;

/* GObject requirements. */
GType                   athena_file_get_type                          (void);

/* Getting at a single file. */
AthenaFile *          athena_file_get                               (GFile                          *location);
AthenaFile *          athena_file_get_by_uri                        (const char                     *uri);

/* Get a file only if the athena version already exists */
AthenaFile *          athena_file_get_existing                      (GFile                          *location);
AthenaFile *          athena_file_get_existing_by_uri               (const char                     *uri);

/* Covers for g_object_ref and g_object_unref that provide two conveniences:
 * 1) Using these is type safe.
 * 2) You are allowed to call these with NULL,
 */
AthenaFile *          athena_file_ref                               (AthenaFile                   *file);
void                    athena_file_unref                             (AthenaFile                   *file);

/* Monitor the file. */
void                    athena_file_monitor_add                       (AthenaFile                   *file,
									 gconstpointer                   client,
									 AthenaFileAttributes          attributes);
void                    athena_file_monitor_remove                    (AthenaFile                   *file,
									 gconstpointer                   client);

/* Waiting for data that's read asynchronously.
 * This interface currently works only for metadata, but could be expanded
 * to other attributes as well.
 */
void                    athena_file_call_when_ready                   (AthenaFile                   *file,
									 AthenaFileAttributes          attributes,
									 AthenaFileCallback            callback,
									 gpointer                        callback_data);
void                    athena_file_cancel_call_when_ready            (AthenaFile                   *file,
									 AthenaFileCallback            callback,
									 gpointer                        callback_data);
gboolean                athena_file_check_if_ready                    (AthenaFile                   *file,
									 AthenaFileAttributes          attributes);
void                    athena_file_invalidate_attributes             (AthenaFile                   *file,
									 AthenaFileAttributes          attributes);
void                    athena_file_invalidate_all_attributes         (AthenaFile                   *file);

/* Basic attributes for file objects. */
gboolean                athena_file_contains_text                     (AthenaFile                   *file);
char *                  athena_file_get_display_name                  (AthenaFile                   *file);
char *                  athena_file_get_edit_name                     (AthenaFile                   *file);
char *                  athena_file_get_name                          (AthenaFile                   *file);
GFile *                 athena_file_get_location                      (AthenaFile                   *file);
char *			 athena_file_get_description			 (AthenaFile			 *file);
char *                  athena_file_get_uri                           (AthenaFile                   *file);
char *                  athena_file_get_uri_scheme                    (AthenaFile                   *file);
AthenaFile *          athena_file_get_parent                        (AthenaFile                   *file);
GFile *                 athena_file_get_parent_location               (AthenaFile                   *file);
char *                  athena_file_get_parent_uri                    (AthenaFile                   *file);
char *                  athena_file_get_parent_uri_for_display        (AthenaFile                   *file);
gboolean                athena_file_can_get_size                      (AthenaFile                   *file);
goffset                 athena_file_get_size                          (AthenaFile                   *file);
time_t                  athena_file_get_mtime                         (AthenaFile                   *file);
GFileType               athena_file_get_file_type                     (AthenaFile                   *file);
char *                  athena_file_get_mime_type                     (AthenaFile                   *file);
gboolean                athena_file_is_mime_type                      (AthenaFile                   *file,
									 const char                     *mime_type);
gboolean                athena_file_is_launchable                     (AthenaFile                   *file);
gboolean                athena_file_is_symbolic_link                  (AthenaFile                   *file);
gboolean                athena_file_is_mountpoint                     (AthenaFile                   *file);
GMount *                athena_file_get_mount                         (AthenaFile                   *file);
char *                  athena_file_get_volume_free_space             (AthenaFile                   *file);
char *                  athena_file_get_volume_name                   (AthenaFile                   *file);
char *                  athena_file_get_symbolic_link_target_path     (AthenaFile                   *file);
char *                  athena_file_get_symbolic_link_target_uri      (AthenaFile                   *file);
gboolean                athena_file_is_broken_symbolic_link           (AthenaFile                   *file);
gboolean                athena_file_is_athena_link                  (AthenaFile                   *file);
gboolean                athena_file_is_executable                     (AthenaFile                   *file);
gboolean                athena_file_is_directory                      (AthenaFile                   *file);
gboolean                athena_file_is_user_special_directory         (AthenaFile                   *file,
									 GUserDirectory                 special_directory);
gboolean		athena_file_is_archive			(AthenaFile			*file);
gboolean                athena_file_is_in_trash                       (AthenaFile                   *file);
gboolean                athena_file_is_in_desktop                     (AthenaFile                   *file);
gboolean		athena_file_is_home				(AthenaFile                   *file);
gboolean                athena_file_is_desktop_directory              (AthenaFile                   *file);
GError *                athena_file_get_file_info_error               (AthenaFile                   *file);
gboolean                athena_file_get_directory_item_count          (AthenaFile                   *file,
									 guint                          *count,
									 gboolean                       *count_unreadable);
void                    athena_file_recompute_deep_counts             (AthenaFile                   *file);
AthenaRequestStatus   athena_file_get_deep_counts                   (AthenaFile                   *file,
									 guint                          *directory_count,
									 guint                          *file_count,
									 guint                          *unreadable_directory_count,
									 goffset               *total_size,
									 gboolean                        force);
gboolean                athena_file_should_show_thumbnail             (AthenaFile                   *file);
gboolean                athena_file_should_show_directory_item_count  (AthenaFile                   *file);
gboolean                athena_file_should_show_type                  (AthenaFile                   *file);
GList *                 athena_file_get_keywords                      (AthenaFile                   *file);
GList *                 athena_file_get_emblem_icons                  (AthenaFile                   *file,
									 char                          **exclude);
char *                  athena_file_get_top_left_text                 (AthenaFile                   *file);
char *                  athena_file_peek_top_left_text                (AthenaFile                   *file,
									 gboolean                        need_large_text,
									 gboolean                       *got_top_left_text);
gboolean                athena_file_get_directory_item_mime_types     (AthenaFile                   *file,
									 GList                         **mime_list);

void                    athena_file_set_attributes                    (AthenaFile                   *file, 
									 GFileInfo                      *attributes,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
GFilesystemPreviewType  athena_file_get_filesystem_use_preview        (AthenaFile *file);

char *                  athena_file_get_filesystem_id                 (AthenaFile                   *file);

AthenaFile *          athena_file_get_trash_original_file           (AthenaFile                   *file);

/* Permissions. */
gboolean                athena_file_can_get_permissions               (AthenaFile                   *file);
gboolean                athena_file_can_set_permissions               (AthenaFile                   *file);
guint                   athena_file_get_permissions                   (AthenaFile                   *file);
gboolean                athena_file_can_get_owner                     (AthenaFile                   *file);
gboolean                athena_file_can_set_owner                     (AthenaFile                   *file);
gboolean                athena_file_can_get_group                     (AthenaFile                   *file);
gboolean                athena_file_can_set_group                     (AthenaFile                   *file);
char *                  athena_file_get_owner_name                    (AthenaFile                   *file);
char *                  athena_file_get_group_name                    (AthenaFile                   *file);
GList *                 athena_get_user_names                         (void);
GList *                 athena_get_all_group_names                    (void);
GList *                 athena_file_get_settable_group_names          (AthenaFile                   *file);
gboolean                athena_file_can_get_selinux_context           (AthenaFile                   *file);
char *                  athena_file_get_selinux_context               (AthenaFile                   *file);

/* "Capabilities". */
gboolean                athena_file_can_read                          (AthenaFile                   *file);
gboolean                athena_file_can_write                         (AthenaFile                   *file);
gboolean                athena_file_can_execute                       (AthenaFile                   *file);
gboolean                athena_file_can_rename                        (AthenaFile                   *file);
gboolean                athena_file_can_delete                        (AthenaFile                   *file);
gboolean                athena_file_can_trash                         (AthenaFile                   *file);

gboolean                athena_file_can_mount                         (AthenaFile                   *file);
gboolean                athena_file_can_unmount                       (AthenaFile                   *file);
gboolean                athena_file_can_eject                         (AthenaFile                   *file);
gboolean                athena_file_can_start                         (AthenaFile                   *file);
gboolean                athena_file_can_start_degraded                (AthenaFile                   *file);
gboolean                athena_file_can_stop                          (AthenaFile                   *file);
GDriveStartStopType     athena_file_get_start_stop_type               (AthenaFile                   *file);
gboolean                athena_file_can_poll_for_media                (AthenaFile                   *file);
gboolean                athena_file_is_media_check_automatic          (AthenaFile                   *file);

void                    athena_file_mount                             (AthenaFile                   *file,
									 GMountOperation                *mount_op,
									 GCancellable                   *cancellable,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    athena_file_unmount                           (AthenaFile                   *file,
									 GMountOperation                *mount_op,
									 GCancellable                   *cancellable,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    athena_file_eject                             (AthenaFile                   *file,
									 GMountOperation                *mount_op,
									 GCancellable                   *cancellable,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);

void                    athena_file_start                             (AthenaFile                   *file,
									 GMountOperation                *start_op,
									 GCancellable                   *cancellable,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    athena_file_stop                              (AthenaFile                   *file,
									 GMountOperation                *mount_op,
									 GCancellable                   *cancellable,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    athena_file_poll_for_media                    (AthenaFile                   *file);

/* Basic operations for file objects. */
void                    athena_file_set_owner                         (AthenaFile                   *file,
									 const char                     *user_name_or_id,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    athena_file_set_group                         (AthenaFile                   *file,
									 const char                     *group_name_or_id,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    athena_file_set_permissions                   (AthenaFile                   *file,
									 guint32                         permissions,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    athena_file_rename                            (AthenaFile                   *file,
									 const char                     *new_name,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    athena_file_cancel                            (AthenaFile                   *file,
									 AthenaFileOperationCallback   callback,
									 gpointer                        callback_data);

/* Return true if this file has already been deleted.
 * This object will be unref'd after sending the files_removed signal,
 * but it could hang around longer if someone ref'd it.
 */
gboolean                athena_file_is_gone                           (AthenaFile                   *file);

/* Return true if this file is not confirmed to have ever really
 * existed. This is true when the AthenaFile object has been created, but no I/O
 * has yet confirmed the existence of a file by that name.
 */
gboolean                athena_file_is_not_yet_confirmed              (AthenaFile                   *file);

/* Simple getting and setting top-level metadata. */
char *                  athena_file_get_metadata                      (AthenaFile                   *file,
									 const char                     *key,
									 const char                     *default_metadata);
GList *                 athena_file_get_metadata_list                 (AthenaFile                   *file,
									 const char                     *key);
void                    athena_file_set_metadata                      (AthenaFile                   *file,
									 const char                     *key,
									 const char                     *default_metadata,
									 const char                     *metadata);
void                    athena_file_set_metadata_list                 (AthenaFile                   *file,
									 const char                     *key,
									 GList                          *list);

/* Covers for common data types. */
gboolean                athena_file_get_boolean_metadata              (AthenaFile                   *file,
									 const char                     *key,
									 gboolean                        default_metadata);
void                    athena_file_set_boolean_metadata              (AthenaFile                   *file,
									 const char                     *key,
									 gboolean                        default_metadata,
									 gboolean                        metadata);
int                     athena_file_get_integer_metadata              (AthenaFile                   *file,
									 const char                     *key,
									 int                             default_metadata);
void                    athena_file_set_integer_metadata              (AthenaFile                   *file,
									 const char                     *key,
									 int                             default_metadata,
									 int                             metadata);

#define UNDEFINED_TIME ((time_t) (-1))

time_t                  athena_file_get_time_metadata                 (AthenaFile                  *file,
									 const char                    *key);
void                    athena_file_set_time_metadata                 (AthenaFile                  *file,
									 const char                    *key,
									 time_t                         time);


/* Attributes for file objects as user-displayable strings. */
char *                  athena_file_get_string_attribute              (AthenaFile                   *file,
									 const char                     *attribute_name);
char *                  athena_file_get_string_attribute_q            (AthenaFile                   *file,
									 GQuark                          attribute_q);
char *                  athena_file_get_string_attribute_with_default (AthenaFile                   *file,
									 const char                     *attribute_name);
char *                  athena_file_get_string_attribute_with_default_q (AthenaFile                  *file,
									 GQuark                          attribute_q);
char *			athena_file_fit_modified_date_as_string	(AthenaFile 			*file,
									 int				 width,
									 AthenaWidthMeasureCallback    measure_callback,
									 AthenaTruncateCallback	 truncate_callback,
									 void				*measure_truncate_context);

/* Matching with another URI. */
gboolean                athena_file_matches_uri                       (AthenaFile                   *file,
									 const char                     *uri);

/* Is the file local? */
gboolean                athena_file_is_local                          (AthenaFile                   *file);

/* Comparing two file objects for sorting */
AthenaFileSortType    athena_file_get_default_sort_type             (AthenaFile                   *file,
									 gboolean                       *reversed);
const gchar *           athena_file_get_default_sort_attribute        (AthenaFile                   *file,
									 gboolean                       *reversed);

int                     athena_file_compare_for_sort                  (AthenaFile                   *file_1,
									 AthenaFile                   *file_2,
									 AthenaFileSortType            sort_type,
									 gboolean			 directories_first,
									 gboolean		  	 reversed);
int                     athena_file_compare_for_sort_by_attribute     (AthenaFile                   *file_1,
									 AthenaFile                   *file_2,
									 const char                     *attribute,
									 gboolean                        directories_first,
									 gboolean                        reversed);
int                     athena_file_compare_for_sort_by_attribute_q   (AthenaFile                   *file_1,
									 AthenaFile                   *file_2,
									 GQuark                          attribute,
									 gboolean                        directories_first,
									 gboolean                        reversed);
gboolean                athena_file_is_date_sort_attribute_q          (GQuark                          attribute);

int                     athena_file_compare_display_name              (AthenaFile                   *file_1,
									 const char                     *pattern);
int                     athena_file_compare_location                  (AthenaFile                    *file_1,
                                                                         AthenaFile                    *file_2);

/* filtering functions for use by various directory views */
gboolean                athena_file_is_hidden_file                    (AthenaFile                   *file);
gboolean                athena_file_should_show                       (AthenaFile                   *file,
									 gboolean                        show_hidden,
									 gboolean                        show_foreign);
GList                  *athena_file_list_filter_hidden                (GList                          *files,
									 gboolean                        show_hidden);


/* Get the URI that's used when activating the file.
 * Getting this can require reading the contents of the file.
 */
gboolean                athena_file_is_launcher                       (AthenaFile                   *file);
gboolean                athena_file_is_foreign_link                   (AthenaFile                   *file);
gboolean                athena_file_is_trusted_link                   (AthenaFile                   *file);
gboolean                athena_file_has_activation_uri                (AthenaFile                   *file);
char *                  athena_file_get_activation_uri                (AthenaFile                   *file);
GFile *                 athena_file_get_activation_location           (AthenaFile                   *file);

char *                  athena_file_get_drop_target_uri               (AthenaFile                   *file);

GIcon *                 athena_file_get_gicon                         (AthenaFile                   *file,
									 AthenaFileIconFlags           flags);
AthenaIconInfo *      athena_file_get_icon                          (AthenaFile                   *file,
									 int                             size,
									 AthenaFileIconFlags           flags);
GdkPixbuf *             athena_file_get_icon_pixbuf                   (AthenaFile                   *file,
									 int                             size,
									 gboolean                        force_size,
									 AthenaFileIconFlags           flags);

gboolean                athena_file_has_open_window                   (AthenaFile                   *file);
void                    athena_file_set_has_open_window               (AthenaFile                   *file,
									 gboolean                        has_open_window);

/* Thumbnailing handling */
gboolean                athena_file_is_thumbnailing                   (AthenaFile                   *file);

/* Convenience functions for dealing with a list of AthenaFile objects that each have a ref.
 * These are just convenient names for functions that work on lists of GtkObject *.
 */
GList *                 athena_file_list_ref                          (GList                          *file_list);
void                    athena_file_list_unref                        (GList                          *file_list);
void                    athena_file_list_free                         (GList                          *file_list);
GList *                 athena_file_list_copy                         (GList                          *file_list);
GList *                 athena_file_list_from_uris                    (GList                          *uri_list);
GList *			athena_file_list_sort_by_display_name		(GList				*file_list);
void                    athena_file_list_call_when_ready              (GList                          *file_list,
									 AthenaFileAttributes          attributes,
									 AthenaFileListHandle        **handle,
									 AthenaFileListCallback        callback,
									 gpointer                        callback_data);
void                    athena_file_list_cancel_call_when_ready       (AthenaFileListHandle         *handle);

/* Debugging */
void                    athena_file_dump                              (AthenaFile                   *file);

typedef struct AthenaFileDetails AthenaFileDetails;

struct AthenaFile {
	GObject parent_slot;
	AthenaFileDetails *details;
};

/* This is actually a "protected" type, but it must be here so we can
 * compile the get_date function pointer declaration below.
 */
typedef enum {
	ATHENA_DATE_TYPE_MODIFIED,
	ATHENA_DATE_TYPE_CHANGED,
	ATHENA_DATE_TYPE_ACCESSED,
	ATHENA_DATE_TYPE_PERMISSIONS_CHANGED,
	ATHENA_DATE_TYPE_TRASHED
} AthenaDateType;

typedef struct {
	GObjectClass parent_slot;

	/* Subclasses can set this to something other than G_FILE_TYPE_UNKNOWN and
	   it will be used as the default file type. This is useful when creating
	   a "virtual" AthenaFile subclass that you can't actually get real
	   information about. For exaple AthenaDesktopDirectoryFile. */
	GFileType default_file_type; 
	
	/* Called when the file notices any change. */
	void                  (* changed)                (AthenaFile *file);

	/* Called periodically while directory deep count is being computed. */
	void                  (* updated_deep_count_in_progress) (AthenaFile *file);

	/* Virtual functions (mainly used for trash directory). */
	void                  (* monitor_add)            (AthenaFile           *file,
							  gconstpointer           client,
							  AthenaFileAttributes  attributes);
	void                  (* monitor_remove)         (AthenaFile           *file,
							  gconstpointer           client);
	void                  (* call_when_ready)        (AthenaFile           *file,
							  AthenaFileAttributes  attributes,
							  AthenaFileCallback    callback,
							  gpointer                callback_data);
	void                  (* cancel_call_when_ready) (AthenaFile           *file,
							  AthenaFileCallback    callback,
							  gpointer                callback_data);
	gboolean              (* check_if_ready)         (AthenaFile           *file,
							  AthenaFileAttributes  attributes);
	gboolean              (* get_item_count)         (AthenaFile           *file,
							  guint                  *count,
							  gboolean               *count_unreadable);
	AthenaRequestStatus (* get_deep_counts)        (AthenaFile           *file,
							  guint                  *directory_count,
							  guint                  *file_count,
							  guint                  *unreadable_directory_count,
							  goffset       *total_size);
	gboolean              (* get_date)               (AthenaFile           *file,
							  AthenaDateType        type,
							  time_t                 *date);
	char *                (* get_where_string)       (AthenaFile           *file);

	void                  (* set_metadata)           (AthenaFile           *file,
							  const char             *key,
							  const char             *value);
	void                  (* set_metadata_as_list)   (AthenaFile           *file,
							  const char             *key,
							  char                  **value);
	
	void                  (* mount)                  (AthenaFile                   *file,
							  GMountOperation                *mount_op,
							  GCancellable                   *cancellable,
							  AthenaFileOperationCallback   callback,
							  gpointer                        callback_data);
	void                 (* unmount)                 (AthenaFile                   *file,
							  GMountOperation                *mount_op,
							  GCancellable                   *cancellable,
							  AthenaFileOperationCallback   callback,
							  gpointer                        callback_data);
	void                 (* eject)                   (AthenaFile                   *file,
							  GMountOperation                *mount_op,
							  GCancellable                   *cancellable,
							  AthenaFileOperationCallback   callback,
							  gpointer                        callback_data);

	void                  (* start)                  (AthenaFile                   *file,
							  GMountOperation                *start_op,
							  GCancellable                   *cancellable,
							  AthenaFileOperationCallback   callback,
							  gpointer                        callback_data);
	void                 (* stop)                    (AthenaFile                   *file,
							  GMountOperation                *mount_op,
							  GCancellable                   *cancellable,
							  AthenaFileOperationCallback   callback,
							  gpointer                        callback_data);

	void                 (* poll_for_media)          (AthenaFile                   *file);
} AthenaFileClass;

#endif /* ATHENA_FILE_H */
