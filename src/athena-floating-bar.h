/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Athena - Floating status bar.
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

#ifndef __ATHENA_FLOATING_BAR_H__
#define __ATHENA_FLOATING_BAR_H__

#include <gtk/gtk.h>

#define ATHENA_FLOATING_BAR_ACTION_ID_STOP 1

#define ATHENA_TYPE_FLOATING_BAR athena_floating_bar_get_type()
#define ATHENA_FLOATING_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_FLOATING_BAR, AthenaFloatingBar))
#define ATHENA_FLOATING_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_FLOATING_BAR, AthenaFloatingBarClass))
#define ATHENA_IS_FLOATING_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_FLOATING_BAR))
#define ATHENA_IS_FLOATING_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_FLOATING_BAR))
#define ATHENA_FLOATING_BAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_FLOATING_BAR, AthenaFloatingBarClass))

typedef struct _AthenaFloatingBar AthenaFloatingBar;
typedef struct _AthenaFloatingBarClass AthenaFloatingBarClass;
typedef struct _AthenaFloatingBarDetails AthenaFloatingBarDetails;

struct _AthenaFloatingBar {
	GtkBox parent;
	AthenaFloatingBarDetails *priv;
};

struct _AthenaFloatingBarClass {
	GtkBoxClass parent_class;
};

/* GObject */
GType       athena_floating_bar_get_type  (void);

GtkWidget * athena_floating_bar_new              (const gchar *label,
						    gboolean show_spinner);

void        athena_floating_bar_set_label        (AthenaFloatingBar *self,
						    const gchar *label);
void        athena_floating_bar_set_show_spinner (AthenaFloatingBar *self,
						    gboolean show_spinner);

void        athena_floating_bar_add_action       (AthenaFloatingBar *self,
						    const gchar *stock_id,
						    gint action_id);
void        athena_floating_bar_cleanup_actions  (AthenaFloatingBar *self);

#endif /* __ATHENA_FLOATING_BAR_H__ */

