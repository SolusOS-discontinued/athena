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

#ifndef ATHENA_SEARCH_ENGINE_H
#define ATHENA_SEARCH_ENGINE_H

#include <glib-object.h>
#include <libathena-private/athena-query.h>

#define ATHENA_TYPE_SEARCH_ENGINE		(athena_search_engine_get_type ())
#define ATHENA_SEARCH_ENGINE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_SEARCH_ENGINE, AthenaSearchEngine))
#define ATHENA_SEARCH_ENGINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_SEARCH_ENGINE, AthenaSearchEngineClass))
#define ATHENA_IS_SEARCH_ENGINE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_SEARCH_ENGINE))
#define ATHENA_IS_SEARCH_ENGINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_SEARCH_ENGINE))
#define ATHENA_SEARCH_ENGINE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_SEARCH_ENGINE, AthenaSearchEngineClass))

typedef struct AthenaSearchEngineDetails AthenaSearchEngineDetails;

typedef struct AthenaSearchEngine {
	GObject parent;
	AthenaSearchEngineDetails *details;
} AthenaSearchEngine;

typedef struct {
	GObjectClass parent_class;
	
	/* VTable */
	void (*set_query) (AthenaSearchEngine *engine, AthenaQuery *query);
	void (*start) (AthenaSearchEngine *engine);
	void (*stop) (AthenaSearchEngine *engine);

	/* Signals */
	void (*hits_added) (AthenaSearchEngine *engine, GList *hits);
	void (*hits_subtracted) (AthenaSearchEngine *engine, GList *hits);
	void (*finished) (AthenaSearchEngine *engine);
	void (*error) (AthenaSearchEngine *engine, const char *error_message);
} AthenaSearchEngineClass;

GType          athena_search_engine_get_type  (void);
gboolean       athena_search_engine_enabled (void);

AthenaSearchEngine* athena_search_engine_new       (void);

void           athena_search_engine_set_query (AthenaSearchEngine *engine, AthenaQuery *query);
void	       athena_search_engine_start (AthenaSearchEngine *engine);
void	       athena_search_engine_stop (AthenaSearchEngine *engine);

void	       athena_search_engine_hits_added (AthenaSearchEngine *engine, GList *hits);
void	       athena_search_engine_hits_subtracted (AthenaSearchEngine *engine, GList *hits);
void	       athena_search_engine_finished (AthenaSearchEngine *engine);
void	       athena_search_engine_error (AthenaSearchEngine *engine, const char *error_message);

#endif /* ATHENA_SEARCH_ENGINE_H */
