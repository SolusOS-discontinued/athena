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

#include <config.h>
#include "athena-search-engine.h"
#include "athena-search-engine-simple.h"

#ifdef ENABLE_TRACKER
#include "athena-search-engine-tracker.h"
#endif

enum {
	HITS_ADDED,
	HITS_SUBTRACTED,
	FINISHED,
	ERROR,
	LAST_SIGNAL
}; 

static guint signals[LAST_SIGNAL];

G_DEFINE_ABSTRACT_TYPE (AthenaSearchEngine, athena_search_engine,
			G_TYPE_OBJECT);

static void
athena_search_engine_class_init (AthenaSearchEngineClass *class)
{
	signals[HITS_ADDED] =
		g_signal_new ("hits-added",
		              G_TYPE_FROM_CLASS (class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (AthenaSearchEngineClass, hits_added),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1,
			      G_TYPE_POINTER);

	signals[HITS_SUBTRACTED] =
		g_signal_new ("hits-subtracted",
		              G_TYPE_FROM_CLASS (class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (AthenaSearchEngineClass, hits_subtracted),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1,
			      G_TYPE_POINTER);

	signals[FINISHED] =
		g_signal_new ("finished",
		              G_TYPE_FROM_CLASS (class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (AthenaSearchEngineClass, finished),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	
	signals[ERROR] =
		g_signal_new ("error",
		              G_TYPE_FROM_CLASS (class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (AthenaSearchEngineClass, error),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1,
			      G_TYPE_STRING);

}

static void
athena_search_engine_init (AthenaSearchEngine *engine)
{
}

AthenaSearchEngine *
athena_search_engine_new (void)
{
	AthenaSearchEngine *engine;
	
#ifdef ENABLE_TRACKER	
	engine = athena_search_engine_tracker_new ();
	if (engine) {
		return engine;
	}
#endif
	
	engine = athena_search_engine_simple_new ();
	return engine;
}

void
athena_search_engine_set_query (AthenaSearchEngine *engine, AthenaQuery *query)
{
	g_return_if_fail (ATHENA_IS_SEARCH_ENGINE (engine));
	g_return_if_fail (ATHENA_SEARCH_ENGINE_GET_CLASS (engine)->set_query != NULL);

	ATHENA_SEARCH_ENGINE_GET_CLASS (engine)->set_query (engine, query);
}

void
athena_search_engine_start (AthenaSearchEngine *engine)
{
	g_return_if_fail (ATHENA_IS_SEARCH_ENGINE (engine));
	g_return_if_fail (ATHENA_SEARCH_ENGINE_GET_CLASS (engine)->start != NULL);

	ATHENA_SEARCH_ENGINE_GET_CLASS (engine)->start (engine);
}


void
athena_search_engine_stop (AthenaSearchEngine *engine)
{
	g_return_if_fail (ATHENA_IS_SEARCH_ENGINE (engine));
	g_return_if_fail (ATHENA_SEARCH_ENGINE_GET_CLASS (engine)->stop != NULL);

	ATHENA_SEARCH_ENGINE_GET_CLASS (engine)->stop (engine);
}

void	       
athena_search_engine_hits_added (AthenaSearchEngine *engine, GList *hits)
{
	g_return_if_fail (ATHENA_IS_SEARCH_ENGINE (engine));

	g_signal_emit (engine, signals[HITS_ADDED], 0, hits);
}


void	       
athena_search_engine_hits_subtracted (AthenaSearchEngine *engine, GList *hits)
{
	g_return_if_fail (ATHENA_IS_SEARCH_ENGINE (engine));

	g_signal_emit (engine, signals[HITS_SUBTRACTED], 0, hits);
}


void	       
athena_search_engine_finished (AthenaSearchEngine *engine)
{
	g_return_if_fail (ATHENA_IS_SEARCH_ENGINE (engine));

	g_signal_emit (engine, signals[FINISHED], 0);
}

void
athena_search_engine_error (AthenaSearchEngine *engine, const char *error_message)
{
	g_return_if_fail (ATHENA_IS_SEARCH_ENGINE (engine));

	g_signal_emit (engine, signals[ERROR], 0, error_message);
}
