/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * athena-progress-ui-handler.h: file operation progress user interface.
 *
 * Copyright (C) 2007, 2011 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors: Alexander Larsson <alexl@redhat.com>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __ATHENA_PROGRESS_UI_HANDLER_H__
#define __ATHENA_PROGRESS_UI_HANDLER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define ATHENA_TYPE_PROGRESS_UI_HANDLER athena_progress_ui_handler_get_type()
#define ATHENA_PROGRESS_UI_HANDLER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_PROGRESS_UI_HANDLER, AthenaProgressUIHandler))
#define ATHENA_PROGRESS_UI_HANDLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_PROGRESS_UI_HANDLER, AthenaProgressUIHandlerClass))
#define ATHENA_IS_PROGRESS_UI_HANDLER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_PROGRESS_UI_HANDLER))
#define ATHENA_IS_PROGRESS_UI_HANDLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_PROGRESS_UI_HANDLER))
#define ATHENA_PROGRESS_UI_HANDLER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_PROGRESS_UI_HANDLER, AthenaProgressUIHandlerClass))

typedef struct _AthenaProgressUIHandlerPriv AthenaProgressUIHandlerPriv;

typedef struct {
  GObject parent;

  /* private */
  AthenaProgressUIHandlerPriv *priv;
} AthenaProgressUIHandler;

typedef struct {
  GObjectClass parent_class;
} AthenaProgressUIHandlerClass;

GType athena_progress_ui_handler_get_type (void);

AthenaProgressUIHandler * athena_progress_ui_handler_new (void);

G_END_DECLS

#endif /* __ATHENA_PROGRESS_UI_HANDLER_H__ */
