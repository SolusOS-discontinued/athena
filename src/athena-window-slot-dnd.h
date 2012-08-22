/*
 * athena-window-slot-dnd.c - Handle DnD for widgets acting as
 * AthenaWindowSlot proxies
 *
 * Copyright (C) 2000, 2001 Eazel, Inc.
 * Copyright (C) 2010, Red Hat, Inc.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
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
 * Authors: Pavel Cisler <pavel@eazel.com>,
 * 	    Ettore Perazzoli <ettore@gnu.org>
 */

#ifndef __ATHENA_WINDOW_SLOT_DND_H__
#define __ATHENA_WINDOW_SLOT_DND_H__

#include <gio/gio.h>
#include <gtk/gtk.h>

#include <libathena-private/athena-dnd.h>

#include "athena-window-slot.h"

void athena_drag_slot_proxy_init (GtkWidget *widget,
                                    AthenaFile *target_file,
                                    AthenaWindowSlot *target_slot);

#endif /* __ATHENA_WINDOW_SLOT_DND_H__ */
