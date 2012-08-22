/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-icon-container.h - the container widget for file manager icons

   Copyright (C) 2002 Sun Microsystems, Inc.

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

   Author: Michael Meeks <michael@ximian.com>
*/

#ifndef ATHENA_ICON_VIEW_CONTAINER_H
#define ATHENA_ICON_VIEW_CONTAINER_H

#include "athena-icon-view.h"

#include <libathena-private/athena-icon-container.h>

typedef struct AthenaIconViewContainer AthenaIconViewContainer;
typedef struct AthenaIconViewContainerClass AthenaIconViewContainerClass;

#define ATHENA_TYPE_ICON_VIEW_CONTAINER athena_icon_view_container_get_type()
#define ATHENA_ICON_VIEW_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_ICON_VIEW_CONTAINER, AthenaIconViewContainer))
#define ATHENA_ICON_VIEW_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_ICON_VIEW_CONTAINER, AthenaIconViewContainerClass))
#define ATHENA_IS_ICON_VIEW_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_ICON_VIEW_CONTAINER))
#define ATHENA_IS_ICON_VIEW_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_ICON_VIEW_CONTAINER))
#define ATHENA_ICON_VIEW_CONTAINER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_ICON_VIEW_CONTAINER, AthenaIconViewContainerClass))

typedef struct AthenaIconViewContainerDetails AthenaIconViewContainerDetails;

struct AthenaIconViewContainer {
	AthenaIconContainer parent;

	AthenaIconView *view;
	gboolean    sort_for_desktop;
};

struct AthenaIconViewContainerClass {
	AthenaIconContainerClass parent_class;
};

GType                  athena_icon_view_container_get_type         (void);
AthenaIconContainer *athena_icon_view_container_construct        (AthenaIconViewContainer *icon_container,
								      AthenaIconView      *view);
AthenaIconContainer *athena_icon_view_container_new              (AthenaIconView      *view);
void                   athena_icon_view_container_set_sort_desktop (AthenaIconViewContainer *container,
								      gboolean         desktop);

#endif /* ATHENA_ICON_VIEW_CONTAINER_H */
