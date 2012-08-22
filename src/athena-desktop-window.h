/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Athena
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Athena is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Athena is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Authors: Darin Adler <darin@bentspoon.com>
 */

/* athena-desktop-window.h
 */

#ifndef ATHENA_DESKTOP_WINDOW_H
#define ATHENA_DESKTOP_WINDOW_H

#include "athena-window.h"

#define ATHENA_TYPE_DESKTOP_WINDOW athena_desktop_window_get_type()
#define ATHENA_DESKTOP_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_DESKTOP_WINDOW, AthenaDesktopWindow))
#define ATHENA_DESKTOP_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_DESKTOP_WINDOW, AthenaDesktopWindowClass))
#define ATHENA_IS_DESKTOP_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_DESKTOP_WINDOW))
#define ATHENA_IS_DESKTOP_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_DESKTOP_WINDOW))
#define ATHENA_DESKTOP_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_DESKTOP_WINDOW, AthenaDesktopWindowClass))

typedef struct AthenaDesktopWindowDetails AthenaDesktopWindowDetails;

typedef struct {
	AthenaWindow parent_spot;
	AthenaDesktopWindowDetails *details;
        gboolean affect_desktop_on_next_location_change;
} AthenaDesktopWindow;

typedef struct {
	AthenaWindowClass parent_spot;
} AthenaDesktopWindowClass;

GType                  athena_desktop_window_get_type            (void);
AthenaDesktopWindow *athena_desktop_window_new                 (GdkScreen *screen);
void                   athena_desktop_window_update_directory    (AthenaDesktopWindow *window);
gboolean               athena_desktop_window_loaded              (AthenaDesktopWindow *window);

#endif /* ATHENA_DESKTOP_WINDOW_H */
