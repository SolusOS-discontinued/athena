/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Athena
 *
 * Copyright (C) 2011, Red Hat, Inc.
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
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __ATHENA_TOOLBAR_H__
#define __ATHENA_TOOLBAR_H__

#include <gtk/gtk.h>

#define ATHENA_TYPE_TOOLBAR athena_toolbar_get_type()
#define ATHENA_TOOLBAR(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_TOOLBAR, AthenaToolbar))
#define ATHENA_TOOLBAR_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_TOOLBAR, AthenaToolbarClass))
#define ATHENA_IS_TOOLBAR(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_TOOLBAR))
#define ATHENA_IS_TOOLBAR_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_TOOLBAR))
#define ATHENA_TOOLBAR_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_TOOLBAR, AthenaToolbarClass))

typedef struct _AthenaToolbar AthenaToolbar;
typedef struct _AthenaToolbarPriv AthenaToolbarPriv;
typedef struct _AthenaToolbarClass AthenaToolbarClass;

typedef enum {
	ATHENA_TOOLBAR_MODE_PATH_BAR,
	ATHENA_TOOLBAR_MODE_LOCATION_BAR,
} AthenaToolbarMode;

struct _AthenaToolbar {
	GtkBox parent;

	/* private */
	AthenaToolbarPriv *priv;
};

struct _AthenaToolbarClass {
	GtkBoxClass parent_class;
};

GType athena_toolbar_get_type (void);

GtkWidget *athena_toolbar_new (GtkActionGroup *action_group);

GtkWidget *athena_toolbar_get_show_location_entry (AthenaToolbar *self);
GtkWidget *athena_toolbar_get_path_bar (AthenaToolbar *self);
GtkWidget *athena_toolbar_get_location_bar (AthenaToolbar *self);
GtkWidget *athena_toolbar_get_search_bar (AthenaToolbar *self);

void athena_toolbar_set_show_main_bar (AthenaToolbar *self,
					 gboolean show_main_bar);
void athena_toolbar_set_show_location_entry (AthenaToolbar *self,
					       gboolean show_location_entry);
void athena_toolbar_set_show_search_bar (AthenaToolbar *self,
					   gboolean show_search_bar);

#endif /* __ATHENA_TOOLBAR_H__ */
