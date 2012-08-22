/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * athena-desktop-background.c: Helper object to handle desktop background
 *                                changes.
 *
 * Copyright (C) 2000 Eazel, Inc.
 * Copyright (C) 2010 Cosimo Cecchi <cosimoc@gnome.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors: Darin Adler <darin@bentspoon.com>
 *          Cosimo Cecchi <cosimoc@gnome.org>
 */

#ifndef __NAUTILIUS_DESKTOP_BACKGROUND_H__
#define __NAUTILIUS_DESKTOP_BACKGROUND_H__

#include <gtk/gtk.h>

#include "athena-icon-container.h"

typedef struct AthenaDesktopBackground AthenaDesktopBackground;
typedef struct AthenaDesktopBackgroundClass AthenaDesktopBackgroundClass;

#define ATHENA_TYPE_DESKTOP_BACKGROUND athena_desktop_background_get_type()
#define ATHENA_DESKTOP_BACKGROUND(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_DESKTOP_BACKGROUND, AthenaDesktopBackground))
#define ATHENA_DESKTOP_BACKGROUND_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_DESKTOP_BACKGROUND, AthenaDesktopBackgroundClass))
#define ATHENA_IS_DESKTOP_BACKGROUND(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_DESKTOP_BACKGROUND))
#define ATHENA_IS_DESKTOP_BACKGROUND_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_DESKTOP_BACKGROUND))
#define ATHENA_DESKTOP_BACKGROUND_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_DESKTOP_BACKGROUND, AthenaDesktopBackgroundClass))

GType athena_desktop_background_get_type (void);
AthenaDesktopBackground * athena_desktop_background_new (AthenaIconContainer *container);

void athena_desktop_background_receive_dropped_background_image (AthenaDesktopBackground *self,
								   const gchar *image_uri);

typedef struct AthenaDesktopBackgroundDetails AthenaDesktopBackgroundDetails;

struct AthenaDesktopBackground {
	GObject parent;
	AthenaDesktopBackgroundDetails *details;
};

struct AthenaDesktopBackgroundClass {
	GObjectClass parent_class;
};

#endif /* __NAUTILIUS_DESKTOP_BACKGROUND_H__ */
