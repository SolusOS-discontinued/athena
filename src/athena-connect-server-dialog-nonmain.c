/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Athena
 *
 * Copyright (C) 2005 Red Hat, Inc.
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

#include <config.h>
#include <gio/gio.h>
#include "athena-connect-server-dialog.h"
#include <libathena-private/athena-global-preferences.h>

/* This file contains the glue for the calls from the connect to server dialog
 * to the main athena binary. A different version of this glue is in
 * athena-connect-server-dialog-main.c for the standalone version.
 */

static GSimpleAsyncResult *display_location_res = NULL;

static void
window_go_to_cb (AthenaWindow *window,
		 GError *error,
		 gpointer user_data)
{
	if (error != NULL) {
		g_simple_async_result_set_from_error (display_location_res, error);
	}

	g_simple_async_result_complete (display_location_res);

	g_object_unref (display_location_res);
	display_location_res = NULL;
}

gboolean
athena_connect_server_dialog_display_location_finish (AthenaConnectServerDialog *self,
							GAsyncResult *res,
							GError **error)
{
	if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error)) {
		return FALSE;
	}

	return TRUE;
}

void
athena_connect_server_dialog_display_location_async (AthenaConnectServerDialog *self,
						       GFile *location,
						       GAsyncReadyCallback callback,
						       gpointer user_data)
{
	AthenaWindow *window;
	GtkWidget *widget;

	widget = GTK_WIDGET (self);

	display_location_res =
		g_simple_async_result_new (G_OBJECT (self),
					   callback, user_data,
					   athena_connect_server_dialog_display_location_async);

	window = athena_application_create_window (athena_application_get_singleton (),
						     gtk_widget_get_screen (widget));

	athena_window_go_to_full (window, location,
				    window_go_to_cb, self);
}
