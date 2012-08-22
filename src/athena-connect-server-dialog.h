/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Athena
 *
 * Copyright (C) 2003 Red Hat, Inc.
 * Copyright (C) 2010 Cosimo Cecchi <cosimoc@gnome.org>
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
 */

#ifndef ATHENA_CONNECT_SERVER_DIALOG_H
#define ATHENA_CONNECT_SERVER_DIALOG_H

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "athena-application.h"
#include "athena-window.h"

#define ATHENA_TYPE_CONNECT_SERVER_DIALOG\
	(athena_connect_server_dialog_get_type ())
#define ATHENA_CONNECT_SERVER_DIALOG(obj)\
        (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_CONNECT_SERVER_DIALOG,\
				     AthenaConnectServerDialog))
#define ATHENA_CONNECT_SERVER_DIALOG_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_CONNECT_SERVER_DIALOG,\
				  AthenaConnectServerDialogClass))
#define ATHENA_IS_CONNECT_SERVER_DIALOG(obj)\
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_CONNECT_SERVER_DIALOG)

typedef struct _AthenaConnectServerDialog AthenaConnectServerDialog;
typedef struct _AthenaConnectServerDialogClass AthenaConnectServerDialogClass;
typedef struct _AthenaConnectServerDialogDetails AthenaConnectServerDialogDetails;

struct _AthenaConnectServerDialog {
	GtkDialog parent;
	AthenaConnectServerDialogDetails *details;
};

struct _AthenaConnectServerDialogClass {
	GtkDialogClass parent_class;
};

GType athena_connect_server_dialog_get_type (void);

GtkWidget* athena_connect_server_dialog_new (AthenaWindow *window);

void athena_connect_server_dialog_display_location_async (AthenaConnectServerDialog *self,
							    GFile *location,
							    GAsyncReadyCallback callback,
							    gpointer user_data);
gboolean athena_connect_server_dialog_display_location_finish (AthenaConnectServerDialog *self,
								 GAsyncResult *result,
								 GError **error);

void athena_connect_server_dialog_fill_details_async (AthenaConnectServerDialog *self,
							GMountOperation *operation,
							const gchar *default_user,
							const gchar *default_domain,
							GAskPasswordFlags flags,
							GAsyncReadyCallback callback,
							gpointer user_data);
gboolean athena_connect_server_dialog_fill_details_finish (AthenaConnectServerDialog *self,
							     GAsyncResult *result);

#endif /* ATHENA_CONNECT_SERVER_DIALOG_H */
