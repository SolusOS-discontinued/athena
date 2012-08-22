/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Red Hat, Inc.
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
 * Author: Alexander Larsson <alexl@redhat.com>
 *
 */

#ifndef ATHENA_QUERY_EDITOR_H
#define ATHENA_QUERY_EDITOR_H

#include <gtk/gtk.h>

#include <libathena-private/athena-query.h>

#include "athena-search-bar.h"

#define ATHENA_TYPE_QUERY_EDITOR athena_query_editor_get_type()
#define ATHENA_QUERY_EDITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_QUERY_EDITOR, AthenaQueryEditor))
#define ATHENA_QUERY_EDITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_QUERY_EDITOR, AthenaQueryEditorClass))
#define ATHENA_IS_QUERY_EDITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_QUERY_EDITOR))
#define ATHENA_IS_QUERY_EDITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_QUERY_EDITOR))
#define ATHENA_QUERY_EDITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_QUERY_EDITOR, AthenaQueryEditorClass))

typedef struct AthenaQueryEditorDetails AthenaQueryEditorDetails;

typedef struct AthenaQueryEditor {
	GtkBox parent;
	AthenaQueryEditorDetails *details;
} AthenaQueryEditor;

typedef struct {
	GtkBoxClass parent_class;

	void (* changed) (AthenaQueryEditor  *editor,
			  AthenaQuery        *query,
			  gboolean              reload);
	void (* cancel)   (AthenaQueryEditor *editor);
} AthenaQueryEditorClass;

#include "athena-window-slot.h"

GType      athena_query_editor_get_type     	   (void);
GtkWidget* athena_query_editor_new          	   (gboolean start_hidden);
GtkWidget* athena_query_editor_new_with_bar      (gboolean start_hidden,
						    gboolean start_attached,
						    AthenaSearchBar *bar,
						    AthenaWindowSlot *slot);
void       athena_query_editor_set_default_query (AthenaQueryEditor *editor);

void	   athena_query_editor_grab_focus (AthenaQueryEditor *editor);
void       athena_query_editor_clear_query (AthenaQueryEditor *editor);

AthenaQuery *athena_query_editor_get_query   (AthenaQueryEditor *editor);
void           athena_query_editor_set_query   (AthenaQueryEditor *editor,
						  AthenaQuery       *query);
void           athena_query_editor_set_visible (AthenaQueryEditor *editor,
						  gboolean             visible);

#endif /* ATHENA_QUERY_EDITOR_H */
