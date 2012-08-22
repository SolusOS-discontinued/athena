/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Athena
 *
 *  Copyright (C) 2004 Red Hat, Inc.
 *  Copyright (C) 2003 Marco Pesenti Gritti
 *
 *  Athena is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Athena is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *  Based on ephy-navigation-action.h from Epiphany
 *
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *           Marco Pesenti Gritti
 *
 */

#ifndef ATHENA_NAVIGATION_ACTION_H
#define ATHENA_NAVIGATION_ACTION_H

#include <gtk/gtk.h>

#define ATHENA_TYPE_NAVIGATION_ACTION            (athena_navigation_action_get_type ())
#define ATHENA_NAVIGATION_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_NAVIGATION_ACTION, AthenaNavigationAction))
#define ATHENA_NAVIGATION_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_NAVIGATION_ACTION, AthenaNavigationActionClass))
#define ATHENA_IS_NAVIGATION_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_NAVIGATION_ACTION))
#define ATHENA_IS_NAVIGATION_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), ATHENA_TYPE_NAVIGATION_ACTION))
#define ATHENA_NAVIGATION_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), ATHENA_TYPE_NAVIGATION_ACTION, AthenaNavigationActionClass))

typedef struct _AthenaNavigationAction       AthenaNavigationAction;
typedef struct _AthenaNavigationActionClass  AthenaNavigationActionClass;
typedef struct AthenaNavigationActionPrivate AthenaNavigationActionPrivate;

typedef enum
{
	ATHENA_NAVIGATION_DIRECTION_BACK,
   	ATHENA_NAVIGATION_DIRECTION_FORWARD,
   	ATHENA_NAVIGATION_DIRECTION_UP,
   	ATHENA_NAVIGATION_DIRECTION_RELOAD,
   	ATHENA_NAVIGATION_DIRECTION_HOME,
   	ATHENA_NAVIGATION_DIRECTION_COMPUTER,
   	ATHENA_NAVIGATION_DIRECTION_EDIT
 
} AthenaNavigationDirection;

struct _AthenaNavigationAction
{
	GtkAction parent;
	
	/*< private >*/
	AthenaNavigationActionPrivate *priv;
};

struct _AthenaNavigationActionClass
{
	GtkActionClass parent_class;
};

GType    athena_navigation_action_get_type   (void);

#endif
