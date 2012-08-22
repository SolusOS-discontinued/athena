/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-window-pane.h: Athena window pane

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

   Author: Holger Berndt <berndth@gmx.de>
*/

#ifndef ATHENA_WINDOW_PANE_H
#define ATHENA_WINDOW_PANE_H

#include <glib-object.h>

#include "athena-window.h"

#include <libathena-private/athena-icon-info.h>

#define ATHENA_TYPE_WINDOW_PANE	 (athena_window_pane_get_type())
#define ATHENA_WINDOW_PANE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_WINDOW_PANE, AthenaWindowPaneClass))
#define ATHENA_WINDOW_PANE(obj)	 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_WINDOW_PANE, AthenaWindowPane))
#define ATHENA_IS_WINDOW_PANE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_WINDOW_PANE))
#define ATHENA_IS_WINDOW_PANE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_WINDOW_PANE))
#define ATHENA_WINDOW_PANE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_WINDOW_PANE, AthenaWindowPaneClass))

struct _AthenaWindowPaneClass {
	GtkBoxClass parent_class;
};

/* A AthenaWindowPane is a layer between a slot and a window.
 * Each slot is contained in one pane, and each pane can contain
 * one or more slots. It also supports the notion of an "active slot".
 * On the other hand, each pane is contained in a window, while each
 * window can contain one or multiple panes. Likewise, the window has
 * the notion of an "active pane".
 *
 * A navigation window may have one or more panes.
 */
struct _AthenaWindowPane {
	GtkBox parent;

	/* hosting window */
	AthenaWindow *window;

	/* available slots, and active slot.
	 * Both of them may never be NULL. */
	GList *slots;
	AthenaWindowSlot *active_slot;

	/* location bar */
	GtkWidget *location_bar;
	GtkWidget *path_bar;
	GtkWidget *search_bar;
	GtkWidget *tool_bar;

	gboolean temporary_navigation_bar;
	gboolean temporary_search_bar;

	/* notebook */
	GtkWidget *notebook;

	GtkActionGroup *action_group;

	GtkWidget *last_focus_widget;
};

GType athena_window_pane_get_type (void);

AthenaWindowPane *athena_window_pane_new (AthenaWindow *window);

AthenaWindowSlot *athena_window_pane_open_slot  (AthenaWindowPane *pane,
						     AthenaWindowOpenSlotFlags flags);
void                athena_window_pane_close_slot (AthenaWindowPane *pane,
						     AthenaWindowSlot *slot);

void athena_window_pane_sync_location_widgets (AthenaWindowPane *pane);
void athena_window_pane_sync_search_widgets  (AthenaWindowPane *pane);
void athena_window_pane_set_active (AthenaWindowPane *pane, gboolean is_active);
void athena_window_pane_slot_close (AthenaWindowPane *pane, AthenaWindowSlot *slot);

void athena_window_pane_grab_focus (AthenaWindowPane *pane);

/* bars */
void     athena_window_pane_ensure_location_bar (AthenaWindowPane *pane);

#endif /* ATHENA_WINDOW_PANE_H */
