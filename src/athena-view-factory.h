/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-view-factory.h: register and create AthenaViews
 
   Copyright (C) 2004 Red Hat Inc.
  
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

#ifndef ATHENA_VIEW_FACTORY_H
#define ATHENA_VIEW_FACTORY_H

#include <string.h>

#include <gio/gio.h>

#include "athena-view.h"
#include "athena-window-slot.h"

G_BEGIN_DECLS

typedef struct _AthenaViewInfo AthenaViewInfo;

struct _AthenaViewInfo {
	char *id;
	char *view_combo_label;               /* Foo View (used in preferences dialog and navigation combo) */
	char *view_menu_label_with_mnemonic;  /* View -> _Foo (this is the "_Foo" part) */
	char *error_label;                 /* The foo view encountered an error. */
	char *startup_error_label;         /* The foo view encountered an error while starting up. */
	char *display_location_label;      /* Display this location with the foo view. */
	AthenaView * (*create) (AthenaWindowSlot *slot);
	/* BONOBOTODO: More args here */
	gboolean (*supports_uri) (const char *uri,
				  GFileType file_type,
				  const char *mime_type);
};


void                    athena_view_factory_register          (AthenaViewInfo   *view_info);
const AthenaViewInfo *athena_view_factory_lookup            (const char         *id);
AthenaView *          athena_view_factory_create            (const char         *id,
								 AthenaWindowSlot *slot);
gboolean                athena_view_factory_view_supports_uri (const char         *id,
								 GFile              *location,
								 GFileType          file_type,
								 const char         *mime_type);
GList *                 athena_view_factory_get_views_for_uri (const char         *uri,
								 GFileType          file_type,
								 const char         *mime_type);




G_END_DECLS

#endif /* ATHENA_VIEW_FACTORY_H */
