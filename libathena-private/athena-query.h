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

#ifndef ATHENA_QUERY_H
#define ATHENA_QUERY_H

#include <glib-object.h>

#define ATHENA_TYPE_QUERY		(athena_query_get_type ())
#define ATHENA_QUERY(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_QUERY, AthenaQuery))
#define ATHENA_QUERY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_QUERY, AthenaQueryClass))
#define ATHENA_IS_QUERY(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_QUERY))
#define ATHENA_IS_QUERY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_QUERY))
#define ATHENA_QUERY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_QUERY, AthenaQueryClass))

typedef struct AthenaQueryDetails AthenaQueryDetails;

typedef struct AthenaQuery {
	GObject parent;
	AthenaQueryDetails *details;
} AthenaQuery;

typedef struct {
	GObjectClass parent_class;
} AthenaQueryClass;

GType          athena_query_get_type (void);
gboolean       athena_query_enabled  (void);

AthenaQuery* athena_query_new      (void);

char *         athena_query_get_text           (AthenaQuery *query);
void           athena_query_set_text           (AthenaQuery *query, const char *text);

char *         athena_query_get_location       (AthenaQuery *query);
void           athena_query_set_location       (AthenaQuery *query, const char *uri);

GList *        athena_query_get_mime_types     (AthenaQuery *query);
void           athena_query_set_mime_types     (AthenaQuery *query, GList *mime_types);
void           athena_query_add_mime_type      (AthenaQuery *query, const char *mime_type);

char *         athena_query_to_readable_string (AthenaQuery *query);
AthenaQuery *athena_query_load               (char *file);
gboolean       athena_query_save               (AthenaQuery *query, char *file);

#endif /* ATHENA_QUERY_H */
