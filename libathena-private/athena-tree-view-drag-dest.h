/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Athena
 *
 * Copyright (C) 2002 Sun Microsystems, Inc.
 *
 * Athena is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Athena is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 * Author: Dave Camp <dave@ximian.com>
 */

/* athena-tree-view-drag-dest.h: Handles drag and drop for treeviews which 
 *                                 contain a hierarchy of files
 */

#ifndef ATHENA_TREE_VIEW_DRAG_DEST_H
#define ATHENA_TREE_VIEW_DRAG_DEST_H

#include <gtk/gtk.h>

#include "athena-file.h"

G_BEGIN_DECLS

#define ATHENA_TYPE_TREE_VIEW_DRAG_DEST	(athena_tree_view_drag_dest_get_type ())
#define ATHENA_TREE_VIEW_DRAG_DEST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_TREE_VIEW_DRAG_DEST, AthenaTreeViewDragDest))
#define ATHENA_TREE_VIEW_DRAG_DEST_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_TREE_VIEW_DRAG_DEST, AthenaTreeViewDragDestClass))
#define ATHENA_IS_TREE_VIEW_DRAG_DEST(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_TREE_VIEW_DRAG_DEST))
#define ATHENA_IS_TREE_VIEW_DRAG_DEST_CLASS(klass)	(G_TYPE_CLASS_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_TREE_VIEW_DRAG_DEST))

typedef struct _AthenaTreeViewDragDest        AthenaTreeViewDragDest;
typedef struct _AthenaTreeViewDragDestClass   AthenaTreeViewDragDestClass;
typedef struct _AthenaTreeViewDragDestDetails AthenaTreeViewDragDestDetails;

struct _AthenaTreeViewDragDest {
	GObject parent;
	
	AthenaTreeViewDragDestDetails *details;
};

struct _AthenaTreeViewDragDestClass {
	GObjectClass parent;
	
	char *(*get_root_uri) (AthenaTreeViewDragDest *dest);
	AthenaFile *(*get_file_for_path) (AthenaTreeViewDragDest *dest,
					    GtkTreePath *path);
	void (*move_copy_items) (AthenaTreeViewDragDest *dest,
				 const GList *item_uris,
				 const char *target_uri,
				 GdkDragAction action,
				 int x,
				 int y);
	void (* handle_netscape_url) (AthenaTreeViewDragDest *dest,
				 const char *url,
				 const char *target_uri,
				 GdkDragAction action,
				 int x,
				 int y);
	void (* handle_uri_list) (AthenaTreeViewDragDest *dest,
				  const char *uri_list,
				  const char *target_uri,
				  GdkDragAction action,
				  int x,
				  int y);
	void (* handle_text)    (AthenaTreeViewDragDest *dest,
				  const char *text,
				  const char *target_uri,
				  GdkDragAction action,
				  int x,
				  int y);
	void (* handle_raw)    (AthenaTreeViewDragDest *dest,
				  char *raw_data,
				  int length,
				  const char *target_uri,
				  const char *direct_save_uri,
				  GdkDragAction action,
				  int x,
				  int y);
};

GType                     athena_tree_view_drag_dest_get_type (void);
AthenaTreeViewDragDest *athena_tree_view_drag_dest_new      (GtkTreeView *tree_view);

G_END_DECLS

#endif
