/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-bookmark.h - implementation of individual bookmarks.
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
 * Copyright (C) 2011, Red Hat, Inc.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors: John Sullivan <sullivan@eazel.com>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 */

#ifndef ATHENA_BOOKMARK_H
#define ATHENA_BOOKMARK_H

#include <gtk/gtk.h>
#include <gio/gio.h>
typedef struct AthenaBookmark AthenaBookmark;

#define ATHENA_TYPE_BOOKMARK athena_bookmark_get_type()
#define ATHENA_BOOKMARK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_BOOKMARK, AthenaBookmark))
#define ATHENA_BOOKMARK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_BOOKMARK, AthenaBookmarkClass))
#define ATHENA_IS_BOOKMARK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_BOOKMARK))
#define ATHENA_IS_BOOKMARK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_BOOKMARK))
#define ATHENA_BOOKMARK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_BOOKMARK, AthenaBookmarkClass))

typedef struct AthenaBookmarkDetails AthenaBookmarkDetails;

struct AthenaBookmark {
	GObject object;
	AthenaBookmarkDetails *details;	
};

struct AthenaBookmarkClass {
	GObjectClass parent_class;

	/* Signals that clients can connect to. */

	/* The contents-changed signal is emitted when the bookmark's contents
	 * (custom name or URI) changed.
	 */
	void	(* contents_changed) (AthenaBookmark *bookmark);
};

typedef struct AthenaBookmarkClass AthenaBookmarkClass;

GType                 athena_bookmark_get_type               (void);
AthenaBookmark *    athena_bookmark_new                    (GFile *location,
                                                                const char *custom_name,
                                                                GIcon *icon);
AthenaBookmark *    athena_bookmark_copy                   (AthenaBookmark      *bookmark);
const char *          athena_bookmark_get_name               (AthenaBookmark      *bookmark);
GFile *               athena_bookmark_get_location           (AthenaBookmark      *bookmark);
char *                athena_bookmark_get_uri                (AthenaBookmark      *bookmark);
GIcon *               athena_bookmark_get_icon               (AthenaBookmark      *bookmark);
gboolean	      athena_bookmark_get_has_custom_name    (AthenaBookmark      *bookmark);		
void                  athena_bookmark_set_custom_name        (AthenaBookmark      *bookmark,
								const char            *new_name);		
gboolean              athena_bookmark_uri_known_not_to_exist (AthenaBookmark      *bookmark);
int                   athena_bookmark_compare_with           (gconstpointer          a,
								gconstpointer          b);
int                   athena_bookmark_compare_uris           (gconstpointer          a,
								gconstpointer          b);

void                  athena_bookmark_set_scroll_pos         (AthenaBookmark      *bookmark,
								const char            *uri);
char *                athena_bookmark_get_scroll_pos         (AthenaBookmark      *bookmark);


/* Helper functions for displaying bookmarks */
GtkWidget *           athena_bookmark_menu_item_new          (AthenaBookmark      *bookmark);

#endif /* ATHENA_BOOKMARK_H */
