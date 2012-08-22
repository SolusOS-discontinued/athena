/*
 *  athena-column-provider.h - Interface for Athena extensions that 
 *                               provide column descriptions.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

/* This interface is implemented by Athena extensions that want to
 * add columns to the list view and details to the icon view.
 * Extensions are asked for a list of columns to display.  Each
 * returned column refers to a string attribute which can be filled in
 * by AthenaInfoProvider */

#ifndef ATHENA_COLUMN_PROVIDER_H
#define ATHENA_COLUMN_PROVIDER_H

#include <glib-object.h>
#include "athena-extension-types.h"
#include "athena-column.h"

G_BEGIN_DECLS

#define ATHENA_TYPE_COLUMN_PROVIDER           (athena_column_provider_get_type ())
#define ATHENA_COLUMN_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_COLUMN_PROVIDER, AthenaColumnProvider))
#define ATHENA_IS_COLUMN_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_COLUMN_PROVIDER))
#define ATHENA_COLUMN_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATHENA_TYPE_COLUMN_PROVIDER, AthenaColumnProviderIface))

typedef struct _AthenaColumnProvider       AthenaColumnProvider;
typedef struct _AthenaColumnProviderIface  AthenaColumnProviderIface;

struct _AthenaColumnProviderIface {
	GTypeInterface g_iface;

	GList *(*get_columns) (AthenaColumnProvider *provider);
};

/* Interface Functions */
GType                   athena_column_provider_get_type       (void);
GList                  *athena_column_provider_get_columns    (AthenaColumnProvider *provider);

G_END_DECLS

#endif
