/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-progress-info.h: file operation progress info.
 
   Copyright (C) 2007 Red Hat, Inc.
  
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

#ifndef ATHENA_PROGRESS_INFO_H
#define ATHENA_PROGRESS_INFO_H

#include <glib-object.h>
#include <gio/gio.h>

#define ATHENA_TYPE_PROGRESS_INFO         (athena_progress_info_get_type ())
#define ATHENA_PROGRESS_INFO(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_PROGRESS_INFO, AthenaProgressInfo))
#define ATHENA_PROGRESS_INFO_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_PROGRESS_INFO, AthenaProgressInfoClass))
#define ATHENA_IS_PROGRESS_INFO(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_PROGRESS_INFO))
#define ATHENA_IS_PROGRESS_INFO_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_PROGRESS_INFO))
#define ATHENA_PROGRESS_INFO_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_PROGRESS_INFO, AthenaProgressInfoClass))

typedef struct _AthenaProgressInfo      AthenaProgressInfo;
typedef struct _AthenaProgressInfoClass AthenaProgressInfoClass;

GType athena_progress_info_get_type (void) G_GNUC_CONST;

/* Signals:
   "changed" - status or details changed
   "progress-changed" - the percentage progress changed (or we pulsed if in activity_mode
   "started" - emited on job start
   "finished" - emitted when job is done
   
   All signals are emitted from idles in main loop.
   All methods are threadsafe.
 */

AthenaProgressInfo *athena_progress_info_new (void);

GList *       athena_get_all_progress_info (void);

char *        athena_progress_info_get_status      (AthenaProgressInfo *info);
char *        athena_progress_info_get_details     (AthenaProgressInfo *info);
double        athena_progress_info_get_progress    (AthenaProgressInfo *info);
GCancellable *athena_progress_info_get_cancellable (AthenaProgressInfo *info);
void          athena_progress_info_cancel          (AthenaProgressInfo *info);
gboolean      athena_progress_info_get_is_started  (AthenaProgressInfo *info);
gboolean      athena_progress_info_get_is_finished (AthenaProgressInfo *info);
gboolean      athena_progress_info_get_is_paused   (AthenaProgressInfo *info);

void          athena_progress_info_start           (AthenaProgressInfo *info);
void          athena_progress_info_finish          (AthenaProgressInfo *info);
void          athena_progress_info_pause           (AthenaProgressInfo *info);
void          athena_progress_info_resume          (AthenaProgressInfo *info);
void          athena_progress_info_set_status      (AthenaProgressInfo *info,
						      const char           *status);
void          athena_progress_info_take_status     (AthenaProgressInfo *info,
						      char                 *status);
void          athena_progress_info_set_details     (AthenaProgressInfo *info,
						      const char           *details);
void          athena_progress_info_take_details    (AthenaProgressInfo *info,
						      char                 *details);
void          athena_progress_info_set_progress    (AthenaProgressInfo *info,
						      double                current,
						      double                total);
void          athena_progress_info_pulse_progress  (AthenaProgressInfo *info);



#endif /* ATHENA_PROGRESS_INFO_H */
