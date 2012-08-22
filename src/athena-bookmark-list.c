/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Athena
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
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
 * Authors: John Sullivan <sullivan@eazel.com>
 */

/* athena-bookmark-list.c - implementation of centralized list of bookmarks.
 */

#include <config.h>
#include "athena-bookmark-list.h"

#include <libathena-private/athena-file-utilities.h>
#include <libathena-private/athena-file.h>
#include <libathena-private/athena-icon-names.h>

#include <gio/gio.h>
#include <string.h>

#define MAX_BOOKMARK_LENGTH 80
#define LOAD_JOB 1
#define SAVE_JOB 2

enum {
	CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];
static char *window_geometry;
static AthenaBookmarkList *singleton = NULL;

/* forward declarations */

static void        athena_bookmark_list_load_file     (AthenaBookmarkList *bookmarks);
static void        athena_bookmark_list_save_file     (AthenaBookmarkList *bookmarks);

G_DEFINE_TYPE(AthenaBookmarkList, athena_bookmark_list, G_TYPE_OBJECT)

static AthenaBookmark *
new_bookmark_from_uri (const char *uri, const char *label)
{
	AthenaBookmark *new_bookmark;
	GFile *location;

	location = NULL;
	if (uri) {
		location = g_file_new_for_uri (uri);
	}
	
	new_bookmark = NULL;

	if (location) {
		new_bookmark = athena_bookmark_new (location, label, NULL);
		g_object_unref (location);
	}

	return new_bookmark;
}

static GFile *
athena_bookmark_list_get_file (void)
{
	char *filename;
	GFile *file;

	filename = g_build_filename (g_get_home_dir (),
				     ".gtk-bookmarks",
				     NULL);
	file = g_file_new_for_path (filename);

	g_free (filename);

	return file;
}

/* Initialization.  */

static void
bookmark_in_list_changed_callback (AthenaBookmark     *bookmark,
				   AthenaBookmarkList *bookmarks)
{
	g_assert (ATHENA_IS_BOOKMARK (bookmark));
	g_assert (ATHENA_IS_BOOKMARK_LIST (bookmarks));

	/* save changes to the list */
	athena_bookmark_list_save_file (bookmarks);
}

static void
bookmark_in_list_notify (GObject *object,
			 GParamSpec *pspec,
			 AthenaBookmarkList *bookmarks)
{
	/* emit the changed signal without saving, as only appearance properties changed */
	g_signal_emit (bookmarks, signals[CHANGED], 0);
}

static void
stop_monitoring_bookmark (AthenaBookmarkList *bookmarks,
			  AthenaBookmark     *bookmark)
{
	g_signal_handlers_disconnect_by_func (bookmark,
					      bookmark_in_list_changed_callback,
					      bookmarks);
}

static void
stop_monitoring_one (gpointer data, gpointer user_data)
{
	g_assert (ATHENA_IS_BOOKMARK (data));
	g_assert (ATHENA_IS_BOOKMARK_LIST (user_data));

	stop_monitoring_bookmark (ATHENA_BOOKMARK_LIST (user_data), 
				  ATHENA_BOOKMARK (data));
}

static void
clear (AthenaBookmarkList *bookmarks)
{
	g_list_foreach (bookmarks->list, stop_monitoring_one, bookmarks);
	g_list_free_full (bookmarks->list, g_object_unref);
	bookmarks->list = NULL;
}

static void
do_finalize (GObject *object)
{
	if (ATHENA_BOOKMARK_LIST (object)->monitor != NULL) {
		g_file_monitor_cancel (ATHENA_BOOKMARK_LIST (object)->monitor);
		ATHENA_BOOKMARK_LIST (object)->monitor = NULL;
	}

	g_queue_free (ATHENA_BOOKMARK_LIST (object)->pending_ops);

	clear (ATHENA_BOOKMARK_LIST (object));

	G_OBJECT_CLASS (athena_bookmark_list_parent_class)->finalize (object);
}

static GObject *
do_constructor (GType type,
                guint n_construct_params,
                GObjectConstructParam *construct_params)
{
	GObject *retval;

	if (singleton != NULL) {
		return g_object_ref (singleton);
	}

	retval = G_OBJECT_CLASS (athena_bookmark_list_parent_class)->constructor
		(type, n_construct_params, construct_params);

	singleton = ATHENA_BOOKMARK_LIST (retval);
	g_object_add_weak_pointer (retval, (gpointer) &singleton);

	return retval;
}


static void
athena_bookmark_list_class_init (AthenaBookmarkListClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->finalize = do_finalize;
	object_class->constructor = do_constructor;

	signals[CHANGED] =
		g_signal_new ("changed",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (AthenaBookmarkListClass, 
					       changed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
}

static void
bookmark_monitor_changed_cb (GFileMonitor      *monitor,
			     GFile             *child,
			     GFile             *other_file,
			     GFileMonitorEvent  eflags,
			     gpointer           user_data)
{
	if (eflags == G_FILE_MONITOR_EVENT_CHANGED ||
	    eflags == G_FILE_MONITOR_EVENT_CREATED) {
		g_return_if_fail (ATHENA_IS_BOOKMARK_LIST (ATHENA_BOOKMARK_LIST (user_data)));
		athena_bookmark_list_load_file (ATHENA_BOOKMARK_LIST (user_data));
	}
}

static void
athena_bookmark_list_init (AthenaBookmarkList *bookmarks)
{
	GFile *file;

	bookmarks->pending_ops = g_queue_new ();

	athena_bookmark_list_load_file (bookmarks);

	file = athena_bookmark_list_get_file ();
	bookmarks->monitor = g_file_monitor_file (file, 0, NULL, NULL);
	g_file_monitor_set_rate_limit (bookmarks->monitor, 1000);

	g_signal_connect (bookmarks->monitor, "changed",
			  G_CALLBACK (bookmark_monitor_changed_cb), bookmarks);

	g_object_unref (file);
}

static void
insert_bookmark_internal (AthenaBookmarkList *bookmarks,
			  AthenaBookmark     *bookmark,
			  int                   index)
{
	bookmarks->list = g_list_insert (bookmarks->list, bookmark, index);

	g_signal_connect_object (bookmark, "contents-changed",
				 G_CALLBACK (bookmark_in_list_changed_callback), bookmarks, 0);
	g_signal_connect_object (bookmark, "notify::icon",
				 G_CALLBACK (bookmark_in_list_notify), bookmarks, 0);
	g_signal_connect_object (bookmark, "notify::name",
				 G_CALLBACK (bookmark_in_list_notify), bookmarks, 0);
}

/**
 * athena_bookmark_list_append:
 *
 * Append a bookmark to a bookmark list.
 * @bookmarks: AthenaBookmarkList to append to.
 * @bookmark: Bookmark to append a copy of.
 **/
void
athena_bookmark_list_append (AthenaBookmarkList *bookmarks, 
			       AthenaBookmark     *bookmark)
{
	g_return_if_fail (ATHENA_IS_BOOKMARK_LIST (bookmarks));
	g_return_if_fail (ATHENA_IS_BOOKMARK (bookmark));

	insert_bookmark_internal (bookmarks, 
				  athena_bookmark_copy (bookmark), 
				  -1);

	athena_bookmark_list_save_file (bookmarks);
}

/**
 * athena_bookmark_list_contains:
 *
 * Check whether a bookmark with matching name and url is already in the list.
 * @bookmarks: AthenaBookmarkList to check contents of.
 * @bookmark: AthenaBookmark to match against.
 * 
 * Return value: TRUE if matching bookmark is in list, FALSE otherwise
 **/
gboolean
athena_bookmark_list_contains (AthenaBookmarkList *bookmarks, 
				 AthenaBookmark     *bookmark)
{
	g_return_val_if_fail (ATHENA_IS_BOOKMARK_LIST (bookmarks), FALSE);
	g_return_val_if_fail (ATHENA_IS_BOOKMARK (bookmark), FALSE);

	return g_list_find_custom (bookmarks->list,
				   (gpointer)bookmark, 
				   athena_bookmark_compare_with) 
		!= NULL;
}

/**
 * athena_bookmark_list_delete_item_at:
 * 
 * Delete the bookmark at the specified position.
 * @bookmarks: the list of bookmarks.
 * @index: index, must be less than length of list.
 **/
void
athena_bookmark_list_delete_item_at (AthenaBookmarkList *bookmarks, 
				       guint                 index)
{
	GList *doomed;

	g_return_if_fail (ATHENA_IS_BOOKMARK_LIST (bookmarks));
	g_return_if_fail (index < g_list_length (bookmarks->list));

	doomed = g_list_nth (bookmarks->list, index);
	bookmarks->list = g_list_remove_link (bookmarks->list, doomed);

	g_assert (ATHENA_IS_BOOKMARK (doomed->data));
	stop_monitoring_bookmark (bookmarks, ATHENA_BOOKMARK (doomed->data));
	g_object_unref (doomed->data);

	g_list_free_1 (doomed);

	athena_bookmark_list_save_file (bookmarks);
}

/**
 * athena_bookmark_list_move_item:
 *
 * Move the item from the given position to the destination.
 * @index: the index of the first bookmark.
 * @destination: the index of the second bookmark.
 **/
void
athena_bookmark_list_move_item (AthenaBookmarkList *bookmarks,
				  guint index,
				  guint destination)
{
	GList *bookmark_item;

	if (index == destination) {
		return;
	}

	bookmark_item = g_list_nth (bookmarks->list, index);
	bookmarks->list = g_list_remove_link (bookmarks->list,
					      bookmark_item);

	if (index < destination) {
		bookmarks->list = g_list_insert (bookmarks->list,
						 bookmark_item->data,
						 destination - 1);
	} else {
		bookmarks->list = g_list_insert (bookmarks->list,
						 bookmark_item->data,
						 destination);
	}

	athena_bookmark_list_save_file (bookmarks);
}

/**
 * athena_bookmark_list_delete_items_with_uri:
 * 
 * Delete all bookmarks with the given uri.
 * @bookmarks: the list of bookmarks.
 * @uri: The uri to match.
 **/
void
athena_bookmark_list_delete_items_with_uri (AthenaBookmarkList *bookmarks, 
				      	      const char           *uri)
{
	GList *node, *next;
	gboolean list_changed;
	char *bookmark_uri;

	g_return_if_fail (ATHENA_IS_BOOKMARK_LIST (bookmarks));
	g_return_if_fail (uri != NULL);

	list_changed = FALSE;
	for (node = bookmarks->list; node != NULL;  node = next) {
		next = node->next;

		bookmark_uri = athena_bookmark_get_uri (ATHENA_BOOKMARK (node->data));
		if (g_strcmp0 (bookmark_uri, uri) == 0) {
			bookmarks->list = g_list_remove_link (bookmarks->list, node);
			stop_monitoring_bookmark (bookmarks, ATHENA_BOOKMARK (node->data));
			g_object_unref (node->data);
			g_list_free_1 (node);
			list_changed = TRUE;
		}
		g_free (bookmark_uri);
	}

	if (list_changed) {
		athena_bookmark_list_save_file (bookmarks);
	}
}

/**
 * athena_bookmark_list_get_window_geometry:
 * 
 * Get a string representing the bookmark_list's window's geometry.
 * This is the value set earlier by athena_bookmark_list_set_window_geometry.
 * @bookmarks: the list of bookmarks associated with the window.
 * Return value: string representation of window's geometry, suitable for
 * passing to gnome_parse_geometry(), or NULL if
 * no window geometry has yet been saved for this bookmark list.
 **/
const char *
athena_bookmark_list_get_window_geometry (AthenaBookmarkList *bookmarks)
{
	return window_geometry;
}

/**
 * athena_bookmark_list_insert_item:
 * 
 * Insert a bookmark at a specified position.
 * @bookmarks: the list of bookmarks.
 * @index: the position to insert the bookmark at.
 * @new_bookmark: the bookmark to insert a copy of.
 **/
void
athena_bookmark_list_insert_item (AthenaBookmarkList *bookmarks,
				    AthenaBookmark     *new_bookmark,
				    guint                 index)
{
	g_return_if_fail (ATHENA_IS_BOOKMARK_LIST (bookmarks));
	g_return_if_fail (index <= g_list_length (bookmarks->list));

	insert_bookmark_internal (bookmarks,
				  athena_bookmark_copy (new_bookmark), 
				  index);

	athena_bookmark_list_save_file (bookmarks);
}

/**
 * athena_bookmark_list_item_at:
 * 
 * Get the bookmark at the specified position.
 * @bookmarks: the list of bookmarks.
 * @index: index, must be less than length of list.
 * 
 * Return value: the bookmark at position @index in @bookmarks.
 **/
AthenaBookmark *
athena_bookmark_list_item_at (AthenaBookmarkList *bookmarks, guint index)
{
	g_return_val_if_fail (ATHENA_IS_BOOKMARK_LIST (bookmarks), NULL);
	g_return_val_if_fail (index < g_list_length (bookmarks->list), NULL);

	return ATHENA_BOOKMARK (g_list_nth_data (bookmarks->list, index));
}

/**
 * athena_bookmark_list_length:
 * 
 * Get the number of bookmarks in the list.
 * @bookmarks: the list of bookmarks.
 * 
 * Return value: the length of the bookmark list.
 **/
guint
athena_bookmark_list_length (AthenaBookmarkList *bookmarks)
{
	g_return_val_if_fail (ATHENA_IS_BOOKMARK_LIST(bookmarks), 0);

	return g_list_length (bookmarks->list);
}

static void
load_file_finish (AthenaBookmarkList *bookmarks,
		  GObject *source,
		  GAsyncResult *res)
{
	GError *error = NULL;
	gchar *contents = NULL;

	g_file_load_contents_finish (G_FILE (source),
				     res, &contents, NULL, NULL, &error);

	if (error == NULL) {
        	char **lines;
      		int i;

		lines = g_strsplit (contents, "\n", -1);
      	 	for (i = 0; lines[i]; i++) {
			/* Ignore empty or invalid lines that cannot be parsed properly */
	  		if (lines[i][0] != '\0' && lines[i][0] != ' ') {
				/* gtk 2.7/2.8 might have labels appended to bookmarks which are separated by a space */
				/* we must seperate the bookmark uri and the potential label */
 				char *space, *label;

				label = NULL;
      				space = strchr (lines[i], ' ');
      				if (space) {
					*space = '\0';
					label = g_strdup (space + 1);
				}
				insert_bookmark_internal (bookmarks, 
						          new_bookmark_from_uri (lines[i], label), 
						          -1);

				g_free (label);
			}
		}
      		g_free (contents);
       		g_strfreev (lines);

		g_signal_emit (bookmarks, signals[CHANGED], 0);
	} else if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND)) {
		g_warning ("Could not load bookmark file: %s\n", error->message);
		g_error_free (error);
	}
}

static void
load_file_async (AthenaBookmarkList *self,
		 GAsyncReadyCallback callback)
{
	GFile *file;

	file = athena_bookmark_list_get_file ();

	/* Wipe out old list. */
	clear (self);

	/* keep the bookmark list alive */
	g_object_ref (self);
	g_file_load_contents_async (file, NULL, callback, self);

	g_object_unref (file);
}

static void
save_file_finish (AthenaBookmarkList *bookmarks,
		  GObject *source,
		  GAsyncResult *res)
{
	GError *error = NULL;
	GFile *file;

	g_file_replace_contents_finish (G_FILE (source),
					res, NULL, &error);

	if (error != NULL) {
		g_warning ("Unable to replace contents of the bookmarks file: %s",
			   error->message);
		g_error_free (error);
	}

	file = athena_bookmark_list_get_file ();

	/* re-enable bookmark file monitoring */
	bookmarks->monitor = g_file_monitor_file (file, 0, NULL, NULL);
	g_file_monitor_set_rate_limit (bookmarks->monitor, 1000);
	g_signal_connect (bookmarks->monitor, "changed",
			  G_CALLBACK (bookmark_monitor_changed_cb), bookmarks);

	g_object_unref (file);
}

static void
save_file_async (AthenaBookmarkList *bookmarks,
		 GAsyncReadyCallback callback)
{
	GFile *file;
	GList *l;
	GString *bookmark_string;

	/* temporarily disable bookmark file monitoring when writing file */
	if (bookmarks->monitor != NULL) {
		g_file_monitor_cancel (bookmarks->monitor);
		bookmarks->monitor = NULL;
	}

	file = athena_bookmark_list_get_file ();
	bookmark_string = g_string_new (NULL);

	for (l = bookmarks->list; l; l = l->next) {
		AthenaBookmark *bookmark;

		bookmark = ATHENA_BOOKMARK (l->data);

		/* make sure we save label if it has one for compatibility with GTK 2.7 and 2.8 */
		if (athena_bookmark_get_has_custom_name (bookmark)) {
			const char *label;
			char *uri;
			label = athena_bookmark_get_name (bookmark);
			uri = athena_bookmark_get_uri (bookmark);
			g_string_append_printf (bookmark_string,
						"%s %s\n", uri, label);
			g_free (uri);
		} else {
			char *uri;
			uri = athena_bookmark_get_uri (bookmark);
			g_string_append_printf (bookmark_string, "%s\n", uri);
			g_free (uri);
		}
	}

	/* keep the bookmark list alive */
	g_object_ref (bookmarks);
	g_file_replace_contents_async (file, bookmark_string->str,
				       bookmark_string->len, NULL,
				       FALSE, 0, NULL, callback,
				       bookmarks);

	g_object_unref (file);
}

static void
process_next_op (AthenaBookmarkList *bookmarks);

static void
op_processed_cb (GObject *source,
		 GAsyncResult *res,
		 gpointer user_data)
{
	AthenaBookmarkList *self = user_data;
	int op;

	op = GPOINTER_TO_INT (g_queue_pop_tail (self->pending_ops));

	if (op == LOAD_JOB) {
		load_file_finish (self, source, res);
	} else {
		save_file_finish (self, source, res);
	}

	if (!g_queue_is_empty (self->pending_ops)) {
		process_next_op (self);
	}

	/* release the reference acquired during the _async method */
	g_object_unref (self);
}

static void
process_next_op (AthenaBookmarkList *bookmarks)
{
	gint op;

	op = GPOINTER_TO_INT (g_queue_peek_tail (bookmarks->pending_ops));

	if (op == LOAD_JOB) {
		load_file_async (bookmarks, op_processed_cb);
	} else {
		save_file_async (bookmarks, op_processed_cb);
	}
}

/**
 * athena_bookmark_list_load_file:
 * 
 * Reads bookmarks from file, clobbering contents in memory.
 * @bookmarks: the list of bookmarks to fill with file contents.
 **/
static void
athena_bookmark_list_load_file (AthenaBookmarkList *bookmarks)
{
	g_queue_push_head (bookmarks->pending_ops, GINT_TO_POINTER (LOAD_JOB));

	if (g_queue_get_length (bookmarks->pending_ops) == 1) {
		process_next_op (bookmarks);
	}
}

/**
 * athena_bookmark_list_save_file:
 * 
 * Save bookmarks to disk.
 * @bookmarks: the list of bookmarks to save.
 **/
static void
athena_bookmark_list_save_file (AthenaBookmarkList *bookmarks)
{
	g_signal_emit (bookmarks, signals[CHANGED], 0);

	g_queue_push_head (bookmarks->pending_ops, GINT_TO_POINTER (SAVE_JOB));

	if (g_queue_get_length (bookmarks->pending_ops) == 1) {
		process_next_op (bookmarks);
	}
}

/**
 * athena_bookmark_list_new:
 * 
 * Create a new bookmark_list, with contents read from disk.
 * 
 * Return value: A pointer to the new widget.
 **/
AthenaBookmarkList *
athena_bookmark_list_new (void)
{
	AthenaBookmarkList *list;

	list = ATHENA_BOOKMARK_LIST (g_object_new (ATHENA_TYPE_BOOKMARK_LIST, NULL));

	return list;
}

/**
 * athena_bookmark_list_set_window_geometry:
 * 
 * Set a bookmarks window's geometry (position & size), in string form. This is
 * stored to disk by this class, and can be retrieved later in
 * the same session or in a future session.
 * @bookmarks: the list of bookmarks associated with the window.
 * @geometry: the new window geometry string.
 **/
void
athena_bookmark_list_set_window_geometry (AthenaBookmarkList *bookmarks,
					    const char           *geometry)
{
	g_return_if_fail (ATHENA_IS_BOOKMARK_LIST (bookmarks));
	g_return_if_fail (geometry != NULL);

	g_free (window_geometry);
	window_geometry = g_strdup (geometry);

	athena_bookmark_list_save_file (bookmarks);
}

