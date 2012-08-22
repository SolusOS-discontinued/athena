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

/* athena-bookmark-list.h - interface for centralized list of bookmarks.
 */

#ifndef ATHENA_BOOKMARK_LIST_H
#define ATHENA_BOOKMARK_LIST_H

#include <libathena-private/athena-bookmark.h>
#include <gio/gio.h>

typedef struct AthenaBookmarkList AthenaBookmarkList;
typedef struct AthenaBookmarkListClass AthenaBookmarkListClass;

#define ATHENA_TYPE_BOOKMARK_LIST athena_bookmark_list_get_type()
#define ATHENA_BOOKMARK_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_BOOKMARK_LIST, AthenaBookmarkList))
#define ATHENA_BOOKMARK_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_BOOKMARK_LIST, AthenaBookmarkListClass))
#define ATHENA_IS_BOOKMARK_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_BOOKMARK_LIST))
#define ATHENA_IS_BOOKMARK_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_BOOKMARK_LIST))
#define ATHENA_BOOKMARK_LIST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_BOOKMARK_LIST, AthenaBookmarkListClass))

struct AthenaBookmarkList {
	GObject object;

	GList *list; 
	GFileMonitor *monitor;
	GQueue *pending_ops;
};

struct AthenaBookmarkListClass {
	GObjectClass parent_class;
	void (* changed) (AthenaBookmarkList *bookmarks);
};

GType                   athena_bookmark_list_get_type            (void);
AthenaBookmarkList *  athena_bookmark_list_new                 (void);
void                    athena_bookmark_list_append              (AthenaBookmarkList   *bookmarks,
								    AthenaBookmark *bookmark);
gboolean                athena_bookmark_list_contains            (AthenaBookmarkList   *bookmarks,
								    AthenaBookmark *bookmark);
void                    athena_bookmark_list_delete_item_at      (AthenaBookmarkList   *bookmarks,
								    guint                   index);
void                    athena_bookmark_list_delete_items_with_uri (AthenaBookmarkList *bookmarks,
								    const char		   *uri);
void                    athena_bookmark_list_insert_item         (AthenaBookmarkList   *bookmarks,
								    AthenaBookmark *bookmark,
								    guint                   index);
guint                   athena_bookmark_list_length              (AthenaBookmarkList   *bookmarks);
AthenaBookmark *      athena_bookmark_list_item_at             (AthenaBookmarkList   *bookmarks,
								    guint                   index);
void                    athena_bookmark_list_move_item           (AthenaBookmarkList *bookmarks,
								    guint                 index,
								    guint                 destination);
void                    athena_bookmark_list_set_window_geometry (AthenaBookmarkList   *bookmarks,
								    const char             *geometry);
const char *            athena_bookmark_list_get_window_geometry (AthenaBookmarkList   *bookmarks);

#endif /* ATHENA_BOOKMARK_LIST_H */
