/*
 * athena-freedesktop-dbus: Implementation for the org.freedesktop DBus file-management interfaces
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Authors: Akshay Gupta <kitallis@gmail.com>
 *          Federico Mena Quintero <federico@gnome.org>
 */

#include <config.h>

#include "athena-freedesktop-dbus.h"
#include "athena-freedesktop-generated.h"

/* We share the same debug domain as athena-dbus-manager */
#define DEBUG_FLAG ATHENA_DEBUG_DBUS
#include <libathena-private/athena-debug.h>

#include "athena-properties-window.h"

#include <gio/gio.h>


typedef struct _AthenaFreedesktopDBus AthenaFreedesktopDBus;
typedef struct _AthenaFreedesktopDBusClass AthenaFreedesktopDBusClass;

struct _AthenaFreedesktopDBus {
	GObject parent;

	/* Parent application */
	AthenaApplication *application;

	/* Id from g_dbus_own_name() */
	guint owner_id;

	/* DBus paraphernalia */
	GDBusConnection *connection;
	GDBusObjectManagerServer *object_manager;

	/* Our DBus implementation skeleton */
	AthenaFreedesktopFileManager1 *skeleton;
};

struct _AthenaFreedesktopDBusClass {
	GObjectClass parent_class;
};

enum {
	PROP_APPLICATION = 1
};

#define SERVICE_TIMEOUT 5

static GType athena_freedesktop_dbus_get_type (void) G_GNUC_CONST;
G_DEFINE_TYPE (AthenaFreedesktopDBus, athena_freedesktop_dbus, G_TYPE_OBJECT);

static AthenaFreedesktopDBus *singleton = NULL;

static gboolean
skeleton_handle_show_items_cb (AthenaFreedesktopFileManager1 *object,
			       GDBusMethodInvocation *invocation,
			       const gchar *const *uris,
			       const gchar *startup_id,
			       gpointer data)
{
	AthenaFreedesktopDBus *fdb = data;
	int i;

	for (i = 0; uris[i] != NULL; i++) {
		GFile *file;
		GFile *parent;

		file = g_file_new_for_uri (uris[i]);
		parent = g_file_get_parent (file);

		if (parent != NULL) {
			athena_application_open_location (fdb->application, parent, file, startup_id);
			g_object_unref (parent);
		} else {
			athena_application_open_location (fdb->application, file, NULL, startup_id);
		}

		g_object_unref (file);
	}

	athena_freedesktop_file_manager1_complete_show_items (object, invocation);
	return TRUE;
}

static gboolean
skeleton_handle_show_folders_cb (AthenaFreedesktopFileManager1 *object,
				 GDBusMethodInvocation *invocation,
				 const gchar *const *uris,
				 const gchar *startup_id,
				 gpointer data)
{
	AthenaFreedesktopDBus *fdb = data;
	int i;

	for (i = 0; uris[i] != NULL; i++) {
		GFile *file;

		file = g_file_new_for_uri (uris[i]);

		athena_application_open_location (fdb->application, file, NULL, startup_id);

		g_object_unref (file);
	}

	athena_freedesktop_file_manager1_complete_show_folders (object, invocation);
	return TRUE;
}

static gboolean
skeleton_handle_show_item_properties_cb (AthenaFreedesktopFileManager1 *object,
					 GDBusMethodInvocation *invocation,
					 const gchar *const *uris,
					 const gchar *startup_id,
					 gpointer data)
{
	GList *files;
	int i;

	files = NULL;

	for (i = 0; uris[i] != NULL; i++) {
		files = g_list_prepend (files, athena_file_get_by_uri (uris[i]));
        }

	files = g_list_reverse (files);

	athena_properties_window_present (files, NULL, startup_id);

	athena_file_list_free (files);

	athena_freedesktop_file_manager1_complete_show_item_properties (object, invocation);
	return TRUE;
}

static gboolean
service_timeout_cb (gpointer data)
{
	AthenaFreedesktopDBus *fdb = data;

	DEBUG ("Reached the DBus service timeout");

	/* just unconditionally release here, as if an operation has been
	 * called, its progress handler will hold it alive for all the task duration.
	 */
	g_application_release (G_APPLICATION (fdb->application));

	return FALSE;
}

static void
bus_acquired_cb (GDBusConnection *conn,
		 const gchar     *name,
		 gpointer         user_data)
{
	AthenaFreedesktopDBus *fdb = user_data;

	DEBUG ("Bus acquired at %s", name);

	fdb->connection = g_object_ref (conn);
	fdb->object_manager = g_dbus_object_manager_server_new ("/org/freedesktop/FileManager1");

	fdb->skeleton = athena_freedesktop_file_manager1_skeleton_new ();

	g_signal_connect (fdb->skeleton, "handle-show-items",
			  G_CALLBACK (skeleton_handle_show_items_cb), fdb);
	g_signal_connect (fdb->skeleton, "handle-show-folders",
			  G_CALLBACK (skeleton_handle_show_folders_cb), fdb);
	g_signal_connect (fdb->skeleton, "handle-show-item-properties",
			  G_CALLBACK (skeleton_handle_show_item_properties_cb), fdb);

	g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (fdb->skeleton), fdb->connection, "/org/freedesktop/FileManager1", NULL);

	g_dbus_object_manager_server_set_connection (fdb->object_manager, fdb->connection);

	g_timeout_add_seconds (SERVICE_TIMEOUT, service_timeout_cb, fdb);
}

static void
name_acquired_cb (GDBusConnection *connection,
		  const gchar     *name,
		  gpointer         user_data)
{
	DEBUG ("Acquired the name %s on the session message bus\n", name);
}

static void
name_lost_cb (GDBusConnection *connection,
	      const gchar     *name,
	      gpointer         user_data)
{
	DEBUG ("Lost (or failed to acquire) the name %s on the session message bus\n", name);
}

static void
athena_freedesktop_dbus_dispose (GObject *object)
{
	AthenaFreedesktopDBus *fdb = (AthenaFreedesktopDBus *) object;

	if (fdb->owner_id != 0) {
		g_bus_unown_name (fdb->owner_id);
		fdb->owner_id = 0;
	}

	if (fdb->skeleton != NULL) {
		g_dbus_interface_skeleton_unexport (G_DBUS_INTERFACE_SKELETON (fdb->skeleton));
		g_object_unref (fdb->skeleton);
		fdb->skeleton = NULL;
	}

	g_clear_object (&fdb->object_manager);
	g_clear_object (&fdb->connection);
	fdb->application = NULL;

	G_OBJECT_CLASS (athena_freedesktop_dbus_parent_class)->dispose (object);
}

static void
athena_freedesktop_dbus_constructed (GObject *object)
{
	AthenaFreedesktopDBus *fdb = (AthenaFreedesktopDBus *) object;

	G_OBJECT_CLASS (athena_freedesktop_dbus_parent_class)->constructed (object);

	g_application_hold (G_APPLICATION (fdb->application));

	fdb->owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
					"org.freedesktop.FileManager1",
					G_BUS_NAME_OWNER_FLAGS_NONE,
					bus_acquired_cb,
					name_acquired_cb,
					name_lost_cb,
					fdb,
					NULL);
}

static void
athena_freedesktop_dbus_set_property (GObject *object,
					guint property_id,
					const GValue *value,
					GParamSpec *pspec)
{
	AthenaFreedesktopDBus *fdb = (AthenaFreedesktopDBus *) object;

	switch (property_id) {
	case PROP_APPLICATION:
		fdb->application = g_value_get_object (value);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void
athena_freedesktop_dbus_class_init (AthenaFreedesktopDBusClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose		= athena_freedesktop_dbus_dispose;
	object_class->constructed	= athena_freedesktop_dbus_constructed;
	object_class->set_property	= athena_freedesktop_dbus_set_property;

	g_object_class_install_property (object_class,
					 PROP_APPLICATION,
					 g_param_spec_object ("application",
							      "AthenaApplication instance",
							      "The owning AthenaApplication instance",
							      ATHENA_TYPE_APPLICATION,
							      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

}

static void
athena_freedesktop_dbus_init (AthenaFreedesktopDBus *fdb)
{
	/* nothing */
}

/* Tries to own the org.freedesktop.FileManager1 service name */
void
athena_freedesktop_dbus_start (AthenaApplication *app)
{	
	if (singleton != NULL) {
		return;
	}

	singleton = g_object_new (athena_freedesktop_dbus_get_type (),
				  "application", app,
				  NULL);
}

/* Releases the org.freedesktop.FileManager1 service name */
void
athena_freedesktop_dbus_stop (void)
{
	g_clear_object (&singleton);
}
