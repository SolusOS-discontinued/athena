/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 *  athena-module.h - Interface to athena extensions
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
 *  Author: Dave Camp <dave@ximian.com>
 * 
 */

#include <config.h>
#include "athena-module.h"

#include <eel/eel-debug.h>
#include <gmodule.h>

#define ATHENA_TYPE_MODULE    	(athena_module_get_type ())
#define ATHENA_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_MODULE, AthenaModule))
#define ATHENA_MODULE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_MODULE, AthenaModule))
#define ATHENA_IS_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_MODULE))
#define ATHENA_IS_MODULE_CLASS(klass)	(G_TYPE_CLASS_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_MODULE))

typedef struct _AthenaModule        AthenaModule;
typedef struct _AthenaModuleClass   AthenaModuleClass;

struct _AthenaModule {
	GTypeModule parent;

	GModule *library;

	char *path;

	void (*initialize) (GTypeModule  *module);
	void (*shutdown)   (void);

	void (*list_types) (const GType **types,
			    int          *num_types);

};

struct _AthenaModuleClass {
	GTypeModuleClass parent;	
};

static GList *module_objects = NULL;

static GType athena_module_get_type (void);

G_DEFINE_TYPE (AthenaModule, athena_module, G_TYPE_TYPE_MODULE);

static gboolean
module_pulls_in_orbit (GModule *module)
{
	gpointer symbol;
	gboolean res;

	res = g_module_symbol (module, "ORBit_realloc_tcval", &symbol);

	return res;
}

static gboolean
athena_module_load (GTypeModule *gmodule)
{
	AthenaModule *module;
	
	module = ATHENA_MODULE (gmodule);
	
	module->library = g_module_open (module->path, G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);

	if (!module->library) {
		g_warning ("%s", g_module_error ());
		return FALSE;
	}

	/* ORBit installs atexit() handlers, which would get unloaded together
	 * with the module now that the main process doesn't depend on GConf anymore,
	 * causing athena to sefgault at exit.
	 * If we detect that an extension would pull in ORBit, we make the
	 * module resident to prevent that.
	 */
        if (module_pulls_in_orbit (module->library)) {
		g_module_make_resident (module->library);
        }

	if (!g_module_symbol (module->library,
			      "athena_module_initialize",
			      (gpointer *)&module->initialize) ||
	    !g_module_symbol (module->library,
			      "athena_module_shutdown",
			      (gpointer *)&module->shutdown) ||
	    !g_module_symbol (module->library,
			      "athena_module_list_types",
			      (gpointer *)&module->list_types)) {

		g_warning ("%s", g_module_error ());
		g_module_close (module->library);
		
		return FALSE;
	}

	module->initialize (gmodule);
	
	return TRUE;
}

static void
athena_module_unload (GTypeModule *gmodule)
{
	AthenaModule *module;
	
	module = ATHENA_MODULE (gmodule);
	
	module->shutdown ();
	
	g_module_close (module->library);
	
	module->initialize = NULL;
	module->shutdown = NULL;
	module->list_types = NULL;
}

static void
athena_module_finalize (GObject *object)
{
	AthenaModule *module;
	
	module = ATHENA_MODULE (object);

	g_free (module->path);
	
	G_OBJECT_CLASS (athena_module_parent_class)->finalize (object);
}

static void
athena_module_init (AthenaModule *module)
{
}

static void
athena_module_class_init (AthenaModuleClass *class)
{
	G_OBJECT_CLASS (class)->finalize = athena_module_finalize;
	G_TYPE_MODULE_CLASS (class)->load = athena_module_load;
	G_TYPE_MODULE_CLASS (class)->unload = athena_module_unload;
}

static void
module_object_weak_notify (gpointer user_data, GObject *object)
{
	module_objects = g_list_remove (module_objects, object);
}

static void
add_module_objects (AthenaModule *module)
{
	const GType *types;
	int num_types;
	int i;
	
	module->list_types (&types, &num_types);
	
	for (i = 0; i < num_types; i++) {
		if (types[i] == 0) { /* Work around broken extensions */
			break;
		}
		athena_module_add_type (types[i]);
	}
}

static AthenaModule *
athena_module_load_file (const char *filename)
{
	AthenaModule *module;
	
	module = g_object_new (ATHENA_TYPE_MODULE, NULL);
	module->path = g_strdup (filename);
	
	if (g_type_module_use (G_TYPE_MODULE (module))) {
		add_module_objects (module);
		g_type_module_unuse (G_TYPE_MODULE (module));
		return module;
	} else {
		g_object_unref (module);
		return NULL;
	}
}

static void
load_module_dir (const char *dirname)
{
	GDir *dir;
	
	dir = g_dir_open (dirname, 0, NULL);
	
	if (dir) {
		const char *name;
		
		while ((name = g_dir_read_name (dir))) {
			if (g_str_has_suffix (name, "." G_MODULE_SUFFIX)) {
				char *filename;

				filename = g_build_filename (dirname, 
							     name, 
							     NULL);
				athena_module_load_file (filename);
				g_free (filename);
			}
		}

		g_dir_close (dir);
	}
}

static void
free_module_objects (void)
{
	GList *l, *next;
	
	for (l = module_objects; l != NULL; l = next) {
		next = l->next;
		g_object_unref (l->data);
	}
	
	g_list_free (module_objects);
}

void
athena_module_setup (void)
{
	static gboolean initialized = FALSE;

	if (!initialized) {
		initialized = TRUE;
		
		load_module_dir (ATHENA_EXTENSIONDIR);

		eel_debug_call_at_shutdown (free_module_objects);
	}
}

GList *
athena_module_get_extensions_for_type (GType type)
{
	GList *l;
	GList *ret = NULL;
	
	for (l = module_objects; l != NULL; l = l->next) {
		if (G_TYPE_CHECK_INSTANCE_TYPE (G_OBJECT (l->data),
						type)) {
			g_object_ref (l->data);
			ret = g_list_prepend (ret, l->data);
		}
	}

	return ret;	
}

void
athena_module_extension_list_free (GList *extensions)
{
	GList *l, *next;
	
	for (l = extensions; l != NULL; l = next) {
		next = l->next;
		g_object_unref (l->data);
	}
	g_list_free (extensions);
}

void   
athena_module_add_type (GType type)
{
	GObject *object;
	
	object = g_object_new (type, NULL);
	g_object_weak_ref (object, 
			   (GWeakNotify)module_object_weak_notify,
			   NULL);

	module_objects = g_list_prepend (module_objects, object);
}
