/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2008 Red Hat, Inc.
 * Copyright (C) 2006 Paolo Borelli <pborelli@katamail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Authors: David Zeuthen <davidz@redhat.com>
 *          Paolo Borelli <pborelli@katamail.com>
 *
 */

#ifndef __ATHENA_X_CONTENT_BAR_H
#define __ATHENA_X_CONTENT_BAR_H

#include <gtk/gtk.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define ATHENA_TYPE_X_CONTENT_BAR         (athena_x_content_bar_get_type ())
#define ATHENA_X_CONTENT_BAR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_X_CONTENT_BAR, AthenaXContentBar))
#define ATHENA_X_CONTENT_BAR_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_X_CONTENT_BAR, AthenaXContentBarClass))
#define ATHENA_IS_X_CONTENT_BAR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_X_CONTENT_BAR))
#define ATHENA_IS_X_CONTENT_BAR_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_X_CONTENT_BAR))
#define ATHENA_X_CONTENT_BAR_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_X_CONTENT_BAR, AthenaXContentBarClass))

typedef struct AthenaXContentBarPrivate AthenaXContentBarPrivate;

typedef struct
{
	GtkInfoBar parent;

	AthenaXContentBarPrivate *priv;
} AthenaXContentBar;

typedef struct
{
	GtkInfoBarClass parent_class;
} AthenaXContentBarClass;

GType		 athena_x_content_bar_get_type	(void) G_GNUC_CONST;

GtkWidget	*athena_x_content_bar_new		   (GMount              *mount, 
							    const char          *x_content_type);

G_END_DECLS

#endif /* __ATHENA_X_CONTENT_BAR_H */
