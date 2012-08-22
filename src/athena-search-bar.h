/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Novell, Inc.
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
 * Author: Anders Carlsson <andersca@imendio.com>
 *
 */

#ifndef ATHENA_SEARCH_BAR_H
#define ATHENA_SEARCH_BAR_H

#include <gtk/gtk.h>
#include <libathena-private/athena-query.h>

#define ATHENA_TYPE_SEARCH_BAR athena_search_bar_get_type()
#define ATHENA_SEARCH_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_SEARCH_BAR, AthenaSearchBar))
#define ATHENA_SEARCH_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_SEARCH_BAR, AthenaSearchBarClass))
#define ATHENA_IS_SEARCH_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_SEARCH_BAR))
#define ATHENA_IS_SEARCH_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_SEARCH_BAR))
#define ATHENA_SEARCH_BAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_SEARCH_BAR, AthenaSearchBarClass))

typedef struct AthenaSearchBarDetails AthenaSearchBarDetails;

typedef struct AthenaSearchBar {
	GtkBox parent;
	AthenaSearchBarDetails *details;
} AthenaSearchBar;

typedef struct {
	GtkBoxClass parent_class;

	void (* activate) (AthenaSearchBar *bar);
	void (* cancel)   (AthenaSearchBar *bar);
} AthenaSearchBarClass;

GType      athena_search_bar_get_type     	(void);
GtkWidget* athena_search_bar_new          	(void);

GtkWidget *    athena_search_bar_get_entry     (AthenaSearchBar *bar);
GtkWidget *    athena_search_bar_borrow_entry  (AthenaSearchBar *bar);
void           athena_search_bar_return_entry  (AthenaSearchBar *bar);
void           athena_search_bar_grab_focus    (AthenaSearchBar *bar);
AthenaQuery *athena_search_bar_get_query     (AthenaSearchBar *bar);
void           athena_search_bar_clear         (AthenaSearchBar *bar);

#endif /* ATHENA_SEARCH_BAR_H */
