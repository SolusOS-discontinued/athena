/*
 *  athena-menu-provider.h - Interface for Athena extensions that 
 *                             provide context menu items.
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
 * add context menu entries to files.  Extensions are called when
 * Athena constructs the context menu for a file.  They are passed a
 * list of AthenaFileInfo objects which holds the current selection */

#ifndef ATHENA_MENU_PROVIDER_H
#define ATHENA_MENU_PROVIDER_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include "athena-extension-types.h"
#include "athena-file-info.h"
#include "athena-menu.h"

G_BEGIN_DECLS

#define ATHENA_TYPE_MENU_PROVIDER           (athena_menu_provider_get_type ())
#define ATHENA_MENU_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_MENU_PROVIDER, AthenaMenuProvider))
#define ATHENA_IS_MENU_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_MENU_PROVIDER))
#define ATHENA_MENU_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATHENA_TYPE_MENU_PROVIDER, AthenaMenuProviderIface))

typedef struct _AthenaMenuProvider       AthenaMenuProvider;
typedef struct _AthenaMenuProviderIface  AthenaMenuProviderIface;

struct _AthenaMenuProviderIface {
	GTypeInterface g_iface;

	GList *(*get_file_items)       (AthenaMenuProvider *provider,
					GtkWidget            *window,
					GList                *files);
	GList *(*get_background_items) (AthenaMenuProvider *provider,
					GtkWidget            *window,
					AthenaFileInfo     *current_folder);
};

/* Interface Functions */
GType                   athena_menu_provider_get_type             (void);
GList                  *athena_menu_provider_get_file_items       (AthenaMenuProvider *provider,
								     GtkWidget            *window,
								     GList                *files);
GList                  *athena_menu_provider_get_background_items (AthenaMenuProvider *provider,
								     GtkWidget            *window,
								     AthenaFileInfo     *current_folder);

/* This function emit a signal to inform athena that its item list has changed. */
void                    athena_menu_provider_emit_items_updated_signal (AthenaMenuProvider *provider);

G_END_DECLS

#endif
