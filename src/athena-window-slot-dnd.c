/*
 * athena-window-slot-dnd.c - Handle DnD for widgets acting as
 * AthenaWindowSlot proxies
 *
 * Copyright (C) 2000, 2001 Eazel, Inc.
 * Copyright (C) 2010, Red Hat, Inc.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors: Pavel Cisler <pavel@eazel.com>,
 * 	    Ettore Perazzoli <ettore@gnu.org>
 */

#include <config.h>

#include "athena-view-dnd.h"
#include "athena-window-slot-dnd.h"

typedef struct {
  gboolean have_data;
  gboolean have_valid_data;

  gboolean drop_occured;

  unsigned int info;
  union {
    GList *selection_list;
    GList *uri_list;
    char *netscape_url;
  } data;

  AthenaFile *target_file;
  AthenaWindowSlot *target_slot;
} AthenaDragSlotProxyInfo;

static gboolean
slot_proxy_drag_motion (GtkWidget          *widget,
			GdkDragContext     *context,
			int                 x,
			int                 y,
			unsigned int        time,
			gpointer            user_data)
{
  AthenaDragSlotProxyInfo *drag_info;
  AthenaWindowSlot *target_slot;
  GtkWidget *window;
  GdkAtom target;
  int action;
  char *target_uri;

  drag_info = user_data;

  action = 0;

  if (gtk_drag_get_source_widget (context) == widget) {
    goto out;
  }

  window = gtk_widget_get_toplevel (widget);
  g_assert (ATHENA_IS_WINDOW (window));

  if (!drag_info->have_data) {
    target = gtk_drag_dest_find_target (widget, context, NULL);

    if (target == GDK_NONE) {
      goto out;
    }

    gtk_drag_get_data (widget, context, target, time);
  }

  target_uri = NULL;
  if (drag_info->target_file != NULL) {
    target_uri = athena_file_get_uri (drag_info->target_file);
  } else {
    if (drag_info->target_slot != NULL) {
      target_slot = drag_info->target_slot;
    } else {
      target_slot = athena_window_get_active_slot (ATHENA_WINDOW (window));
    }

    if (target_slot != NULL) {
      target_uri = athena_window_slot_get_current_uri (target_slot);
    }
  }

  if (drag_info->have_data &&
      drag_info->have_valid_data) {
    if (drag_info->info == ATHENA_ICON_DND_GNOME_ICON_LIST) {
      athena_drag_default_drop_action_for_icons (context, target_uri,
                                                   drag_info->data.selection_list,
                                                   &action);
    } else if (drag_info->info == ATHENA_ICON_DND_URI_LIST) {
      action = athena_drag_default_drop_action_for_uri_list (context, target_uri);
    } else if (drag_info->info == ATHENA_ICON_DND_NETSCAPE_URL) {
      action = athena_drag_default_drop_action_for_netscape_url (context);
    }
  }

  g_free (target_uri);

 out:
  if (action != 0) {
    gtk_drag_highlight (widget);
  } else {
    gtk_drag_unhighlight (widget);
  }

  gdk_drag_status (context, action, time);

  return TRUE;
}

static void
drag_info_free (gpointer user_data)
{
  AthenaDragSlotProxyInfo *drag_info = user_data;

  g_clear_object (&drag_info->target_file);
  g_clear_object (&drag_info->target_slot);

  g_slice_free (AthenaDragSlotProxyInfo, drag_info);
}

static void
drag_info_clear (AthenaDragSlotProxyInfo *drag_info)
{
  if (!drag_info->have_data) {
    goto out;
  }

  if (drag_info->info == ATHENA_ICON_DND_GNOME_ICON_LIST) {
    athena_drag_destroy_selection_list (drag_info->data.selection_list);
  } else if (drag_info->info == ATHENA_ICON_DND_URI_LIST) {
    g_list_free (drag_info->data.uri_list);
  } else if (drag_info->info == ATHENA_ICON_DND_NETSCAPE_URL) {
    g_free (drag_info->data.netscape_url);
  }

 out:
  drag_info->have_data = FALSE;
  drag_info->have_valid_data = FALSE;

  drag_info->drop_occured = FALSE;
}

static void
slot_proxy_drag_leave (GtkWidget          *widget,
		       GdkDragContext     *context,
		       unsigned int        time,
		       gpointer            user_data)
{
  AthenaDragSlotProxyInfo *drag_info;

  drag_info = user_data;

  gtk_drag_unhighlight (widget);
  drag_info_clear (drag_info);
}

static gboolean
slot_proxy_drag_drop (GtkWidget          *widget,
		      GdkDragContext     *context,
		      int                 x,
		      int                 y,
		      unsigned int        time,
		      gpointer            user_data)
{
  GdkAtom target;
  AthenaDragSlotProxyInfo *drag_info;

  drag_info = user_data;
  g_assert (!drag_info->have_data);

  drag_info->drop_occured = TRUE;

  target = gtk_drag_dest_find_target (widget, context, NULL);
  gtk_drag_get_data (widget, context, target, time);

  return TRUE;
}


static void
slot_proxy_handle_drop (GtkWidget                *widget,
			GdkDragContext           *context,
			unsigned int              time,
			AthenaDragSlotProxyInfo *drag_info)
{
  GtkWidget *window;
  AthenaWindowSlot *target_slot;
  AthenaView *target_view;
  char *target_uri;
  GList *uri_list;

  if (!drag_info->have_data ||
      !drag_info->have_valid_data) {
    gtk_drag_finish (context, FALSE, FALSE, time);
    drag_info_clear (drag_info);
    return;
  }

  window = gtk_widget_get_toplevel (widget);
  g_assert (ATHENA_IS_WINDOW (window));

  if (drag_info->target_slot != NULL) {
    target_slot = drag_info->target_slot;
  } else {
    target_slot = athena_window_get_active_slot (ATHENA_WINDOW (window));
  }

  target_uri = NULL;
  if (drag_info->target_file != NULL) {
    target_uri = athena_file_get_uri (drag_info->target_file);
  } else if (target_slot != NULL) {
    target_uri = athena_window_slot_get_current_uri (target_slot);
  }

  target_view = NULL;
  if (target_slot != NULL) {
    target_view = athena_window_slot_get_current_view (target_slot);
  }

  if (target_slot != NULL && target_view != NULL) {
    if (drag_info->info == ATHENA_ICON_DND_GNOME_ICON_LIST) {
      uri_list = athena_drag_uri_list_from_selection_list (drag_info->data.selection_list);
      g_assert (uri_list != NULL);

      athena_view_drop_proxy_received_uris (target_view,
                                              uri_list,
                                              target_uri,
                                              gdk_drag_context_get_selected_action (context));
      g_list_free_full (uri_list, g_free);
    } else if (drag_info->info == ATHENA_ICON_DND_URI_LIST) {
      athena_view_drop_proxy_received_uris (target_view,
                                              drag_info->data.uri_list,
                                              target_uri,
                                              gdk_drag_context_get_selected_action (context));
    } if (drag_info->info == ATHENA_ICON_DND_NETSCAPE_URL) {
      athena_view_handle_netscape_url_drop (target_view,
                                              drag_info->data.netscape_url,
                                              target_uri,
                                              gdk_drag_context_get_selected_action (context),
                                              0, 0);
    }


    gtk_drag_finish (context, TRUE, FALSE, time);
  } else {
    gtk_drag_finish (context, FALSE, FALSE, time);
  }

  g_free (target_uri);

  drag_info_clear (drag_info);
}

static void
slot_proxy_drag_data_received (GtkWidget          *widget,
			       GdkDragContext     *context,
			       int                 x,
			       int                 y,
			       GtkSelectionData   *data,
			       unsigned int        info,
			       unsigned int        time,
			       gpointer            user_data)
{
  AthenaDragSlotProxyInfo *drag_info;
  char **uris;

  drag_info = user_data;

  g_assert (!drag_info->have_data);

  drag_info->have_data = TRUE;
  drag_info->info = info;

  if (gtk_selection_data_get_length (data) < 0) {
    drag_info->have_valid_data = FALSE;
    return;
  }

  if (info == ATHENA_ICON_DND_GNOME_ICON_LIST) {
    drag_info->data.selection_list = athena_drag_build_selection_list (data);

    drag_info->have_valid_data = drag_info->data.selection_list != NULL;
  } else if (info == ATHENA_ICON_DND_URI_LIST) {
    uris = gtk_selection_data_get_uris (data);
    drag_info->data.uri_list = athena_drag_uri_list_from_array ((const char **) uris);
    g_strfreev (uris);

    drag_info->have_valid_data = drag_info->data.uri_list != NULL;
  } else if (info == ATHENA_ICON_DND_NETSCAPE_URL) {
    drag_info->data.netscape_url = g_strdup ((char *) gtk_selection_data_get_data (data));

    drag_info->have_valid_data = drag_info->data.netscape_url != NULL;
  }

  if (drag_info->drop_occured) {
    slot_proxy_handle_drop (widget, context, time, drag_info);
  }
}

void
athena_drag_slot_proxy_init (GtkWidget *widget,
                               AthenaFile *target_file,
                               AthenaWindowSlot *target_slot)
{
  AthenaDragSlotProxyInfo *drag_info;

  const GtkTargetEntry targets[] = {
    { ATHENA_ICON_DND_GNOME_ICON_LIST_TYPE, 0, ATHENA_ICON_DND_GNOME_ICON_LIST },
    { ATHENA_ICON_DND_NETSCAPE_URL_TYPE, 0, ATHENA_ICON_DND_NETSCAPE_URL }
  };
  GtkTargetList *target_list;

  g_assert (GTK_IS_WIDGET (widget));

  drag_info = g_slice_new0 (AthenaDragSlotProxyInfo);

  g_object_set_data_full (G_OBJECT (widget), "drag-slot-proxy-data", drag_info,
                          drag_info_free);

  if (target_file != NULL)
    drag_info->target_file = g_object_ref (target_file);

  if (target_slot != NULL)
    drag_info->target_slot = g_object_ref (target_slot);

  gtk_drag_dest_set (widget, 0,
                     NULL, 0,
                     GDK_ACTION_MOVE |
                     GDK_ACTION_COPY |
                     GDK_ACTION_LINK |
                     GDK_ACTION_ASK);

  target_list = gtk_target_list_new (targets, G_N_ELEMENTS (targets));
  gtk_target_list_add_uri_targets (target_list, ATHENA_ICON_DND_URI_LIST);
  gtk_drag_dest_set_target_list (widget, target_list);
  gtk_target_list_unref (target_list);

  g_signal_connect (widget, "drag-motion",
                    G_CALLBACK (slot_proxy_drag_motion),
                    drag_info);
  g_signal_connect (widget, "drag-drop",
                    G_CALLBACK (slot_proxy_drag_drop),
                    drag_info);
  g_signal_connect (widget, "drag-data-received",
                    G_CALLBACK (slot_proxy_drag_data_received),
                    drag_info);
  g_signal_connect (widget, "drag-leave",
                    G_CALLBACK (slot_proxy_drag_leave),
                    drag_info);
}
