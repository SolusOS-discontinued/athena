/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Athena - Athena navigation state
 *
 * Copyright (C) 2011 Red Hat Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __ATHENA_NAVIGATION_STATE_H__
#define __ATHENA_NAVIGATION_STATE_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#define ATHENA_TYPE_NAVIGATION_STATE athena_navigation_state_get_type()
#define ATHENA_NAVIGATION_STATE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_NAVIGATION_STATE, AthenaNavigationState))
#define ATHENA_NAVIGATION_STATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_NAVIGATION_STATE, AthenaNavigationStateClass))
#define ATHENA_IS_NAVIGATION_STATE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_NAVIGATION_STATE))
#define ATHENA_IS_NAVIGATION_STATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_NAVIGATION_STATE))
#define ATHENA_NAVIGATION_STATE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_NAVIGATION_STATE, AthenaNavigationStateClass))

typedef struct _AthenaNavigationState AthenaNavigationState;
typedef struct _AthenaNavigationStateClass AthenaNavigationStateClass;
typedef struct _AthenaNavigationStateDetails AthenaNavigationStateDetails;

struct _AthenaNavigationState {
	GObject parent;
	AthenaNavigationStateDetails *priv;
};

struct _AthenaNavigationStateClass {
	GObjectClass parent_class;
};

/* GObject */
GType       athena_navigation_state_get_type  (void);

AthenaNavigationState * athena_navigation_state_new (GtkActionGroup *slave,
                                                         const gchar **action_names);

void athena_navigation_state_add_group (AthenaNavigationState *state,
                                          GtkActionGroup *group);
void athena_navigation_state_set_master (AthenaNavigationState *state,
                                           GtkActionGroup *master);
GtkActionGroup * athena_navigation_state_get_master (AthenaNavigationState *self);

void athena_navigation_state_sync_all (AthenaNavigationState *state);

void athena_navigation_state_set_boolean (AthenaNavigationState *self,
					    const gchar *action_name,
					    gboolean value);

#endif /* __ATHENA_NAVIGATION_STATE_H__ */
