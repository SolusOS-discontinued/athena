/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Athena
 *
 * Copyright (C) 2011 Red Hat, Inc.
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
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 */

#include <config.h>

#include "athena-progress-info-manager.h"

struct _AthenaProgressInfoManagerPriv {
	GList *progress_infos;
};

enum {
	NEW_PROGRESS_INFO,
	LAST_SIGNAL
};

static AthenaProgressInfoManager *singleton = NULL;

static guint signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (AthenaProgressInfoManager, athena_progress_info_manager,
               G_TYPE_OBJECT);

static void
athena_progress_info_manager_finalize (GObject *obj)
{
	AthenaProgressInfoManager *self = ATHENA_PROGRESS_INFO_MANAGER (obj);

	if (self->priv->progress_infos != NULL) {
		g_list_free_full (self->priv->progress_infos, g_object_unref);
	}

	G_OBJECT_CLASS (athena_progress_info_manager_parent_class)->finalize (obj);
}

static GObject *
athena_progress_info_manager_constructor (GType type,
					    guint n_props,
					    GObjectConstructParam *props)
{
	GObject *retval;

	if (singleton != NULL) {
		return g_object_ref (singleton);
	}

	retval = G_OBJECT_CLASS (athena_progress_info_manager_parent_class)->constructor
		(type, n_props, props);

	singleton = ATHENA_PROGRESS_INFO_MANAGER (retval);
	g_object_add_weak_pointer (retval, (gpointer) &singleton);

	return retval;
}

static void
athena_progress_info_manager_init (AthenaProgressInfoManager *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ATHENA_TYPE_PROGRESS_INFO_MANAGER,
						  AthenaProgressInfoManagerPriv);
}

static void
athena_progress_info_manager_class_init (AthenaProgressInfoManagerClass *klass)
{
	GObjectClass *oclass;

	oclass = G_OBJECT_CLASS (klass);
	oclass->constructor = athena_progress_info_manager_constructor;
	oclass->finalize = athena_progress_info_manager_finalize;

	signals[NEW_PROGRESS_INFO] =
		g_signal_new ("new-progress-info",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      0, NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE,
			      1,
			      ATHENA_TYPE_PROGRESS_INFO);

	g_type_class_add_private (klass, sizeof (AthenaProgressInfoManagerPriv));
}

static void
progress_info_finished_cb (AthenaProgressInfo *info,
			   AthenaProgressInfoManager *self)
{
	self->priv->progress_infos =
		g_list_remove (self->priv->progress_infos, info);
}

AthenaProgressInfoManager *
athena_progress_info_manager_new (void)
{
	return g_object_new (ATHENA_TYPE_PROGRESS_INFO_MANAGER, NULL);
}

void
athena_progress_info_manager_add_new_info (AthenaProgressInfoManager *self,
					     AthenaProgressInfo *info)
{
	if (g_list_find (self->priv->progress_infos, info) != NULL) {
		g_warning ("Adding two times the same progress info object to the manager");
		return;
	}
	
	self->priv->progress_infos =
		g_list_prepend (self->priv->progress_infos, g_object_ref (info));

	g_signal_connect (info, "finished",
			  G_CALLBACK (progress_info_finished_cb), self);

	g_signal_emit (self, signals[NEW_PROGRESS_INFO], 0, info);
}

GList *
athena_progress_info_manager_get_all_infos (AthenaProgressInfoManager *self)
{
	return self->priv->progress_infos;
}
