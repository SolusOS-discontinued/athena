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

#ifndef __ATHENA_PROGRESS_INFO_MANAGER_H__
#define __ATHENA_PROGRESS_INFO_MANAGER_H__

#include <glib-object.h>

#include <libathena-private/athena-progress-info.h>

#define ATHENA_TYPE_PROGRESS_INFO_MANAGER athena_progress_info_manager_get_type()
#define ATHENA_PROGRESS_INFO_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_PROGRESS_INFO_MANAGER, AthenaProgressInfoManager))
#define ATHENA_PROGRESS_INFO_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_PROGRESS_INFO_MANAGER, AthenaProgressInfoManagerClass))
#define ATHENA_IS_PROGRESS_INFO_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_PROGRESS_INFO_MANAGER))
#define ATHENA_IS_PROGRESS_INFO_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_PROGRESS_INFO_MANAGER))
#define ATHENA_PROGRESS_INFO_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_PROGRESS_INFO_MANAGER, AthenaProgressInfoManagerClass))

typedef struct _AthenaProgressInfoManager AthenaProgressInfoManager;
typedef struct _AthenaProgressInfoManagerClass AthenaProgressInfoManagerClass;
typedef struct _AthenaProgressInfoManagerPriv AthenaProgressInfoManagerPriv;

struct _AthenaProgressInfoManager {
  GObject parent;

  /* private */
  AthenaProgressInfoManagerPriv *priv;
};

struct _AthenaProgressInfoManagerClass {
  GObjectClass parent_class;
};

GType athena_progress_info_manager_get_type (void);

AthenaProgressInfoManager* athena_progress_info_manager_new (void);

void athena_progress_info_manager_add_new_info (AthenaProgressInfoManager *self,
                                                  AthenaProgressInfo *info);
GList *athena_progress_info_manager_get_all_infos (AthenaProgressInfoManager *self);

G_END_DECLS

#endif /* __ATHENA_PROGRESS_INFO_MANAGER_H__ */
