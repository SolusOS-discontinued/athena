/*
 *  athena-info-provider.h - Interface for Athena extensions that 
 *                             provide info about files.
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
 * provide information about files.  Extensions are called when Athena 
 * needs information about a file.  They are passed a AthenaFileInfo 
 * object which should be filled with relevant information */

#ifndef ATHENA_INFO_PROVIDER_H
#define ATHENA_INFO_PROVIDER_H

#include <glib-object.h>
#include "athena-extension-types.h"
#include "athena-file-info.h"

G_BEGIN_DECLS

#define ATHENA_TYPE_INFO_PROVIDER           (athena_info_provider_get_type ())
#define ATHENA_INFO_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_INFO_PROVIDER, AthenaInfoProvider))
#define ATHENA_IS_INFO_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_INFO_PROVIDER))
#define ATHENA_INFO_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATHENA_TYPE_INFO_PROVIDER, AthenaInfoProviderIface))

typedef struct _AthenaInfoProvider       AthenaInfoProvider;
typedef struct _AthenaInfoProviderIface  AthenaInfoProviderIface;

typedef void (*AthenaInfoProviderUpdateComplete) (AthenaInfoProvider    *provider,
						    AthenaOperationHandle *handle,
						    AthenaOperationResult  result,
						    gpointer                 user_data);

struct _AthenaInfoProviderIface {
	GTypeInterface g_iface;

	AthenaOperationResult (*update_file_info) (AthenaInfoProvider     *provider,
						     AthenaFileInfo         *file,
						     GClosure                 *update_complete,
						     AthenaOperationHandle **handle);
	void                    (*cancel_update)    (AthenaInfoProvider     *provider,
						     AthenaOperationHandle  *handle);
};

/* Interface Functions */
GType                   athena_info_provider_get_type               (void);
AthenaOperationResult athena_info_provider_update_file_info       (AthenaInfoProvider     *provider,
								       AthenaFileInfo         *file,
								       GClosure                 *update_complete,
								       AthenaOperationHandle **handle);
void                    athena_info_provider_cancel_update          (AthenaInfoProvider     *provider,
								       AthenaOperationHandle  *handle);



/* Helper functions for implementations */
void                    athena_info_provider_update_complete_invoke (GClosure                 *update_complete,
								       AthenaInfoProvider     *provider,
								       AthenaOperationHandle  *handle,
								       AthenaOperationResult   result);

G_END_DECLS

#endif
