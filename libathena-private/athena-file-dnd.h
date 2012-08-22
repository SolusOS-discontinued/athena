/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* athena-file-drag.h - Drag & drop handling code that operated on 
   AthenaFile objects.

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
*/

#ifndef ATHENA_FILE_DND_H
#define ATHENA_FILE_DND_H

#include <libathena-private/athena-dnd.h>
#include <libathena-private/athena-file.h>

#define ATHENA_FILE_DND_ERASE_KEYWORD "erase"

gboolean athena_drag_can_accept_item              (AthenaFile *drop_target_item,
						     const char   *item_uri);
gboolean athena_drag_can_accept_items             (AthenaFile *drop_target_item,
						     const GList  *items);
gboolean athena_drag_can_accept_info              (AthenaFile *drop_target_item,
						     AthenaIconDndTargetType drag_type,
						     const GList *items);

#endif /* ATHENA_FILE_DND_H */

