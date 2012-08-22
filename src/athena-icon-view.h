/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-icon-view.h - interface for icon view of directory.
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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
 * Authors: John Sullivan <sullivan@eazel.com>
 *
 */

#ifndef ATHENA_ICON_VIEW_H
#define ATHENA_ICON_VIEW_H

#include "athena-view.h"

typedef struct AthenaIconView AthenaIconView;
typedef struct AthenaIconViewClass AthenaIconViewClass;

#define ATHENA_TYPE_ICON_VIEW athena_icon_view_get_type()
#define ATHENA_ICON_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_ICON_VIEW, AthenaIconView))
#define ATHENA_ICON_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_ICON_VIEW, AthenaIconViewClass))
#define ATHENA_IS_ICON_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_ICON_VIEW))
#define ATHENA_IS_ICON_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_ICON_VIEW))
#define ATHENA_ICON_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_ICON_VIEW, AthenaIconViewClass))

#define ATHENA_ICON_VIEW_ID "OAFIID:Athena_File_Manager_Icon_View"
#define FM_COMPACT_VIEW_ID "OAFIID:Athena_File_Manager_Compact_View"

typedef struct AthenaIconViewDetails AthenaIconViewDetails;

struct AthenaIconView {
	AthenaView parent;
	AthenaIconViewDetails *details;
};

struct AthenaIconViewClass {
	AthenaViewClass parent_class;
};

/* GObject support */
GType   athena_icon_view_get_type      (void);
int     athena_icon_view_compare_files (AthenaIconView   *icon_view,
					  AthenaFile *a,
					  AthenaFile *b);
void    athena_icon_view_filter_by_screen (AthenaIconView *icon_view,
					     gboolean filter);
gboolean athena_icon_view_is_compact   (AthenaIconView *icon_view);

void    athena_icon_view_register         (void);
void    athena_icon_view_compact_register (void);

AthenaIconContainer * athena_icon_view_get_icon_container (AthenaIconView *view);

#endif /* ATHENA_ICON_VIEW_H */
