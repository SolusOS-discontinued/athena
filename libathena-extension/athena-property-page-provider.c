/*
 *  athena-property-page-provider.c - Interface for Athena extensions
 *                                      that provide property pages for 
 *                                      files.
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
#include "athena-property-page-provider.h"

#include <glib-object.h>

static void
athena_property_page_provider_base_init (gpointer g_class)
{
}

GType                   
athena_property_page_provider_get_type (void)
{
	static GType type = 0;

	if (!type) {
		const GTypeInfo info = {
			sizeof (AthenaPropertyPageProviderIface),
			athena_property_page_provider_base_init,
			NULL,
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		
		type = g_type_register_static (G_TYPE_INTERFACE, 
					       "AthenaPropertyPageProvider",
					       &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}

	return type;
}

/**
 * athena_property_page_provider_get_pages:
 * @provider: a #AthenaPropertyPageProvider
 * @files: (element-type AthenaFileInfo): a #GList of #AthenaFileInfo
 *
 * This function is called by Athena when it wants property page
 * items from the extension.
 *
 * This function is called in the main thread before a property page
 * is shown, so it should return quickly.
 *
 * Returns: (element-type AthenaPropertyPage) (transfer full): A #GList of allocated #AthenaPropertyPage items.
 */
GList *
athena_property_page_provider_get_pages (AthenaPropertyPageProvider *provider,
					   GList *files)
{
	g_return_val_if_fail (ATHENA_IS_PROPERTY_PAGE_PROVIDER (provider), NULL);
	g_return_val_if_fail (ATHENA_PROPERTY_PAGE_PROVIDER_GET_IFACE (provider)->get_pages != NULL, NULL);

	return ATHENA_PROPERTY_PAGE_PROVIDER_GET_IFACE (provider)->get_pages 
		(provider, files);	
}

					       
