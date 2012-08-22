/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-window-slot.h: Athena window slot
 
   Copyright (C) 2008 Free Software Foundation, Inc.
  
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
  
   Author: Christian Neumair <cneumair@gnome.org>
*/

#ifndef ATHENA_WINDOW_SLOT_H
#define ATHENA_WINDOW_SLOT_H

#include "athena-view.h"
#include "athena-window-types.h"
#include "athena-query-editor.h"

#define ATHENA_TYPE_WINDOW_SLOT	 (athena_window_slot_get_type())
#define ATHENA_WINDOW_SLOT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_WINDOW_SLOT, AthenaWindowSlotClass))
#define ATHENA_WINDOW_SLOT(obj)	 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_WINDOW_SLOT, AthenaWindowSlot))
#define ATHENA_IS_WINDOW_SLOT(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_WINDOW_SLOT))
#define ATHENA_IS_WINDOW_SLOT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_WINDOW_SLOT))
#define ATHENA_WINDOW_SLOT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_WINDOW_SLOT, AthenaWindowSlotClass))

typedef enum {
	ATHENA_LOCATION_CHANGE_STANDARD,
	ATHENA_LOCATION_CHANGE_BACK,
	ATHENA_LOCATION_CHANGE_FORWARD,
	ATHENA_LOCATION_CHANGE_RELOAD
} AthenaLocationChangeType;

struct AthenaWindowSlotClass {
	GtkBoxClass parent_class;

	/* wrapped AthenaWindowInfo signals, for overloading */
	void (* active)   (AthenaWindowSlot *slot);
	void (* inactive) (AthenaWindowSlot *slot);
};

/* Each AthenaWindowSlot corresponds to
 * a location in the window for displaying
 * a AthenaView.
 *
 * For navigation windows, this would be a
 * tab, while spatial windows only have one slot.
 */
struct AthenaWindowSlot {
	GtkBox parent;

	AthenaWindowPane *pane;

	/* slot contains
 	 *  1) an event box containing extra_location_widgets
 	 *  2) the view box for the content view
 	 */
	GtkWidget *extra_location_widgets;

	GtkWidget *view_overlay;
	GtkWidget *floating_bar;

	guint set_status_timeout_id;
	guint loading_timeout_id;

	AthenaView *content_view;
	AthenaView *new_content_view;

	/* Information about bookmarks */
	AthenaBookmark *current_location_bookmark;
	AthenaBookmark *last_location_bookmark;

	/* Current location. */
	GFile *location;
	char *title;
	char *status_text;

	AthenaFile *viewed_file;
	gboolean viewed_file_seen;
	gboolean viewed_file_in_trash;

	gboolean allow_stop;

	AthenaQueryEditor *query_editor;

	/* New location. */
	AthenaLocationChangeType location_change_type;
	guint location_change_distance;
	GFile *pending_location;
	char *pending_scroll_to;
	GList *pending_selection;
	AthenaFile *determine_view_file;
	GCancellable *mount_cancellable;
	GError *mount_error;
	gboolean tried_mount;
	AthenaWindowGoToCallback open_callback;
	gpointer open_callback_user_data;

	GCancellable *find_mount_cancellable;

	gboolean visible;

	/* Back/Forward chain, and history list. 
	 * The data in these lists are AthenaBookmark pointers. 
	 */
	GList *back_list, *forward_list;
};

GType   athena_window_slot_get_type (void);

AthenaWindowSlot * athena_window_slot_new (AthenaWindowPane *pane);

void    athena_window_slot_update_title		   (AthenaWindowSlot *slot);
void    athena_window_slot_update_icon		   (AthenaWindowSlot *slot);
void    athena_window_slot_update_query_editor	   (AthenaWindowSlot *slot);

GFile * athena_window_slot_get_location		   (AthenaWindowSlot *slot);
char *  athena_window_slot_get_location_uri		   (AthenaWindowSlot *slot);

void    athena_window_slot_reload			   (AthenaWindowSlot *slot);

void athena_window_slot_open_location_full (AthenaWindowSlot *slot,
					      GFile *location,
					      AthenaWindowOpenFlags flags,
					      GList *new_selection, /* AthenaFile list */
					      AthenaWindowGoToCallback callback,
					      gpointer user_data);

/* convenience wrapper without callback/user_data */
#define athena_window_slot_open_location(slot, location, flags, new_selection)\
	athena_window_slot_open_location_full(slot, location, flags, new_selection, NULL, NULL)

/* these are wrappers that always open according to current mode */
#define athena_window_slot_go_to(slot, location, new_tab) \
	athena_window_slot_open_location(slot, location, \
					   (new_tab ? ATHENA_WINDOW_OPEN_FLAG_NEW_TAB : 0), \
					   NULL)
#define athena_window_slot_go_to_full(slot, location, new_tab, callback, user_data) \
	athena_window_slot_open_location_full(slot, location, \
						(new_tab ? ATHENA_WINDOW_OPEN_FLAG_NEW_TAB : 0), \
						NULL, callback, user_data)

void			athena_window_slot_stop_loading	      (AthenaWindowSlot	*slot);

void			athena_window_slot_set_content_view	      (AthenaWindowSlot	*slot,
								       const char		*id);
const char	       *athena_window_slot_get_content_view_id      (AthenaWindowSlot	*slot);
gboolean		athena_window_slot_content_view_matches_iid (AthenaWindowSlot	*slot,
								       const char		*iid);

void    athena_window_slot_go_home			   (AthenaWindowSlot *slot,
							    gboolean            new_tab);
void    athena_window_slot_go_up                         (AthenaWindowSlot *slot,
							    gboolean close_behind,
							    gboolean new_tab);

void    athena_window_slot_set_content_view_widget	   (AthenaWindowSlot *slot,
							    AthenaView       *content_view);
void    athena_window_slot_set_viewed_file		   (AthenaWindowSlot *slot,
							    AthenaFile      *file);
void    athena_window_slot_set_allow_stop		   (AthenaWindowSlot *slot,
							    gboolean	    allow_stop);
void    athena_window_slot_set_status			   (AthenaWindowSlot *slot,
							    const char	 *status,
							    const char   *short_status);

void    athena_window_slot_add_extra_location_widget     (AthenaWindowSlot *slot,
							    GtkWidget       *widget);
void    athena_window_slot_remove_extra_location_widgets (AthenaWindowSlot *slot);

AthenaView * athena_window_slot_get_current_view     (AthenaWindowSlot *slot);
char           * athena_window_slot_get_current_uri      (AthenaWindowSlot *slot);
AthenaWindow * athena_window_slot_get_window           (AthenaWindowSlot *slot);
void           athena_window_slot_make_hosting_pane_active (AthenaWindowSlot *slot);

gboolean athena_window_slot_should_close_with_mount (AthenaWindowSlot *slot,
						       GMount *mount);

void athena_window_slot_clear_forward_list (AthenaWindowSlot *slot);
void athena_window_slot_clear_back_list    (AthenaWindowSlot *slot);

#endif /* ATHENA_WINDOW_SLOT_H */
