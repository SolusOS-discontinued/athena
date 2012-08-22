/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-directory.h: Athena directory model.
 
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

#ifndef ATHENA_DIRECTORY_H
#define ATHENA_DIRECTORY_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <libathena-private/athena-file-attributes.h>

/* AthenaDirectory is a class that manages the model for a directory,
   real or virtual, for Athena, mainly the file-manager component. The directory is
   responsible for managing both real data and cached metadata. On top of
   the file system independence provided by gio, the directory
   object also provides:
  
       1) A synchronization framework, which notifies via signals as the
          set of known files changes.
       2) An abstract interface for getting attributes and performing
          operations on files.
*/

#define ATHENA_TYPE_DIRECTORY athena_directory_get_type()
#define ATHENA_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_DIRECTORY, AthenaDirectory))
#define ATHENA_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_DIRECTORY, AthenaDirectoryClass))
#define ATHENA_IS_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_DIRECTORY))
#define ATHENA_IS_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_DIRECTORY))
#define ATHENA_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_DIRECTORY, AthenaDirectoryClass))

/* AthenaFile is defined both here and in athena-file.h. */
#ifndef ATHENA_FILE_DEFINED
#define ATHENA_FILE_DEFINED
typedef struct AthenaFile AthenaFile;
#endif

typedef struct AthenaDirectoryDetails AthenaDirectoryDetails;

typedef struct
{
	GObject object;
	AthenaDirectoryDetails *details;
} AthenaDirectory;

typedef void (*AthenaDirectoryCallback) (AthenaDirectory *directory,
					   GList             *files,
					   gpointer           callback_data);

typedef struct
{
	GObjectClass parent_class;

	/*** Notification signals for clients to connect to. ***/

	/* The files_added signal is emitted as the directory model 
	 * discovers new files.
	 */
	void     (* files_added)         (AthenaDirectory          *directory,
					  GList                      *added_files);

	/* The files_changed signal is emitted as changes occur to
	 * existing files that are noticed by the synchronization framework,
	 * including when an old file has been deleted. When an old file
	 * has been deleted, this is the last chance to forget about these
	 * file objects, which are about to be unref'd. Use a call to
	 * athena_file_is_gone () to test for this case.
	 */
	void     (* files_changed)       (AthenaDirectory         *directory,
					  GList                     *changed_files);

	/* The done_loading signal is emitted when a directory load
	 * request completes. This is needed because, at least in the
	 * case where the directory is empty, the caller will receive
	 * no kind of notification at all when a directory load
	 * initiated by `athena_directory_file_monitor_add' completes.
	 */
	void     (* done_loading)        (AthenaDirectory         *directory);

	void     (* load_error)          (AthenaDirectory         *directory,
					  GError                    *error);

	/*** Virtual functions for subclasses to override. ***/
	gboolean (* contains_file)       (AthenaDirectory         *directory,
					  AthenaFile              *file);
	void     (* call_when_ready)     (AthenaDirectory         *directory,
					  AthenaFileAttributes     file_attributes,
					  gboolean                   wait_for_file_list,
					  AthenaDirectoryCallback  callback,
					  gpointer                   callback_data);
	void     (* cancel_callback)     (AthenaDirectory         *directory,
					  AthenaDirectoryCallback  callback,
					  gpointer                   callback_data);
	void     (* file_monitor_add)    (AthenaDirectory          *directory,
					  gconstpointer              client,
					  gboolean                   monitor_hidden_files,
					  AthenaFileAttributes     monitor_attributes,
					  AthenaDirectoryCallback  initial_files_callback,
					  gpointer                   callback_data);
	void     (* file_monitor_remove) (AthenaDirectory         *directory,
					  gconstpointer              client);
	void     (* force_reload)        (AthenaDirectory         *directory);
	gboolean (* are_all_files_seen)  (AthenaDirectory         *directory);
	gboolean (* is_not_empty)        (AthenaDirectory         *directory);

	/* get_file_list is a function pointer that subclasses may override to
	 * customize collecting the list of files in a directory.
	 * For example, the AthenaDesktopDirectory overrides this so that it can
	 * merge together the list of files in the $HOME/Desktop directory with
	 * the list of standard icons (Computer, Home, Trash) on the desktop.
	 */
	GList *	 (* get_file_list)	 (AthenaDirectory *directory);

	/* Should return FALSE if the directory is read-only and doesn't
	 * allow setting of metadata.
	 * An example of this is the search directory.
	 */
	gboolean (* is_editable)         (AthenaDirectory *directory);
} AthenaDirectoryClass;

/* Basic GObject requirements. */
GType              athena_directory_get_type                 (void);

/* Get a directory given a uri.
 * Creates the appropriate subclass given the uri mappings.
 * Returns a referenced object, not a floating one. Unref when finished.
 * If two windows are viewing the same uri, the directory object is shared.
 */
AthenaDirectory *athena_directory_get                      (GFile                     *location);
AthenaDirectory *athena_directory_get_by_uri               (const char                *uri);
AthenaDirectory *athena_directory_get_for_file             (AthenaFile              *file);

/* Covers for g_object_ref and g_object_unref that provide two conveniences:
 * 1) Using these is type safe.
 * 2) You are allowed to call these with NULL,
 */
AthenaDirectory *athena_directory_ref                      (AthenaDirectory         *directory);
void               athena_directory_unref                    (AthenaDirectory         *directory);

/* Access to a URI. */
char *             athena_directory_get_uri                  (AthenaDirectory         *directory);
GFile *            athena_directory_get_location             (AthenaDirectory         *directory);

/* Is this file still alive and in this directory? */
gboolean           athena_directory_contains_file            (AthenaDirectory         *directory,
								AthenaFile              *file);

/* Get the uri of the file in the directory, NULL if not found */
char *             athena_directory_get_file_uri             (AthenaDirectory         *directory,
								const char                *file_name);

/* Get (and ref) a AthenaFile object for this directory. */
AthenaFile *     athena_directory_get_corresponding_file   (AthenaDirectory         *directory);

/* Waiting for data that's read asynchronously.
 * The file attribute and metadata keys are for files in the directory.
 */
void               athena_directory_call_when_ready          (AthenaDirectory         *directory,
								AthenaFileAttributes     file_attributes,
								gboolean                   wait_for_all_files,
								AthenaDirectoryCallback  callback,
								gpointer                   callback_data);
void               athena_directory_cancel_callback          (AthenaDirectory         *directory,
								AthenaDirectoryCallback  callback,
								gpointer                   callback_data);


/* Monitor the files in a directory. */
void               athena_directory_file_monitor_add         (AthenaDirectory         *directory,
								gconstpointer              client,
								gboolean                   monitor_hidden_files,
								AthenaFileAttributes     attributes,
								AthenaDirectoryCallback  initial_files_callback,
								gpointer                   callback_data);
void               athena_directory_file_monitor_remove      (AthenaDirectory         *directory,
								gconstpointer              client);
void               athena_directory_force_reload             (AthenaDirectory         *directory);

/* Get a list of all files currently known in the directory. */
GList *            athena_directory_get_file_list            (AthenaDirectory         *directory);

GList *            athena_directory_match_pattern            (AthenaDirectory         *directory,
							        const char *glob);


/* Return true if the directory has information about all the files.
 * This will be false until the directory has been read at least once.
 */
gboolean           athena_directory_are_all_files_seen       (AthenaDirectory         *directory);

/* Return true if the directory is local. */
gboolean           athena_directory_is_local                 (AthenaDirectory         *directory);

gboolean           athena_directory_is_in_trash              (AthenaDirectory         *directory);

/* Return false if directory contains anything besides a Athena metafile.
 * Only valid if directory is monitored. Used by the Trash monitor.
 */
gboolean           athena_directory_is_not_empty             (AthenaDirectory         *directory);

/* Convenience functions for dealing with a list of AthenaDirectory objects that each have a ref.
 * These are just convenient names for functions that work on lists of GtkObject *.
 */
GList *            athena_directory_list_ref                 (GList                     *directory_list);
void               athena_directory_list_unref               (GList                     *directory_list);
void               athena_directory_list_free                (GList                     *directory_list);
GList *            athena_directory_list_copy                (GList                     *directory_list);
GList *            athena_directory_list_sort_by_uri         (GList                     *directory_list);

/* Fast way to check if a directory is the desktop directory */
gboolean           athena_directory_is_desktop_directory     (AthenaDirectory         *directory);

gboolean           athena_directory_is_editable              (AthenaDirectory         *directory);


#endif /* ATHENA_DIRECTORY_H */
