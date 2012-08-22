/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* 
   athena-trash-monitor.h: Athena trash state watcher.
 
   Copyright (C) 2000 Eazel, Inc.
  
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
  
   Author: Pavel Cisler <pavel@eazel.com>
*/

#ifndef ATHENA_TRASH_MONITOR_H
#define ATHENA_TRASH_MONITOR_H

#include <gtk/gtk.h>
#include <gio/gio.h>

typedef struct AthenaTrashMonitor AthenaTrashMonitor;
typedef struct AthenaTrashMonitorClass AthenaTrashMonitorClass;
typedef struct AthenaTrashMonitorDetails AthenaTrashMonitorDetails;

#define ATHENA_TYPE_TRASH_MONITOR athena_trash_monitor_get_type()
#define ATHENA_TRASH_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_TRASH_MONITOR, AthenaTrashMonitor))
#define ATHENA_TRASH_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_TRASH_MONITOR, AthenaTrashMonitorClass))
#define ATHENA_IS_TRASH_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_TRASH_MONITOR))
#define ATHENA_IS_TRASH_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_TRASH_MONITOR))
#define ATHENA_TRASH_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_TRASH_MONITOR, AthenaTrashMonitorClass))

struct AthenaTrashMonitor {
	GObject object;
	AthenaTrashMonitorDetails *details;
};

struct AthenaTrashMonitorClass {
	GObjectClass parent_class;

	void (* trash_state_changed)		(AthenaTrashMonitor 	*trash_monitor,
				      		 gboolean 		 new_state);
};

GType			athena_trash_monitor_get_type				(void);

AthenaTrashMonitor   *athena_trash_monitor_get 				(void);
gboolean		athena_trash_monitor_is_empty 			(void);
GIcon                  *athena_trash_monitor_get_icon                         (void);

void		        athena_trash_monitor_add_new_trash_directories        (void);

#endif
