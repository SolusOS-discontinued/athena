/*
 *  athena-property-page-provider.h - Interface for Athena extensions
 *                                      that provide property pages.
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
 * add property page to property dialogs.  Extensions are called when 
 * Athena needs property pages for a selection.  They are passed a 
 * list of AthenaFileInfo objects for which information should
 * be displayed  */

#ifndef ATHENA_PROPERTY_PAGE_PROVIDER_H
#define ATHENA_PROPERTY_PAGE_PROVIDER_H

#include <glib-object.h>
#include "athena-extension-types.h"
#include "athena-file-info.h"
#include "athena-property-page.h"

G_BEGIN_DECLS

#define ATHENA_TYPE_PROPERTY_PAGE_PROVIDER           (athena_property_page_provider_get_type ())
#define ATHENA_PROPERTY_PAGE_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_PROPERTY_PAGE_PROVIDER, AthenaPropertyPageProvider))
#define ATHENA_IS_PROPERTY_PAGE_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_PROPERTY_PAGE_PROVIDER))
#define ATHENA_PROPERTY_PAGE_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATHENA_TYPE_PROPERTY_PAGE_PROVIDER, AthenaPropertyPageProviderIface))

typedef struct _AthenaPropertyPageProvider       AthenaPropertyPageProvider;
typedef struct _AthenaPropertyPageProviderIface  AthenaPropertyPageProviderIface;

struct _AthenaPropertyPageProviderIface {
	GTypeInterface g_iface;

	GList *(*get_pages) (AthenaPropertyPageProvider     *provider,
			     GList                    *files);
};

/* Interface Functions */
GType                   athena_property_page_provider_get_type  (void);
GList                  *athena_property_page_provider_get_pages (AthenaPropertyPageProvider *provider,
								   GList                        *files);

G_END_DECLS

#endif
