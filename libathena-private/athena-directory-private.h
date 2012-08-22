/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-directory-private.h: Athena directory model.
 
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

#include <gio/gio.h>
#include <eel/eel-vfs-extensions.h>
#include <libathena-private/athena-directory.h>
#include <libathena-private/athena-file-queue.h>
#include <libathena-private/athena-file.h>
#include <libathena-private/athena-monitor.h>
#include <libathena-extension/athena-info-provider.h>
#include <libxml/tree.h>

typedef struct LinkInfoReadState LinkInfoReadState;
typedef struct TopLeftTextReadState TopLeftTextReadState;
typedef struct FileMonitors FileMonitors;
typedef struct DirectoryLoadState DirectoryLoadState;
typedef struct DirectoryCountState DirectoryCountState;
typedef struct DeepCountState DeepCountState;
typedef struct GetInfoState GetInfoState;
typedef struct NewFilesState NewFilesState;
typedef struct MimeListState MimeListState;
typedef struct ThumbnailState ThumbnailState;
typedef struct MountState MountState;
typedef struct FilesystemInfoState FilesystemInfoState;

typedef enum {
	REQUEST_LINK_INFO,
	REQUEST_DEEP_COUNT,
	REQUEST_DIRECTORY_COUNT,
	REQUEST_FILE_INFO,
	REQUEST_FILE_LIST, /* always FALSE if file != NULL */
	REQUEST_MIME_LIST,
	REQUEST_TOP_LEFT_TEXT,
	REQUEST_LARGE_TOP_LEFT_TEXT,
	REQUEST_EXTENSION_INFO,
	REQUEST_THUMBNAIL,
	REQUEST_MOUNT,
	REQUEST_FILESYSTEM_INFO,
	REQUEST_TYPE_LAST
} RequestType;

/* A request for information about one or more files. */
typedef guint32 Request;
typedef gint32 RequestCounter[REQUEST_TYPE_LAST];

#define REQUEST_WANTS_TYPE(request, type) ((request) & (1<<(type)))
#define REQUEST_SET_TYPE(request, type) (request) |= (1<<(type))

struct AthenaDirectoryDetails
{
	/* The location. */
	GFile *location;

	/* The file objects. */
	AthenaFile *as_file;
	GList *file_list;
	GHashTable *file_hash;

	/* Queues of files needing some I/O done. */
	AthenaFileQueue *high_priority_queue;
	AthenaFileQueue *low_priority_queue;
	AthenaFileQueue *extension_queue;

	/* These lists are going to be pretty short.  If we think they
	 * are going to get big, we can use hash tables instead.
	 */
	GList *call_when_ready_list;
	RequestCounter call_when_ready_counters;
	GList *monitor_list;
	RequestCounter monitor_counters;
	guint call_ready_idle_id;

	AthenaMonitor *monitor;
	gulong 		 mime_db_monitor;

	gboolean in_async_service_loop;
	gboolean state_changed;

	gboolean file_list_monitored;
	gboolean directory_loaded;
	gboolean directory_loaded_sent_notification;
	DirectoryLoadState *directory_load_in_progress;

	GList *pending_file_info; /* list of GnomeVFSFileInfo's that are pending */
	int confirmed_file_count;
        guint dequeue_pending_idle_id;

	GList *new_files_in_progress; /* list of NewFilesState * */

	DirectoryCountState *count_in_progress;

	AthenaFile *deep_count_file;
	DeepCountState *deep_count_in_progress;

	MimeListState *mime_list_in_progress;

	AthenaFile *get_info_file;
	GetInfoState *get_info_in_progress;

	AthenaFile *extension_info_file;
	AthenaInfoProvider *extension_info_provider;
	AthenaOperationHandle *extension_info_in_progress;
	guint extension_info_idle;

	ThumbnailState *thumbnail_state;

	MountState *mount_state;

	FilesystemInfoState *filesystem_info_state;
	
	TopLeftTextReadState *top_left_read_state;

	LinkInfoReadState *link_info_read_state;

	GList *file_operations_in_progress; /* list of FileOperation * */

	GHashTable *hidden_file_hash;
};

AthenaDirectory *athena_directory_get_existing                    (GFile                     *location);

/* async. interface */
void               athena_directory_async_state_changed             (AthenaDirectory         *directory);
void               athena_directory_call_when_ready_internal        (AthenaDirectory         *directory,
								       AthenaFile              *file,
								       AthenaFileAttributes     file_attributes,
								       gboolean                   wait_for_file_list,
								       AthenaDirectoryCallback  directory_callback,
								       AthenaFileCallback       file_callback,
								       gpointer                   callback_data);
gboolean           athena_directory_check_if_ready_internal         (AthenaDirectory         *directory,
								       AthenaFile              *file,
								       AthenaFileAttributes     file_attributes);
void               athena_directory_cancel_callback_internal        (AthenaDirectory         *directory,
								       AthenaFile              *file,
								       AthenaDirectoryCallback  directory_callback,
								       AthenaFileCallback       file_callback,
								       gpointer                   callback_data);
void               athena_directory_monitor_add_internal            (AthenaDirectory         *directory,
								       AthenaFile              *file,
								       gconstpointer              client,
								       gboolean                   monitor_hidden_files,
								       AthenaFileAttributes     attributes,
								       AthenaDirectoryCallback  callback,
								       gpointer                   callback_data);
void               athena_directory_monitor_remove_internal         (AthenaDirectory         *directory,
								       AthenaFile              *file,
								       gconstpointer              client);
void               athena_directory_get_info_for_new_files          (AthenaDirectory         *directory,
								       GList                     *vfs_uris);
AthenaFile *     athena_directory_get_existing_corresponding_file (AthenaDirectory         *directory);
void               athena_directory_invalidate_count_and_mime_list  (AthenaDirectory         *directory);
gboolean           athena_directory_is_file_list_monitored          (AthenaDirectory         *directory);
gboolean           athena_directory_is_anyone_monitoring_file_list  (AthenaDirectory         *directory);
gboolean           athena_directory_has_active_request_for_file     (AthenaDirectory         *directory,
								       AthenaFile              *file);
void               athena_directory_remove_file_monitor_link        (AthenaDirectory         *directory,
								       GList                     *link);
void               athena_directory_schedule_dequeue_pending        (AthenaDirectory         *directory);
void               athena_directory_stop_monitoring_file_list       (AthenaDirectory         *directory);
void               athena_directory_cancel                          (AthenaDirectory         *directory);
void               athena_async_destroying_file                     (AthenaFile              *file);
void               athena_directory_force_reload_internal           (AthenaDirectory         *directory,
								       AthenaFileAttributes     file_attributes);
void               athena_directory_cancel_loading_file_attributes  (AthenaDirectory         *directory,
								       AthenaFile              *file,
								       AthenaFileAttributes     file_attributes);

/* Calls shared between directory, file, and async. code. */
void               athena_directory_emit_files_added                (AthenaDirectory         *directory,
								       GList                     *added_files);
void               athena_directory_emit_files_changed              (AthenaDirectory         *directory,
								       GList                     *changed_files);
void               athena_directory_emit_change_signals             (AthenaDirectory         *directory,
								       GList                     *changed_files);
void               emit_change_signals_for_all_files		      (AthenaDirectory	 *directory);
void               emit_change_signals_for_all_files_in_all_directories (void);
void               athena_directory_emit_done_loading               (AthenaDirectory         *directory);
void               athena_directory_emit_load_error                 (AthenaDirectory         *directory,
								       GError                    *error);
AthenaDirectory *athena_directory_get_internal                    (GFile                     *location,
								       gboolean                   create);
char *             athena_directory_get_name_for_self_as_new_file   (AthenaDirectory         *directory);
Request            athena_directory_set_up_request                  (AthenaFileAttributes     file_attributes);

/* Interface to the file list. */
AthenaFile *     athena_directory_find_file_by_name               (AthenaDirectory         *directory,
								       const char                *filename);
AthenaFile *     athena_directory_find_file_by_internal_filename  (AthenaDirectory         *directory,
								       const char                *internal_filename);

void               athena_directory_add_file                        (AthenaDirectory         *directory,
								       AthenaFile              *file);
void               athena_directory_remove_file                     (AthenaDirectory         *directory,
								       AthenaFile              *file);
FileMonitors *     athena_directory_remove_file_monitors            (AthenaDirectory         *directory,
								       AthenaFile              *file);
void               athena_directory_add_file_monitors               (AthenaDirectory         *directory,
								       AthenaFile              *file,
								       FileMonitors              *monitors);
void               athena_directory_add_file                        (AthenaDirectory         *directory,
								       AthenaFile              *file);
GList *            athena_directory_begin_file_name_change          (AthenaDirectory         *directory,
								       AthenaFile              *file);
void               athena_directory_end_file_name_change            (AthenaDirectory         *directory,
								       AthenaFile              *file,
								       GList                     *node);
void               athena_directory_moved                           (const char                *from_uri,
								       const char                *to_uri);
/* Interface to the work queue. */

void               athena_directory_add_file_to_work_queue          (AthenaDirectory *directory,
								       AthenaFile *file);
void               athena_directory_remove_file_from_work_queue     (AthenaDirectory *directory,
								       AthenaFile *file);

/* KDE compatibility hacks */

void               athena_set_kde_trash_name                        (const char *trash_dir);

/* debugging functions */
int                athena_directory_number_outstanding              (void);
