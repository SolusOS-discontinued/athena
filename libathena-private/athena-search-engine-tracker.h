/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Mr Jamie McCracken
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
 * Author: Jamie McCracken (jamiemcc@gnome.org)
 *
 */

#ifndef ATHENA_SEARCH_ENGINE_TRACKER_H
#define ATHENA_SEARCH_ENGINE_TRACKER_H

#include <libathena-private/athena-search-engine.h>

#define ATHENA_TYPE_SEARCH_ENGINE_TRACKER		(athena_search_engine_tracker_get_type ())
#define ATHENA_SEARCH_ENGINE_TRACKER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_SEARCH_ENGINE_TRACKER, AthenaSearchEngineTracker))
#define ATHENA_SEARCH_ENGINE_TRACKER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_SEARCH_ENGINE_TRACKER, AthenaSearchEngineTrackerClass))
#define ATHENA_IS_SEARCH_ENGINE_TRACKER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_SEARCH_ENGINE_TRACKER))
#define ATHENA_IS_SEARCH_ENGINE_TRACKER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_SEARCH_ENGINE_TRACKER))
#define ATHENA_SEARCH_ENGINE_TRACKER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_SEARCH_ENGINE_TRACKER, AthenaSearchEngineTrackerClass))

typedef struct AthenaSearchEngineTrackerDetails AthenaSearchEngineTrackerDetails;

typedef struct AthenaSearchEngineTracker {
	AthenaSearchEngine parent;
	AthenaSearchEngineTrackerDetails *details;
} AthenaSearchEngineTracker;

typedef struct {
	AthenaSearchEngineClass parent_class;
} AthenaSearchEngineTrackerClass;

GType athena_search_engine_tracker_get_type (void);

AthenaSearchEngine* athena_search_engine_tracker_new (void);

#endif /* ATHENA_SEARCH_ENGINE_TRACKER_H */
