/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-clipboard-monitor.h: lets you notice clipboard changes.
    
   Copyright (C) 2004 Red Hat, Inc.
  
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

#ifndef ATHENA_CLIPBOARD_MONITOR_H
#define ATHENA_CLIPBOARD_MONITOR_H

#include <gtk/gtk.h>

#define ATHENA_TYPE_CLIPBOARD_MONITOR athena_clipboard_monitor_get_type()
#define ATHENA_CLIPBOARD_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_CLIPBOARD_MONITOR, AthenaClipboardMonitor))
#define ATHENA_CLIPBOARD_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_CLIPBOARD_MONITOR, AthenaClipboardMonitorClass))
#define ATHENA_IS_CLIPBOARD_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_CLIPBOARD_MONITOR))
#define ATHENA_IS_CLIPBOARD_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_CLIPBOARD_MONITOR))
#define ATHENA_CLIPBOARD_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_CLIPBOARD_MONITOR, AthenaClipboardMonitorClass))

typedef struct AthenaClipboardMonitorDetails AthenaClipboardMonitorDetails;
typedef struct AthenaClipboardInfo AthenaClipboardInfo;

typedef struct {
	GObject parent_slot;

	AthenaClipboardMonitorDetails *details;
} AthenaClipboardMonitor;

typedef struct {
	GObjectClass parent_slot;
  
	void (* clipboard_changed) (AthenaClipboardMonitor *monitor);
	void (* clipboard_info) (AthenaClipboardMonitor *monitor,
	                         AthenaClipboardInfo *info);
} AthenaClipboardMonitorClass;

struct AthenaClipboardInfo {
	GList *files;
	gboolean cut;
};

GType   athena_clipboard_monitor_get_type (void);

AthenaClipboardMonitor *   athena_clipboard_monitor_get (void);
void athena_clipboard_monitor_set_clipboard_info (AthenaClipboardMonitor *monitor,
                                                    AthenaClipboardInfo *info);
AthenaClipboardInfo * athena_clipboard_monitor_get_clipboard_info (AthenaClipboardMonitor *monitor);
void athena_clipboard_monitor_emit_changed (void);

void athena_clear_clipboard_callback (GtkClipboard *clipboard,
                                        gpointer      user_data);
void athena_get_clipboard_callback   (GtkClipboard     *clipboard,
                                        GtkSelectionData *selection_data,
                                        guint             info,
                                        gpointer          user_data);



#endif /* ATHENA_CLIPBOARD_MONITOR_H */

