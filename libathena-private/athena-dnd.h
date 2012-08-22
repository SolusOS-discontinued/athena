/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-dnd.h - Common Drag & drop handling code shared by the icon container
   and the list view.

   Copyright (C) 2000 Eazel, Inc.

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

   Authors: Pavel Cisler <pavel@eazel.com>,
	    Ettore Perazzoli <ettore@gnu.org>
*/

#ifndef ATHENA_DND_H
#define ATHENA_DND_H

#include <gtk/gtk.h>

/* Drag & Drop target names. */
#define ATHENA_ICON_DND_GNOME_ICON_LIST_TYPE	"x-special/gnome-icon-list"
#define ATHENA_ICON_DND_URI_LIST_TYPE		"text/uri-list"
#define ATHENA_ICON_DND_NETSCAPE_URL_TYPE	"_NETSCAPE_URL"
#define ATHENA_ICON_DND_BGIMAGE_TYPE		"property/bgimage"
#define ATHENA_ICON_DND_ROOTWINDOW_DROP_TYPE	"application/x-rootwindow-drop"
#define ATHENA_ICON_DND_XDNDDIRECTSAVE_TYPE	"XdndDirectSave0" /* XDS Protocol Type */
#define ATHENA_ICON_DND_RAW_TYPE	"application/octet-stream"

/* Item of the drag selection list */
typedef struct {
	char *uri;
	gboolean got_icon_position;
	int icon_x, icon_y;
	int icon_width, icon_height;
} AthenaDragSelectionItem;

/* Standard Drag & Drop types. */
typedef enum {
	ATHENA_ICON_DND_GNOME_ICON_LIST,
	ATHENA_ICON_DND_URI_LIST,
	ATHENA_ICON_DND_NETSCAPE_URL,
	ATHENA_ICON_DND_TEXT,
	ATHENA_ICON_DND_XDNDDIRECTSAVE,
	ATHENA_ICON_DND_RAW,
	ATHENA_ICON_DND_ROOTWINDOW_DROP
} AthenaIconDndTargetType;

typedef enum {
	ATHENA_DND_ACTION_FIRST = GDK_ACTION_ASK << 1,
	ATHENA_DND_ACTION_SET_AS_BACKGROUND = ATHENA_DND_ACTION_FIRST << 0,
	ATHENA_DND_ACTION_SET_AS_FOLDER_BACKGROUND = ATHENA_DND_ACTION_FIRST << 1,
	ATHENA_DND_ACTION_SET_AS_GLOBAL_BACKGROUND = ATHENA_DND_ACTION_FIRST << 2
} AthenaDndAction;

/* drag&drop-related information. */
typedef struct {
	GtkTargetList *target_list;

	/* Stuff saved at "receive data" time needed later in the drag. */
	gboolean got_drop_data_type;
	AthenaIconDndTargetType data_type;
	GtkSelectionData *selection_data;
	char *direct_save_uri;

	/* Start of the drag, in window coordinates. */
	int start_x, start_y;

	/* List of AthenaDragSelectionItems, representing items being dragged, or NULL
	 * if data about them has not been received from the source yet.
	 */
	GList *selection_list;

	/* has the drop occured ? */
	gboolean drop_occured;

	/* whether or not need to clean up the previous dnd data */
	gboolean need_to_destroy;

	/* autoscrolling during dragging */
	int auto_scroll_timeout_id;
	gboolean waiting_to_autoscroll;
	gint64 start_auto_scroll_in;

} AthenaDragInfo;

typedef void		(* AthenaDragEachSelectedItemDataGet)	(const char *url, 
								 int x, int y, int w, int h, 
								 gpointer data);
typedef void		(* AthenaDragEachSelectedItemIterator)	(AthenaDragEachSelectedItemDataGet iteratee, 
								 gpointer iterator_context, 
								 gpointer data);

void			    athena_drag_init				(AthenaDragInfo		      *drag_info,
									 const GtkTargetEntry		      *drag_types,
									 int				       drag_type_count,
									 gboolean			       add_text_targets);
void			    athena_drag_finalize			(AthenaDragInfo		      *drag_info);
AthenaDragSelectionItem  *athena_drag_selection_item_new		(void);
void			    athena_drag_destroy_selection_list	(GList				      *selection_list);
GList			   *athena_drag_build_selection_list		(GtkSelectionData		      *data);

char **			    athena_drag_uri_array_from_selection_list (const GList			      *selection_list);
GList *			    athena_drag_uri_list_from_selection_list	(const GList			      *selection_list);

char **			    athena_drag_uri_array_from_list		(const GList			      *uri_list);
GList *			    athena_drag_uri_list_from_array		(const char			     **uris);

gboolean		    athena_drag_items_local			(const char			      *target_uri,
									 const GList			      *selection_list);
gboolean		    athena_drag_uris_local			(const char			      *target_uri,
									 const GList			      *source_uri_list);
gboolean		    athena_drag_items_in_trash		(const GList			      *selection_list);
gboolean		    athena_drag_items_on_desktop		(const GList			      *selection_list);
void			    athena_drag_default_drop_action_for_icons (GdkDragContext			      *context,
									 const char			      *target_uri,
									 const GList			      *items,
									 int				      *action);
GdkDragAction		    athena_drag_default_drop_action_for_netscape_url (GdkDragContext			     *context);
GdkDragAction		    athena_drag_default_drop_action_for_uri_list     (GdkDragContext			     *context,
										const char			     *target_uri_string);
gboolean		    athena_drag_drag_data_get			(GtkWidget			      *widget,
									 GdkDragContext			      *context,
									 GtkSelectionData		      *selection_data,
									 guint				       info,
									 guint32			       time,
									 gpointer			       container_context,
									 AthenaDragEachSelectedItemIterator  each_selected_item_iterator);
int			    athena_drag_modifier_based_action		(int				       default_action,
									 int				       non_default_action);

GdkDragAction		    athena_drag_drop_action_ask		(GtkWidget			      *widget,
									 GdkDragAction			       possible_actions);

gboolean		    athena_drag_autoscroll_in_scroll_region	(GtkWidget			      *widget);
void			    athena_drag_autoscroll_calculate_delta	(GtkWidget			      *widget,
									 float				      *x_scroll_delta,
									 float				      *y_scroll_delta);
void			    athena_drag_autoscroll_start		(AthenaDragInfo		      *drag_info,
									 GtkWidget			      *widget,
									 GSourceFunc			       callback,
									 gpointer			       user_data);
void			    athena_drag_autoscroll_stop		(AthenaDragInfo		      *drag_info);

gboolean		    athena_drag_selection_includes_special_link (GList			      *selection_list);

#endif
