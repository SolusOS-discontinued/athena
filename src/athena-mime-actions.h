/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-mime-actions.h - uri-specific versions of mime action functions

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

   Authors: Maciej Stachowiak <mjs@eazel.com>
*/

#ifndef ATHENA_MIME_ACTIONS_H
#define ATHENA_MIME_ACTIONS_H

#include <gio/gio.h>

#include <libathena-private/athena-file.h>

#include "athena-window.h"

AthenaFileAttributes athena_mime_actions_get_required_file_attributes (void);

GAppInfo *             athena_mime_get_default_application_for_file     (AthenaFile            *file);
GList *                athena_mime_get_applications_for_file            (AthenaFile            *file);

GAppInfo *             athena_mime_get_default_application_for_files    (GList                   *files);
GList *                athena_mime_get_applications_for_files           (GList                   *file);

gboolean               athena_mime_file_opens_in_view                   (AthenaFile            *file);
gboolean               athena_mime_file_opens_in_external_app           (AthenaFile            *file);
void                   athena_mime_activate_files                       (GtkWindow               *parent_window,
									   AthenaWindowSlot      *slot,
									   GList                   *files,
									   const char              *launch_directory,
									   AthenaWindowOpenFlags  flags,
									   gboolean                 user_confirmation);
void                   athena_mime_activate_file                        (GtkWindow               *parent_window,
									   AthenaWindowSlot      *slot_info,
									   AthenaFile            *file,
									   const char              *launch_directory,
									   AthenaWindowOpenFlags  flags);


#endif /* ATHENA_MIME_ACTIONS_H */
