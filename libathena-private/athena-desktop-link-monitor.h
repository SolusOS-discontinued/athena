/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-desktop-link-monitor.h: singleton that manages the desktop links
    
   Copyright (C) 2003 Red Hat, Inc.
  
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
  
   Author: Alexander Larsson <alexl@redhat.com>
*/

#ifndef ATHENA_DESKTOP_LINK_MONITOR_H
#define ATHENA_DESKTOP_LINK_MONITOR_H

#include <gtk/gtk.h>
#include <libathena-private/athena-desktop-link.h>

#define ATHENA_TYPE_DESKTOP_LINK_MONITOR athena_desktop_link_monitor_get_type()
#define ATHENA_DESKTOP_LINK_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_DESKTOP_LINK_MONITOR, AthenaDesktopLinkMonitor))
#define ATHENA_DESKTOP_LINK_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_DESKTOP_LINK_MONITOR, AthenaDesktopLinkMonitorClass))
#define ATHENA_IS_DESKTOP_LINK_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_DESKTOP_LINK_MONITOR))
#define ATHENA_IS_DESKTOP_LINK_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_DESKTOP_LINK_MONITOR))
#define ATHENA_DESKTOP_LINK_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_DESKTOP_LINK_MONITOR, AthenaDesktopLinkMonitorClass))

typedef struct AthenaDesktopLinkMonitorDetails AthenaDesktopLinkMonitorDetails;

typedef struct {
	GObject parent_slot;
	AthenaDesktopLinkMonitorDetails *details;
} AthenaDesktopLinkMonitor;

typedef struct {
	GObjectClass parent_slot;
} AthenaDesktopLinkMonitorClass;

GType   athena_desktop_link_monitor_get_type (void);

AthenaDesktopLinkMonitor *   athena_desktop_link_monitor_get (void);
void athena_desktop_link_monitor_delete_link (AthenaDesktopLinkMonitor *monitor,
						AthenaDesktopLink *link,
						GtkWidget *parent_view);

/* Used by athena-desktop-link.c */
char * athena_desktop_link_monitor_make_filename_unique (AthenaDesktopLinkMonitor *monitor,
							   const char *filename);

#endif /* ATHENA_DESKTOP_LINK_MONITOR_H */
