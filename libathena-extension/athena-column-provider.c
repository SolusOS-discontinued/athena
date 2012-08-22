/*
 *  athena-column-provider.c - Interface for Athena extensions 
 *                               that provide column specifications.
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

#include <config.h>
#include "athena-column-provider.h"

#include <glib-object.h>

static void
athena_column_provider_base_init (gpointer g_class)
{
}

GType                   
athena_column_provider_get_type (void)
{
	static GType type = 0;

	if (!type) {
		const GTypeInfo info = {
			sizeof (AthenaColumnProviderIface),
			athena_column_provider_base_init,
			NULL,
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		
		type = g_type_register_static (G_TYPE_INTERFACE, 
					       "AthenaColumnProvider",
					       &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}

	return type;
}

/**
 * athena_column_provider_get_columns:
 * @provider: a #AthenaColumnProvider
 *
 * Returns: (element-type AthenaColumn) (transfer full): the provided #AthenaColumn objects
 */
GList *
athena_column_provider_get_columns (AthenaColumnProvider *provider)
{
	g_return_val_if_fail (ATHENA_IS_COLUMN_PROVIDER (provider), NULL);
	g_return_val_if_fail (ATHENA_COLUMN_PROVIDER_GET_IFACE (provider)->get_columns != NULL, NULL);

	return ATHENA_COLUMN_PROVIDER_GET_IFACE (provider)->get_columns 
		(provider);
}

					       
