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
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Maciej Stachowiak <mjs@eazel.com>
 *         Ettore Perazzoli <ettore@gnu.org>
 */

/* athena-location-bar.h - Location bar for Athena
 */

#ifndef ATHENA_LOCATION_BAR_H
#define ATHENA_LOCATION_BAR_H

#include <libathena-private/athena-entry.h>
#include <gtk/gtk.h>

#define ATHENA_TYPE_LOCATION_BAR athena_location_bar_get_type()
#define ATHENA_LOCATION_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_LOCATION_BAR, AthenaLocationBar))
#define ATHENA_LOCATION_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_LOCATION_BAR, AthenaLocationBarClass))
#define ATHENA_IS_LOCATION_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_LOCATION_BAR))
#define ATHENA_IS_LOCATION_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_LOCATION_BAR))
#define ATHENA_LOCATION_BAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_LOCATION_BAR, AthenaLocationBarClass))

typedef struct AthenaLocationBarDetails AthenaLocationBarDetails;

typedef struct AthenaLocationBar {
	GtkBox parent;
	AthenaLocationBarDetails *details;
} AthenaLocationBar;

typedef struct {
	GtkBoxClass parent_class;

	/* for GtkBindingSet */
	void         (* cancel)           (AthenaLocationBar *bar);
} AthenaLocationBarClass;

GType      athena_location_bar_get_type     	(void);
GtkWidget* athena_location_bar_new          	(void);
AthenaEntry * athena_location_bar_get_entry (AthenaLocationBar *location_bar);

void	athena_location_bar_activate	 (AthenaLocationBar *bar);
void    athena_location_bar_set_location     (AthenaLocationBar *bar,
						const char          *location);

#endif /* ATHENA_LOCATION_BAR_H */
