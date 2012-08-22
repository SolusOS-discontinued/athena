/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * athena-application: main Athena application class.
 *
 * Copyright (C) 2000 Red Hat, Inc.
 * Copyright (C) 2010 Cosimo Cecchi <cosimoc@gnome.org>
 *
 * Athena is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Athena is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __ATHENA_APPLICATION_H__
#define __ATHENA_APPLICATION_H__

#include <gdk/gdk.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include <libathena-private/athena-undo-manager.h>

#include "athena-window.h"

#define ATHENA_DESKTOP_ICON_VIEW_IID	"OAFIID:Athena_File_Manager_Desktop_Icon_View"

#define ATHENA_TYPE_APPLICATION athena_application_get_type()
#define ATHENA_APPLICATION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_APPLICATION, AthenaApplication))
#define ATHENA_APPLICATION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_APPLICATION, AthenaApplicationClass))
#define ATHENA_IS_APPLICATION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_APPLICATION))
#define ATHENA_IS_APPLICATION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_APPLICATION))
#define ATHENA_APPLICATION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_APPLICATION, AthenaApplicationClass))

#ifndef ATHENA_SPATIAL_WINDOW_DEFINED
#define ATHENA_SPATIAL_WINDOW_DEFINED
typedef struct _AthenaSpatialWindow AthenaSpatialWindow;
#endif

typedef struct _AthenaApplicationPriv AthenaApplicationPriv;

typedef struct {
	GtkApplication parent;

	AthenaUndoManager *undo_manager;

	AthenaApplicationPriv *priv;
} AthenaApplication;

typedef struct {
	GtkApplicationClass parent_class;
} AthenaApplicationClass;

GType athena_application_get_type (void);

AthenaApplication *athena_application_get_singleton (void);

void athena_application_quit (AthenaApplication *self);

AthenaWindow *     athena_application_create_window (AthenaApplication *application,
							 GdkScreen           *screen);

void athena_application_open_location (AthenaApplication *application,
					 GFile *location,
					 GFile *selection,
					 const char *startup_id);

void athena_application_close_all_windows (AthenaApplication *self);

#endif /* __ATHENA_APPLICATION_H__ */
