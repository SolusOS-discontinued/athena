/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-icon-view.h - interface for icon view of directory.

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

   Authors: Mike Engber <engber@eazel.com>
*/

#ifndef ATHENA_DESKTOP_ICON_VIEW_H
#define ATHENA_DESKTOP_ICON_VIEW_H

#include "athena-icon-view.h"

#define ATHENA_TYPE_DESKTOP_ICON_VIEW athena_desktop_icon_view_get_type()
#define ATHENA_DESKTOP_ICON_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_DESKTOP_ICON_VIEW, AthenaDesktopIconView))
#define ATHENA_DESKTOP_ICON_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_DESKTOP_ICON_VIEW, AthenaDesktopIconViewClass))
#define ATHENA_IS_DESKTOP_ICON_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_DESKTOP_ICON_VIEW))
#define ATHENA_IS_DESKTOP_ICON_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_DESKTOP_ICON_VIEW))
#define ATHENA_DESKTOP_ICON_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_DESKTOP_ICON_VIEW, AthenaDesktopIconViewClass))

#define ATHENA_DESKTOP_ICON_VIEW_ID "OAFIID:Athena_File_Manager_Desktop_Icon_View"

typedef struct AthenaDesktopIconViewDetails AthenaDesktopIconViewDetails;
typedef struct {
	AthenaIconView parent;
	AthenaDesktopIconViewDetails *details;
} AthenaDesktopIconView;

typedef struct {
	AthenaIconViewClass parent_class;
} AthenaDesktopIconViewClass;

/* GObject support */
GType   athena_desktop_icon_view_get_type (void);
void athena_desktop_icon_view_register (void);

#endif /* ATHENA_DESKTOP_ICON_VIEW_H */
