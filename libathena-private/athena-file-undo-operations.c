/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-file-undo-operations.c - Manages undo/redo of file operations
 *
 * Copyright (C) 2007-2011 Amos Brocco
 * Copyright (C) 2010, 2012 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors: Amos Brocco <amos.brocco@gmail.com>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#include <config.h>

#include "athena-file-undo-operations.h"

#include <glib/gi18n.h>

#include "athena-file-operations.h"
#include "athena-file.h"
#include "athena-file-undo-manager.h"

G_DEFINE_TYPE (AthenaFileUndoInfo, athena_file_undo_info, G_TYPE_OBJECT)

enum {
	PROP_OP_TYPE = 1,
	PROP_ITEM_COUNT,
	N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

struct _AthenaFileUndoInfoDetails {
	AthenaFileUndoOp op_type;
	guint count;		/* Number of items */

	GSimpleAsyncResult *apply_async_result;

	gchar *undo_label;
	gchar *redo_label;
	gchar *undo_description;
	gchar *redo_description;
};

/* description helpers */
static void
athena_file_undo_info_init (AthenaFileUndoInfo *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ATHENA_TYPE_FILE_UNDO_INFO,
						  AthenaFileUndoInfoDetails);
	self->priv->apply_async_result = NULL;
}

static void
athena_file_undo_info_get_property (GObject *object,
				      guint property_id,
				      GValue *value,
				      GParamSpec *pspec)
{
	AthenaFileUndoInfo *self = ATHENA_FILE_UNDO_INFO (object);

	switch (property_id) {
	case PROP_OP_TYPE:
		g_value_set_int (value, self->priv->op_type);
		break;
	case PROP_ITEM_COUNT:
		g_value_set_int (value, self->priv->count);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
athena_file_undo_info_set_property (GObject *object,
				      guint property_id,
				      const GValue *value,
				      GParamSpec *pspec)
{
	AthenaFileUndoInfo *self = ATHENA_FILE_UNDO_INFO (object);

	switch (property_id) {
	case PROP_OP_TYPE:
		self->priv->op_type = g_value_get_int (value);
		break;
	case PROP_ITEM_COUNT:
		self->priv->count = g_value_get_int (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
athena_file_redo_info_warn_redo (AthenaFileUndoInfo *self,
				   GtkWindow *parent_window)
{
	g_critical ("Object %p of type %s does not implement redo_func!!", 
		    self, G_OBJECT_TYPE_NAME (self));
}

static void
athena_file_undo_info_warn_undo (AthenaFileUndoInfo *self,
				   GtkWindow *parent_window)
{
	g_critical ("Object %p of type %s does not implement undo_func!!", 
		    self, G_OBJECT_TYPE_NAME (self));
}

static void
athena_file_undo_info_strings_func (AthenaFileUndoInfo *self,
				      gchar **undo_label,
				      gchar **undo_description,
				      gchar **redo_label,
				      gchar **redo_description)
{
	if (undo_label != NULL) {
		*undo_label = g_strdup (_("Undo"));
	}
	if (undo_description != NULL) {
		*undo_description = g_strdup (_("Undo last action"));
	}

	if (redo_label != NULL) {
		*redo_label = g_strdup (_("Redo"));
	}
	if (redo_description != NULL) {
		*redo_description = g_strdup (_("Redo last undone action"));
	}
}

static void
athena_file_undo_info_finalize (GObject *obj)
{
	AthenaFileUndoInfo *self = ATHENA_FILE_UNDO_INFO (obj);

	g_clear_object (&self->priv->apply_async_result);

	G_OBJECT_CLASS (athena_file_undo_info_parent_class)->finalize (obj);
}

static void
athena_file_undo_info_class_init (AthenaFileUndoInfoClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);

	oclass->finalize = athena_file_undo_info_finalize;
	oclass->get_property = athena_file_undo_info_get_property;
	oclass->set_property = athena_file_undo_info_set_property;

	klass->undo_func = athena_file_undo_info_warn_undo;
	klass->redo_func = athena_file_redo_info_warn_redo;
	klass->strings_func = athena_file_undo_info_strings_func;

	properties[PROP_OP_TYPE] =
		g_param_spec_int ("op-type",
				  "Undo info op type",
				  "Type of undo operation",
				  0, ATHENA_FILE_UNDO_OP_NUM_TYPES - 1, 0,
				  G_PARAM_READWRITE |
				  G_PARAM_CONSTRUCT_ONLY);
	properties[PROP_ITEM_COUNT] =
		g_param_spec_int ("item-count",
				  "Number of items",
				  "Number of items",
				  0, G_MAXINT, 0,
				  G_PARAM_READWRITE |
				  G_PARAM_CONSTRUCT_ONLY);

	g_type_class_add_private (klass, sizeof (AthenaFileUndoInfoDetails));
	g_object_class_install_properties (oclass, N_PROPERTIES, properties);
}

static AthenaFileUndoOp
athena_file_undo_info_get_op_type (AthenaFileUndoInfo *self)
{
	return self->priv->op_type;
}

static gint
athena_file_undo_info_get_item_count (AthenaFileUndoInfo *self)
{
	return self->priv->count;
}

void
athena_file_undo_info_apply_async (AthenaFileUndoInfo *self,
				     gboolean undo,
				     GtkWindow *parent_window,
				     GAsyncReadyCallback callback,
				     gpointer user_data)
{
	g_assert (self->priv->apply_async_result == NULL);

	self->priv->apply_async_result = 
		g_simple_async_result_new (G_OBJECT (self),
					   callback, user_data,
					   athena_file_undo_info_apply_async);

	if (undo) {
		ATHENA_FILE_UNDO_INFO_CLASS (G_OBJECT_GET_CLASS (self))->undo_func (self, parent_window);
	} else {
		ATHENA_FILE_UNDO_INFO_CLASS (G_OBJECT_GET_CLASS (self))->redo_func (self, parent_window);
	}
}

typedef struct {
	gboolean success;
	gboolean user_cancel;
} FileUndoInfoOpRes;

static void
file_undo_info_op_res_free (gpointer data)
{
	g_slice_free (FileUndoInfoOpRes, data);
}

gboolean
athena_file_undo_info_apply_finish (AthenaFileUndoInfo *self,
				      GAsyncResult *res,
				      gboolean *user_cancel,
				      GError **error)
{
	FileUndoInfoOpRes *op_res;

	if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error)) {
		return FALSE;
	}

	op_res = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (res));
	*user_cancel = op_res->user_cancel;

	return op_res->success;
}

void
athena_file_undo_info_get_strings (AthenaFileUndoInfo *self,
				     gchar **undo_label,
				     gchar **undo_description,
				     gchar **redo_label,
				     gchar **redo_description)
{
	return ATHENA_FILE_UNDO_INFO_CLASS (G_OBJECT_GET_CLASS (self))->strings_func (self,
											undo_label, undo_description,
											redo_label, redo_description);
}

static void
file_undo_info_complete_apply (AthenaFileUndoInfo *self,
			       gboolean success,
			       gboolean user_cancel)
{
	FileUndoInfoOpRes *op_res = g_slice_new0 (FileUndoInfoOpRes);

	op_res->user_cancel = user_cancel;
	op_res->success = success;


	g_simple_async_result_set_op_res_gpointer (self->priv->apply_async_result, op_res,
						   file_undo_info_op_res_free);
	g_simple_async_result_complete_in_idle (self->priv->apply_async_result);

	g_clear_object (&self->priv->apply_async_result);
}

static void
file_undo_info_transfer_callback (GHashTable * debuting_uris,
				  gboolean success,
                                  gpointer user_data)
{
	AthenaFileUndoInfo *self = user_data;

	/* TODO: we need to forward the cancelled state from 
	 * the file operation to the file undo info object.
	 */
	file_undo_info_complete_apply (self, success, FALSE);
}

static void
file_undo_info_operation_callback (AthenaFile * file,
				   GFile * result_location,
				   GError * error,
				   gpointer user_data)
{
	AthenaFileUndoInfo *self = user_data;

	file_undo_info_complete_apply (self, (error == NULL),
				       g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED));
}

static void
file_undo_info_delete_callback (GHashTable *debuting_uris,
                                gboolean user_cancel,
                                gpointer user_data)
{
	AthenaFileUndoInfo *self = user_data;

	file_undo_info_complete_apply (self,
				       !user_cancel,
				       user_cancel);
}

/* copy/move/duplicate/link/restore from trash */
G_DEFINE_TYPE (AthenaFileUndoInfoExt, athena_file_undo_info_ext, ATHENA_TYPE_FILE_UNDO_INFO)

struct _AthenaFileUndoInfoExtDetails {
	GFile *src_dir;
	GFile *dest_dir;
	GList *sources;	     /* Relative to src_dir */
	GList *destinations; /* Relative to dest_dir */
};

static char *
ext_get_first_target_short_name (AthenaFileUndoInfoExt *self)
{
	GList *targets_first;
	char *file_name = NULL;

	targets_first = g_list_first (self->priv->destinations);

	if (targets_first != NULL &&
	    targets_first->data != NULL) {
		file_name = g_file_get_basename (targets_first->data);
	}

	return file_name;
}

static void
ext_strings_func (AthenaFileUndoInfo *info,
		  gchar **undo_label,
		  gchar **undo_description,
		  gchar **redo_label,
		  gchar **redo_description)
{
	AthenaFileUndoInfoExt *self = ATHENA_FILE_UNDO_INFO_EXT (info);
	AthenaFileUndoOp op_type = athena_file_undo_info_get_op_type (info);
	gint count = athena_file_undo_info_get_item_count (info);
	gchar *name = NULL, *source, *destination;

	source = g_file_get_path (self->priv->src_dir);
	destination = g_file_get_path (self->priv->dest_dir);

	if (count <= 1) {
		name = ext_get_first_target_short_name (self);
	}

	if (op_type == ATHENA_FILE_UNDO_OP_MOVE) {
		if (count > 1) {
			*undo_description = g_strdup_printf (ngettext ("Move %d item back to '%s'",
								       "Move %d items back to '%s'", count),
							     count, source);
			*redo_description = g_strdup_printf (ngettext ("Move %d item to '%s'",
								       "Move %d items to '%s'", count),
							     count, destination);

			*undo_label = g_strdup_printf (ngettext ("_Undo Move %d item",
								 "_Undo Move %d items", count),
						       count);
			*redo_label = g_strdup_printf (ngettext ("_Redo Move %d item",
								 "_Redo Move %d items", count),
						       count);
		} else {
			*undo_description = g_strdup_printf (_("Move '%s' back to '%s'"), name, source);
			*redo_description = g_strdup_printf (_("Move '%s' to '%s'"), name, destination);

			*undo_label = g_strdup (_("_Undo Move"));
			*redo_label = g_strdup (_("_Redo Move"));
		}
	} else if (op_type == ATHENA_FILE_UNDO_OP_RESTORE_FROM_TRASH)  {
		*undo_label = g_strdup (_("_Undo Restore from Trash"));
		*redo_label = g_strdup (_("_Redo Restore from Trash"));

		if (count > 1) {
			*undo_description = g_strdup_printf (ngettext ("Move %d item back to trash",
								       "Move %d items back to trash", count),
							     count);
			*redo_description = g_strdup_printf (ngettext ("Restore %d item from trash",
								       "Restore %d items from trash", count),
							     count);
		} else {
			*undo_description = g_strdup_printf (_("Move '%s' back to trash"), name);
			*redo_description = g_strdup_printf (_("Restore '%s' from trash"), name);
		}
	} else if (op_type == ATHENA_FILE_UNDO_OP_COPY) {
		if (count > 1) {
			*undo_description = g_strdup_printf (ngettext ("Delete %d copied item",
								       "Delete %d copied items", count),
							     count);
			*redo_description = g_strdup_printf (ngettext ("Copy %d item to '%s'",
								       "Copy %d items to '%s'", count),
							     count, destination);

			*undo_label = g_strdup_printf (ngettext ("_Undo Copy %d item",
								 "_Undo Copy %d items", count),
						       count);
			*redo_label = g_strdup_printf (ngettext ("_Redo Copy %d item",
								 "_Redo Copy %d items", count),
						       count);
		} else {
			*undo_description = g_strdup_printf (_("Delete '%s'"), name);
			*redo_description = g_strdup_printf (_("Copy '%s' to '%s'"), name, destination);

			*undo_label = g_strdup (_("_Undo Copy"));
			*redo_label = g_strdup (_("_Redo Copy"));
		}
	} else if (op_type == ATHENA_FILE_UNDO_OP_DUPLICATE) {
		if (count > 1) {
			*undo_description = g_strdup_printf (ngettext ("Delete %d duplicated item",
								       "Delete %d duplicated items", count),
							     count);
			*redo_description = g_strdup_printf (ngettext ("Duplicate %d item in '%s'",
								       "Duplicate %d items in '%s'", count),
							     count, destination);

			*undo_label = g_strdup_printf (ngettext ("_Undo Duplicate %d item",
								 "_Undo Duplicate %d items", count),
						       count);
			*redo_label = g_strdup_printf (ngettext ("_Redo Duplicate %d item",
								 "_Redo Duplicate %d items", count),
						       count);
		} else {
			*undo_description = g_strdup_printf (_("Delete '%s'"), name);
			*redo_description = g_strdup_printf (_("Duplicate '%s' in '%s'"),
							   name, destination);

			*undo_label = g_strdup (_("_Undo Duplicate"));
			*redo_label = g_strdup (_("_Redo Duplicate"));
		}
	} else if (op_type == ATHENA_FILE_UNDO_OP_CREATE_LINK) {
		if (count > 1) {
			*undo_description = g_strdup_printf (ngettext ("Delete links to %d item",
								       "Delete links to %d items", count),
							     count);
			*redo_description = g_strdup_printf (ngettext ("Create links to %d item",
								       "Create links to %d items", count),
							     count);
		} else {
			*undo_description = g_strdup_printf (_("Delete link to '%s'"), name);
			*redo_description = g_strdup_printf (_("Create link to '%s'"), name);

			*undo_label = g_strdup (_("_Undo Create Link"));
			*redo_label = g_strdup (_("_Redo Create Link"));
		}
	} else {
		g_assert_not_reached ();
	}

	g_free (name);
	g_free (source);
	g_free (destination);
}

static void
ext_create_link_redo_func (AthenaFileUndoInfoExt *self,
			   GtkWindow *parent_window)
{
	athena_file_operations_link (self->priv->sources, NULL,
				       self->priv->dest_dir, parent_window,
				       file_undo_info_transfer_callback, self);
}

static void
ext_duplicate_redo_func (AthenaFileUndoInfoExt *self,
			 GtkWindow *parent_window)
{
	athena_file_operations_duplicate (self->priv->sources, NULL, parent_window,
					    file_undo_info_transfer_callback, self);
}

static void
ext_copy_redo_func (AthenaFileUndoInfoExt *self,
		    GtkWindow *parent_window)
{
	athena_file_operations_copy (self->priv->sources, NULL,
				       self->priv->dest_dir, parent_window,
				       file_undo_info_transfer_callback, self);
}

static void
ext_move_restore_redo_func (AthenaFileUndoInfoExt *self,
			    GtkWindow *parent_window)
{
	athena_file_operations_move (self->priv->sources, NULL,
				       self->priv->dest_dir, parent_window,
				       file_undo_info_transfer_callback, self);
}

static void
ext_redo_func (AthenaFileUndoInfo *info,
	       GtkWindow *parent_window)
{
	AthenaFileUndoInfoExt *self = ATHENA_FILE_UNDO_INFO_EXT (info);
	AthenaFileUndoOp op_type = athena_file_undo_info_get_op_type (info);

	if (op_type == ATHENA_FILE_UNDO_OP_MOVE ||
	    op_type == ATHENA_FILE_UNDO_OP_RESTORE_FROM_TRASH)  {
		ext_move_restore_redo_func (self, parent_window);
	} else if (op_type == ATHENA_FILE_UNDO_OP_COPY) {
		ext_copy_redo_func (self, parent_window);
	} else if (op_type == ATHENA_FILE_UNDO_OP_DUPLICATE) {
		ext_duplicate_redo_func (self, parent_window);
	} else if (op_type == ATHENA_FILE_UNDO_OP_CREATE_LINK) {
		ext_create_link_redo_func (self, parent_window);
	} else {
		g_assert_not_reached ();
	}
}

static void
ext_restore_undo_func (AthenaFileUndoInfoExt *self,
		       GtkWindow *parent_window)
{
	athena_file_operations_trash_or_delete (self->priv->destinations, parent_window,
						  file_undo_info_delete_callback, self);
}


static void
ext_move_undo_func (AthenaFileUndoInfoExt *self,
		    GtkWindow *parent_window)
{
	athena_file_operations_move (self->priv->destinations, NULL,
				       self->priv->src_dir, parent_window,
				       file_undo_info_transfer_callback, self);
}

static void
ext_copy_duplicate_undo_func (AthenaFileUndoInfoExt *self,
			      GtkWindow *parent_window)
{
	GList *files;

	files = g_list_copy (self->priv->destinations);
	files = g_list_reverse (files); /* Deleting must be done in reverse */

	athena_file_operations_delete (files, parent_window,
					 file_undo_info_delete_callback, self);

	g_list_free (files);
}

static void
ext_undo_func (AthenaFileUndoInfo *info,
	       GtkWindow *parent_window)
{
	AthenaFileUndoInfoExt *self = ATHENA_FILE_UNDO_INFO_EXT (info);
	AthenaFileUndoOp op_type = athena_file_undo_info_get_op_type (info);

	if (op_type == ATHENA_FILE_UNDO_OP_COPY ||
	    op_type == ATHENA_FILE_UNDO_OP_DUPLICATE ||
	    op_type == ATHENA_FILE_UNDO_OP_CREATE_LINK) {
		ext_copy_duplicate_undo_func (self, parent_window);
	} else if (op_type == ATHENA_FILE_UNDO_OP_MOVE) {
		ext_move_undo_func (self, parent_window);
	} else if (op_type == ATHENA_FILE_UNDO_OP_RESTORE_FROM_TRASH) {
		ext_restore_undo_func (self, parent_window);
	} else {
		g_assert_not_reached ();
	}
}

static void
athena_file_undo_info_ext_init (AthenaFileUndoInfoExt *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, athena_file_undo_info_ext_get_type (),
						  AthenaFileUndoInfoExtDetails);
}

static void
athena_file_undo_info_ext_finalize (GObject *obj)
{
	AthenaFileUndoInfoExt *self = ATHENA_FILE_UNDO_INFO_EXT (obj);

	if (self->priv->sources) {
		g_list_free_full (self->priv->sources, g_object_unref);
	}

	if (self->priv->destinations) {
		g_list_free_full (self->priv->destinations, g_object_unref);
	}

	g_clear_object (&self->priv->src_dir);
	g_clear_object (&self->priv->dest_dir);

	G_OBJECT_CLASS (athena_file_undo_info_ext_parent_class)->finalize (obj);
}

static void
athena_file_undo_info_ext_class_init (AthenaFileUndoInfoExtClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);
	AthenaFileUndoInfoClass *iclass = ATHENA_FILE_UNDO_INFO_CLASS (klass);

	oclass->finalize = athena_file_undo_info_ext_finalize;

	iclass->undo_func = ext_undo_func;
	iclass->redo_func = ext_redo_func;
	iclass->strings_func = ext_strings_func;

	g_type_class_add_private (klass, sizeof (AthenaFileUndoInfoExtDetails));
}

AthenaFileUndoInfo *
athena_file_undo_info_ext_new (AthenaFileUndoOp op_type,
				 gint item_count,
				 GFile *src_dir,
				 GFile *target_dir)
{
	AthenaFileUndoInfoExt *retval;

	retval = g_object_new (ATHENA_TYPE_FILE_UNDO_INFO_EXT,
			       "op-type", op_type,
			       "item-count", item_count,
			       NULL);

	retval->priv->src_dir = g_object_ref (src_dir);
	retval->priv->dest_dir = g_object_ref (target_dir);

	return ATHENA_FILE_UNDO_INFO (retval);
}

void
athena_file_undo_info_ext_add_origin_target_pair (AthenaFileUndoInfoExt *self,
						    GFile                   *origin,
						    GFile                   *target)
{
	self->priv->sources =
		g_list_append (self->priv->sources, g_object_ref (origin));
	self->priv->destinations =
		g_list_append (self->priv->destinations, g_object_ref (target));
}

/* create new file/folder */
G_DEFINE_TYPE (AthenaFileUndoInfoCreate, athena_file_undo_info_create, ATHENA_TYPE_FILE_UNDO_INFO)

struct _AthenaFileUndoInfoCreateDetails {
	char *template;
	GFile *target_file;
	gint length;
};

static void
create_strings_func (AthenaFileUndoInfo *info,
		     gchar **undo_label,
		     gchar **undo_description,
		     gchar **redo_label,
		     gchar **redo_description)

{
	AthenaFileUndoInfoCreate *self = ATHENA_FILE_UNDO_INFO_CREATE (info);
	AthenaFileUndoOp op_type = athena_file_undo_info_get_op_type (info);
	char *name;

	name = g_file_get_parse_name (self->priv->target_file);
	*undo_description = g_strdup_printf (_("Delete '%s'"), name);

	if (op_type == ATHENA_FILE_UNDO_OP_CREATE_EMPTY_FILE) {
		*redo_description = g_strdup_printf (_("Create an empty file '%s'"), name);

		*undo_label = g_strdup (_("_Undo Create Empty File"));
		*redo_label = g_strdup (_("_Redo Create Empty File"));
	} else if (op_type == ATHENA_FILE_UNDO_OP_CREATE_FOLDER) {
		*redo_description = g_strdup_printf (_("Create a new folder '%s'"), name);

		*undo_label = g_strdup (_("_Undo Create Folder"));
		*redo_label = g_strdup (_("_Redo Create Folder"));
	} else if (op_type == ATHENA_FILE_UNDO_OP_CREATE_FILE_FROM_TEMPLATE) {
		*redo_description = g_strdup_printf (_("Create new file '%s' from template "), name);

		*undo_label = g_strdup (_("_Undo Create from Template"));
		*redo_label = g_strdup (_("_Redo Create from Template"));
	} else {
		g_assert_not_reached ();
	}

	g_free (name);
}

static void
create_callback (GFile * new_file,
		 gboolean success,
		 gpointer callback_data)
{
	file_undo_info_transfer_callback (NULL, success, callback_data);
}

static void
create_from_template_redo_func (AthenaFileUndoInfoCreate *self,
				GtkWindow *parent_window)
{
	GFile *parent;
	gchar *parent_uri, *new_name;

	parent = g_file_get_parent (self->priv->target_file);
	parent_uri = g_file_get_uri (parent);
	new_name = g_file_get_parse_name (self->priv->target_file);
	athena_file_operations_new_file_from_template (NULL, NULL,
							 parent_uri, new_name,
							 self->priv->template,
							 create_callback, self);

	g_free (parent_uri);
	g_free (new_name);
	g_object_unref (parent);
}

static void
create_folder_redo_func (AthenaFileUndoInfoCreate *self,
			 GtkWindow *parent_window)
{
	GFile *parent;
	gchar *parent_uri;

	parent = g_file_get_parent (self->priv->target_file);
	parent_uri = g_file_get_uri (parent);
	athena_file_operations_new_folder (NULL, NULL, parent_uri,
					     create_callback, self);

	g_free (parent_uri);
	g_object_unref (parent);
}

static void
create_empty_redo_func (AthenaFileUndoInfoCreate *self,
			GtkWindow *parent_window)

{
	GFile *parent;
	gchar *parent_uri;
	gchar *new_name;

	parent = g_file_get_parent (self->priv->target_file);
	parent_uri = g_file_get_uri (parent);
	new_name = g_file_get_parse_name (self->priv->target_file);
	athena_file_operations_new_file (NULL, NULL, parent_uri,
					   new_name,
					   self->priv->template,
					   self->priv->length,
					   create_callback, self);

	g_free (parent_uri);
	g_free (new_name);
	g_object_unref (parent);
}

static void
create_redo_func (AthenaFileUndoInfo *info,
		  GtkWindow *parent_window)
{
	AthenaFileUndoInfoCreate *self = ATHENA_FILE_UNDO_INFO_CREATE (info);
	AthenaFileUndoOp op_type = athena_file_undo_info_get_op_type (info);

	if (op_type == ATHENA_FILE_UNDO_OP_CREATE_EMPTY_FILE) {
		create_empty_redo_func (self, parent_window);
	} else if (op_type == ATHENA_FILE_UNDO_OP_CREATE_FOLDER) {
		create_folder_redo_func (self, parent_window);
	} else if (op_type == ATHENA_FILE_UNDO_OP_CREATE_FILE_FROM_TEMPLATE) {
		create_from_template_redo_func (self, parent_window);
	} else {
		g_assert_not_reached ();
	}
}

static void
create_undo_func (AthenaFileUndoInfo *info,
		  GtkWindow *parent_window)
{
	AthenaFileUndoInfoCreate *self = ATHENA_FILE_UNDO_INFO_CREATE (info);
	GList *files = NULL;

	files = g_list_append (files, g_object_ref (self->priv->target_file));
	athena_file_operations_delete (files, parent_window,
					 file_undo_info_delete_callback, self);

	g_list_free_full (files, g_object_unref);
}

static void
athena_file_undo_info_create_init (AthenaFileUndoInfoCreate *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, athena_file_undo_info_create_get_type (),
						  AthenaFileUndoInfoCreateDetails);
}

static void
athena_file_undo_info_create_finalize (GObject *obj)
{
	AthenaFileUndoInfoCreate *self = ATHENA_FILE_UNDO_INFO_CREATE (obj);
	g_clear_object (&self->priv->target_file);
	g_free (self->priv->template);	

	G_OBJECT_CLASS (athena_file_undo_info_create_parent_class)->finalize (obj);
}

static void
athena_file_undo_info_create_class_init (AthenaFileUndoInfoCreateClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);
	AthenaFileUndoInfoClass *iclass = ATHENA_FILE_UNDO_INFO_CLASS (klass);

	oclass->finalize = athena_file_undo_info_create_finalize;

	iclass->undo_func = create_undo_func;
	iclass->redo_func = create_redo_func;
	iclass->strings_func = create_strings_func;

	g_type_class_add_private (klass, sizeof (AthenaFileUndoInfoCreateDetails));
}

AthenaFileUndoInfo *
athena_file_undo_info_create_new (AthenaFileUndoOp op_type)
{
	return g_object_new (ATHENA_TYPE_FILE_UNDO_INFO_CREATE,
			     "op-type", op_type,
			     "item-count", 1,
			     NULL);
}

void
athena_file_undo_info_create_set_data (AthenaFileUndoInfoCreate *self,
                                         GFile                      *file,
                                         const char                 *template,
					 gint                        length)
{
	self->priv->target_file = g_object_ref (file);
	self->priv->template = g_strdup (template);
	self->priv->length = length;
}

/* rename */
G_DEFINE_TYPE (AthenaFileUndoInfoRename, athena_file_undo_info_rename, ATHENA_TYPE_FILE_UNDO_INFO)

struct _AthenaFileUndoInfoRenameDetails {
	GFile *old_file;
	GFile *new_file;
};

static void
rename_strings_func (AthenaFileUndoInfo *info,
		     gchar **undo_label,
		     gchar **undo_description,
		     gchar **redo_label,
		     gchar **redo_description)
{
	AthenaFileUndoInfoRename *self = ATHENA_FILE_UNDO_INFO_RENAME (info);
	gchar *new_name, *old_name;

	new_name = g_file_get_parse_name (self->priv->new_file);
	old_name = g_file_get_parse_name (self->priv->old_file);

	*undo_description = g_strdup_printf (_("Rename '%s' as '%s'"), new_name, old_name);
	*redo_description = g_strdup_printf (_("Rename '%s' as '%s'"), old_name, new_name);

	*undo_label = g_strdup (_("_Undo Rename"));
	*redo_label = g_strdup (_("_Redo Rename"));

	g_free (old_name);
	g_free (new_name);
}

static void
rename_redo_func (AthenaFileUndoInfo *info,
		  GtkWindow *parent_window)
{
	AthenaFileUndoInfoRename *self = ATHENA_FILE_UNDO_INFO_RENAME (info);
	gchar *new_name;
	AthenaFile *file;

	new_name = g_file_get_basename (self->priv->new_file);
	file = athena_file_get (self->priv->old_file);
	athena_file_rename (file, new_name,
			      file_undo_info_operation_callback, self);

	athena_file_unref (file);
	g_free (new_name);
}

static void
rename_undo_func (AthenaFileUndoInfo *info,
		  GtkWindow *parent_window)
{
	AthenaFileUndoInfoRename *self = ATHENA_FILE_UNDO_INFO_RENAME (info);
	gchar *new_name;
	AthenaFile *file;

	new_name = g_file_get_basename (self->priv->old_file);
	file = athena_file_get (self->priv->new_file);
	athena_file_rename (file, new_name,
			      file_undo_info_operation_callback, self);

	athena_file_unref (file);
	g_free (new_name);
}

static void
athena_file_undo_info_rename_init (AthenaFileUndoInfoRename *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, athena_file_undo_info_rename_get_type (),
						  AthenaFileUndoInfoRenameDetails);
}

static void
athena_file_undo_info_rename_finalize (GObject *obj)
{
	AthenaFileUndoInfoRename *self = ATHENA_FILE_UNDO_INFO_RENAME (obj);
	g_clear_object (&self->priv->old_file);
	g_clear_object (&self->priv->new_file);

	G_OBJECT_CLASS (athena_file_undo_info_rename_parent_class)->finalize (obj);
}

static void
athena_file_undo_info_rename_class_init (AthenaFileUndoInfoRenameClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);
	AthenaFileUndoInfoClass *iclass = ATHENA_FILE_UNDO_INFO_CLASS (klass);

	oclass->finalize = athena_file_undo_info_rename_finalize;

	iclass->undo_func = rename_undo_func;
	iclass->redo_func = rename_redo_func;
	iclass->strings_func = rename_strings_func;

	g_type_class_add_private (klass, sizeof (AthenaFileUndoInfoRenameDetails));
}

AthenaFileUndoInfo *
athena_file_undo_info_rename_new (void)
{
	return g_object_new (ATHENA_TYPE_FILE_UNDO_INFO_RENAME,
			     "op-type", ATHENA_FILE_UNDO_OP_RENAME,
			     "item-count", 1,
			     NULL);
}

void
athena_file_undo_info_rename_set_data (AthenaFileUndoInfoRename *self,
					 GFile                      *old_file,
					 GFile                      *new_file)
{
	self->priv->old_file = g_object_ref (old_file);
	self->priv->new_file = g_object_ref (new_file);
}

/* trash */
G_DEFINE_TYPE (AthenaFileUndoInfoTrash, athena_file_undo_info_trash, ATHENA_TYPE_FILE_UNDO_INFO)

struct _AthenaFileUndoInfoTrashDetails {
	GHashTable *trashed;
};

static void
trash_strings_func (AthenaFileUndoInfo *info,
		    gchar **undo_label,
		    gchar **undo_description,
		    gchar **redo_label,
		    gchar **redo_description)
{
	AthenaFileUndoInfoTrash *self = ATHENA_FILE_UNDO_INFO_TRASH (info);
	gint count = g_hash_table_size (self->priv->trashed);

	if (count != 1) {
		*undo_description = g_strdup_printf (ngettext ("Restore %d item from trash",
							       "Restore %d items from trash", count),
						     count);
		*redo_description = g_strdup_printf (ngettext ("Move %d item to trash",
							       "Move %d items to trash", count),
						     count);
	} else {
		GList *keys;
		char *name, *orig_path;
		GFile *file;

		keys = g_hash_table_get_keys (self->priv->trashed);
		file = keys->data;
		name = g_file_get_basename (file);
		orig_path = g_file_get_path (file);
		*undo_description = g_strdup_printf (_("Restore '%s' to '%s'"), name, orig_path);

		g_free (name);
		g_free (orig_path);
		g_list_free (keys);

		name = g_file_get_parse_name (file);
		*redo_description = g_strdup_printf (_("Move '%s' to trash"), name);

		g_free (name);

		*undo_label = g_strdup (_("_Undo Trash"));
		*redo_label = g_strdup (_("_Redo Trash"));
	}
}

static void
trash_redo_func_callback (GHashTable *debuting_uris,
			  gboolean user_cancel,
			  gpointer user_data)
{
	AthenaFileUndoInfoTrash *self = user_data;
	GHashTable *new_trashed_files;
	GTimeVal current_time;
	gsize updated_trash_time;
	GFile *file;
	GList *keys, *l;

	if (!user_cancel) {
		new_trashed_files = 
			g_hash_table_new_full (g_file_hash, (GEqualFunc) g_file_equal, 
					       g_object_unref, NULL);

		keys = g_hash_table_get_keys (self->priv->trashed);

		g_get_current_time (&current_time);
		updated_trash_time = current_time.tv_sec;

		for (l = keys; l != NULL; l = l->next) {
			file = l->data;
			g_hash_table_insert (new_trashed_files,
					     g_object_ref (file), GSIZE_TO_POINTER (updated_trash_time));
		}

		g_list_free (keys);
		g_hash_table_destroy (self->priv->trashed);

		self->priv->trashed = new_trashed_files;
	}

	file_undo_info_delete_callback (debuting_uris, user_cancel, user_data);
}

static void
trash_redo_func (AthenaFileUndoInfo *info,
		 GtkWindow *parent_window)
{
	AthenaFileUndoInfoTrash *self = ATHENA_FILE_UNDO_INFO_TRASH (info);

	if (g_hash_table_size (self->priv->trashed) > 0) {
		GList *locations;

		locations = g_hash_table_get_keys (self->priv->trashed);
		athena_file_operations_trash_or_delete (locations, parent_window,
							  trash_redo_func_callback, self);

		g_list_free (locations);
	}
}

static GHashTable *
trash_retrieve_files_to_restore_finish (AthenaFileUndoInfoTrash *self,
					GAsyncResult *res,
					GError **error)
{
	GHashTable *retval = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (res));

	if (retval == NULL) {
		g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error);
	}

	return retval;
}

static void
trash_retrieve_files_to_restore_thread (GSimpleAsyncResult *res,
					GObject *object,
					GCancellable *cancellable)
{
	AthenaFileUndoInfoTrash *self = ATHENA_FILE_UNDO_INFO_TRASH (object);
	GFileEnumerator *enumerator;
	GHashTable *to_restore;
	GFile *trash;
	GError *error = NULL;

	to_restore = g_hash_table_new_full (g_file_hash, (GEqualFunc) g_file_equal, 
					    g_object_unref, g_object_unref);

	trash = g_file_new_for_uri ("trash:///");

	enumerator = g_file_enumerate_children (trash,
			G_FILE_ATTRIBUTE_STANDARD_NAME","
			G_FILE_ATTRIBUTE_TRASH_DELETION_DATE","
			G_FILE_ATTRIBUTE_TRASH_ORIG_PATH,
			G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
			NULL, &error);

	if (enumerator) {
		GFileInfo *info;
		gpointer lookupvalue;
		GFile *item;
		GTimeVal timeval;
		glong trash_time, orig_trash_time;
		const char *origpath;
		GFile *origfile;
		const char *time_string;

		while ((info = g_file_enumerator_next_file (enumerator, NULL, &error)) != NULL) {
			/* Retrieve the original file uri */
			origpath = g_file_info_get_attribute_byte_string (info, G_FILE_ATTRIBUTE_TRASH_ORIG_PATH);
			origfile = g_file_new_for_path (origpath);

			lookupvalue = g_hash_table_lookup (self->priv->trashed, origfile);

			if (lookupvalue) {
				orig_trash_time = GPOINTER_TO_SIZE (lookupvalue);
				time_string = g_file_info_get_attribute_string (info, G_FILE_ATTRIBUTE_TRASH_DELETION_DATE);
				if (time_string != NULL) {
					g_time_val_from_iso8601 (time_string, &timeval);
					trash_time = timeval.tv_sec;
				} else {
					trash_time = 0;
				}

				if (trash_time == orig_trash_time) {
					/* File in the trash */
					item = g_file_get_child (trash, g_file_info_get_name (info));
					g_hash_table_insert (to_restore, item, g_object_ref (origfile));
				}
			}

			g_object_unref (origfile);

		}
		g_file_enumerator_close (enumerator, FALSE, NULL);
		g_object_unref (enumerator);
	}
	g_object_unref (trash);

	if (error != NULL) {
		g_simple_async_result_take_error (res, error);
		g_hash_table_destroy (to_restore);
	} else {
		g_simple_async_result_set_op_res_gpointer (res, to_restore, NULL);
	}
}

static void
trash_retrieve_files_to_restore_async (AthenaFileUndoInfoTrash *self,
				 GAsyncReadyCallback callback,
				 gpointer user_data)
{
	GSimpleAsyncResult *async_op;

	async_op = g_simple_async_result_new (G_OBJECT (self), callback, user_data,
					      trash_retrieve_files_to_restore_async);
	g_simple_async_result_run_in_thread (async_op, trash_retrieve_files_to_restore_thread,
					     G_PRIORITY_DEFAULT, NULL);

	g_object_unref (async_op);
}

static void
trash_retrieve_files_ready (GObject *source,
			    GAsyncResult *res,
			    gpointer user_data)
{
	AthenaFileUndoInfoTrash *self = ATHENA_FILE_UNDO_INFO_TRASH (source);
	GHashTable *files_to_restore;
	GError *error = NULL;

	files_to_restore = trash_retrieve_files_to_restore_finish (self, res, &error);

	if (error == NULL && g_hash_table_size (files_to_restore) > 0) {
		GList *gfiles_in_trash, *l;
		GFile *item;
		GFile *dest;

		gfiles_in_trash = g_hash_table_get_keys (files_to_restore);

		for (l = gfiles_in_trash; l != NULL; l = l->next) {
			item = l->data;
			dest = g_hash_table_lookup (files_to_restore, item);

			g_file_move (item, dest, G_FILE_COPY_NOFOLLOW_SYMLINKS, NULL, NULL, NULL, NULL);
		}

		g_list_free (gfiles_in_trash);

		/* Here we must do what's necessary for the callback */
		file_undo_info_transfer_callback (NULL, (error == NULL), self);
	} else {
		file_undo_info_transfer_callback (NULL, FALSE, self);
	}

	if (files_to_restore != NULL) {
		g_hash_table_destroy (files_to_restore);
	}

	g_clear_error (&error);
}

static void
trash_undo_func (AthenaFileUndoInfo *info,
		 GtkWindow *parent_window)
{
	AthenaFileUndoInfoTrash *self = ATHENA_FILE_UNDO_INFO_TRASH (info);

	/* Internally managed op, pop flag. */
	athena_file_undo_manager_pop_flag ();

	trash_retrieve_files_to_restore_async (self, trash_retrieve_files_ready, NULL);
}

static void
athena_file_undo_info_trash_init (AthenaFileUndoInfoTrash *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, athena_file_undo_info_trash_get_type (),
						  AthenaFileUndoInfoTrashDetails);
	self->priv->trashed =
		g_hash_table_new_full (g_file_hash, (GEqualFunc) g_file_equal, 
				       g_object_unref, NULL);
}

static void
athena_file_undo_info_trash_finalize (GObject *obj)
{
	AthenaFileUndoInfoTrash *self = ATHENA_FILE_UNDO_INFO_TRASH (obj);
	g_hash_table_destroy (self->priv->trashed);

	G_OBJECT_CLASS (athena_file_undo_info_trash_parent_class)->finalize (obj);
}

static void
athena_file_undo_info_trash_class_init (AthenaFileUndoInfoTrashClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);
	AthenaFileUndoInfoClass *iclass = ATHENA_FILE_UNDO_INFO_CLASS (klass);

	oclass->finalize = athena_file_undo_info_trash_finalize;

	iclass->undo_func = trash_undo_func;
	iclass->redo_func = trash_redo_func;
	iclass->strings_func = trash_strings_func;

	g_type_class_add_private (klass, sizeof (AthenaFileUndoInfoTrashDetails));
}

AthenaFileUndoInfo *
athena_file_undo_info_trash_new (gint item_count)
{
	return g_object_new (ATHENA_TYPE_FILE_UNDO_INFO_TRASH,
			     "op-type", ATHENA_FILE_UNDO_OP_MOVE_TO_TRASH,
			     "item-count", item_count,
			     NULL);
}

void
athena_file_undo_info_trash_add_file (AthenaFileUndoInfoTrash *self,
					GFile                     *file)
{
	GTimeVal current_time;
	gsize orig_trash_time;

	g_get_current_time (&current_time);
	orig_trash_time = current_time.tv_sec;

	g_hash_table_insert (self->priv->trashed, g_object_ref (file), GSIZE_TO_POINTER (orig_trash_time));
}

/* recursive permissions */
G_DEFINE_TYPE (AthenaFileUndoInfoRecPermissions, athena_file_undo_info_rec_permissions, ATHENA_TYPE_FILE_UNDO_INFO)

struct _AthenaFileUndoInfoRecPermissionsDetails {
	GFile *dest_dir;
	GHashTable *original_permissions;
	guint32 dir_mask;
	guint32 dir_permissions;
	guint32 file_mask;
	guint32 file_permissions;
};

static void
rec_permissions_strings_func (AthenaFileUndoInfo *info,
			      gchar **undo_label,
			      gchar **undo_description,
			      gchar **redo_label,
			      gchar **redo_description)
{
	AthenaFileUndoInfoRecPermissions *self = ATHENA_FILE_UNDO_INFO_REC_PERMISSIONS (info);
	char *name;

	name = g_file_get_path (self->priv->dest_dir);

	*undo_description = g_strdup_printf (_("Restore original permissions of items enclosed in '%s'"), name);
	*redo_description = g_strdup_printf (_("Set permissions of items enclosed in '%s'"), name);

	*undo_label = g_strdup (_("_Undo Change Permissions"));
	*redo_label = g_strdup (_("_Redo Change Permissions"));

	g_free (name);
}

static void
rec_permissions_callback (gboolean success,
			  gpointer callback_data)
{
	file_undo_info_transfer_callback (NULL, success, callback_data);
}

static void
rec_permissions_redo_func (AthenaFileUndoInfo *info,
			   GtkWindow *parent_window)
{
	AthenaFileUndoInfoRecPermissions *self = ATHENA_FILE_UNDO_INFO_REC_PERMISSIONS (info);
	gchar *parent_uri;

	parent_uri = g_file_get_uri (self->priv->dest_dir);
	athena_file_set_permissions_recursive (parent_uri,
						 self->priv->file_permissions,
						 self->priv->file_mask,
						 self->priv->dir_permissions,
						 self->priv->dir_mask,
						 rec_permissions_callback, self);
	g_free (parent_uri);
}

static void
rec_permissions_undo_func (AthenaFileUndoInfo *info,
			   GtkWindow *parent_window)
{
	AthenaFileUndoInfoRecPermissions *self = ATHENA_FILE_UNDO_INFO_REC_PERMISSIONS (info);

	/* Internally managed op, pop flag. */
	/* TODO: why? */
	athena_file_undo_manager_pop_flag ();

	if (g_hash_table_size (self->priv->original_permissions) > 0) {
		GList *gfiles_list;
		guint32 perm;
		GList *l;
		GFile *dest;
		char *item;

		gfiles_list = g_hash_table_get_keys (self->priv->original_permissions);
		for (l = gfiles_list; l != NULL; l = l->next) {
			item = l->data;
			perm = GPOINTER_TO_UINT (g_hash_table_lookup (self->priv->original_permissions, item));
			dest = g_file_new_for_uri (item);
			g_file_set_attribute_uint32 (dest,
						     G_FILE_ATTRIBUTE_UNIX_MODE,
						     perm, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, NULL);
			g_object_unref (dest);
		}

		g_list_free (gfiles_list);
		/* Here we must do what's necessary for the callback */
		file_undo_info_transfer_callback (NULL, TRUE, self);
	}
}

static void
athena_file_undo_info_rec_permissions_init (AthenaFileUndoInfoRecPermissions *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, athena_file_undo_info_rec_permissions_get_type (),
						  AthenaFileUndoInfoRecPermissionsDetails);

	self->priv->original_permissions =
		g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
}

static void
athena_file_undo_info_rec_permissions_finalize (GObject *obj)
{
	AthenaFileUndoInfoRecPermissions *self = ATHENA_FILE_UNDO_INFO_REC_PERMISSIONS (obj);

	g_hash_table_destroy (self->priv->original_permissions);
	g_clear_object (&self->priv->dest_dir);

	G_OBJECT_CLASS (athena_file_undo_info_rec_permissions_parent_class)->finalize (obj);
}

static void
athena_file_undo_info_rec_permissions_class_init (AthenaFileUndoInfoRecPermissionsClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);
	AthenaFileUndoInfoClass *iclass = ATHENA_FILE_UNDO_INFO_CLASS (klass);

	oclass->finalize = athena_file_undo_info_rec_permissions_finalize;

	iclass->undo_func = rec_permissions_undo_func;
	iclass->redo_func = rec_permissions_redo_func;
	iclass->strings_func = rec_permissions_strings_func;

	g_type_class_add_private (klass, sizeof (AthenaFileUndoInfoRecPermissionsDetails));
}

AthenaFileUndoInfo *
athena_file_undo_info_rec_permissions_new (GFile   *dest,
					     guint32 file_permissions,
					     guint32 file_mask,
					     guint32 dir_permissions,
					     guint32 dir_mask)
{
	AthenaFileUndoInfoRecPermissions *retval;

	retval = g_object_new (ATHENA_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS,
			       "op-type", ATHENA_FILE_UNDO_OP_RECURSIVE_SET_PERMISSIONS,
			       "item-count", 1,
			       NULL);

	retval->priv->dest_dir = g_object_ref (dest);
	retval->priv->file_permissions = file_permissions;
	retval->priv->file_mask = file_mask;
	retval->priv->dir_permissions = dir_permissions;
	retval->priv->dir_mask = dir_mask;

	return ATHENA_FILE_UNDO_INFO (retval);
}

void
athena_file_undo_info_rec_permissions_add_file (AthenaFileUndoInfoRecPermissions *self,
						  GFile                              *file,
						  guint32                             permission)
{
	gchar *original_uri = g_file_get_uri (file);
	g_hash_table_insert (self->priv->original_permissions, original_uri, GUINT_TO_POINTER (permission));
}

/* single file change permissions */
G_DEFINE_TYPE (AthenaFileUndoInfoPermissions, athena_file_undo_info_permissions, ATHENA_TYPE_FILE_UNDO_INFO)

struct _AthenaFileUndoInfoPermissionsDetails {
	GFile *target_file;
	guint32 current_permissions;
	guint32 new_permissions;
};

static void
permissions_strings_func (AthenaFileUndoInfo *info,
			  gchar **undo_label,
			  gchar **undo_description,
			  gchar **redo_label,
			  gchar **redo_description)
{
	AthenaFileUndoInfoPermissions *self = ATHENA_FILE_UNDO_INFO_PERMISSIONS (info);
	gchar *name;

	name = g_file_get_parse_name (self->priv->target_file);
	*undo_description = g_strdup_printf (_("Restore original permissions of '%s'"), name);
	*redo_description = g_strdup_printf (_("Set permissions of '%s'"), name);

	*undo_label = g_strdup (_("_Undo Change Permissions"));
	*redo_label = g_strdup (_("_Redo Change Permissions"));

	g_free (name);
}

static void
permissions_real_func (AthenaFileUndoInfoPermissions *self,
		       guint32 permissions)
{
	AthenaFile *file;

	file = athena_file_get (self->priv->target_file);
	athena_file_set_permissions (file, permissions,
				       file_undo_info_operation_callback, self);

	athena_file_unref (file);
}

static void
permissions_redo_func (AthenaFileUndoInfo *info, 
		       GtkWindow *parent_window)
{
	AthenaFileUndoInfoPermissions *self = ATHENA_FILE_UNDO_INFO_PERMISSIONS (info);
	permissions_real_func (self, self->priv->new_permissions);
}

static void
permissions_undo_func (AthenaFileUndoInfo *info, 
		       GtkWindow *parent_window)
{
	AthenaFileUndoInfoPermissions *self = ATHENA_FILE_UNDO_INFO_PERMISSIONS (info);
	permissions_real_func (self, self->priv->current_permissions);
}

static void
athena_file_undo_info_permissions_init (AthenaFileUndoInfoPermissions *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, athena_file_undo_info_permissions_get_type (),
						  AthenaFileUndoInfoPermissionsDetails);
}

static void
athena_file_undo_info_permissions_finalize (GObject *obj)
{
	AthenaFileUndoInfoPermissions *self = ATHENA_FILE_UNDO_INFO_PERMISSIONS (obj);
	g_clear_object (&self->priv->target_file);

	G_OBJECT_CLASS (athena_file_undo_info_permissions_parent_class)->finalize (obj);
}

static void
athena_file_undo_info_permissions_class_init (AthenaFileUndoInfoPermissionsClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);
	AthenaFileUndoInfoClass *iclass = ATHENA_FILE_UNDO_INFO_CLASS (klass);

	oclass->finalize = athena_file_undo_info_permissions_finalize;

	iclass->undo_func = permissions_undo_func;
	iclass->redo_func = permissions_redo_func;
	iclass->strings_func = permissions_strings_func;

	g_type_class_add_private (klass, sizeof (AthenaFileUndoInfoPermissionsDetails));
}

AthenaFileUndoInfo *
athena_file_undo_info_permissions_new (GFile   *file,
					 guint32  current_permissions,
					 guint32  new_permissions)
{
	AthenaFileUndoInfoPermissions *retval;

	retval = g_object_new (ATHENA_TYPE_FILE_UNDO_INFO_PERMISSIONS,
			     "op-type", ATHENA_FILE_UNDO_OP_SET_PERMISSIONS,
			     "item-count", 1,
			     NULL);

	retval->priv->target_file = g_object_ref (file);
	retval->priv->current_permissions = current_permissions;
	retval->priv->new_permissions = new_permissions;

	return ATHENA_FILE_UNDO_INFO (retval);
}

/* group and owner change */
G_DEFINE_TYPE (AthenaFileUndoInfoOwnership, athena_file_undo_info_ownership, ATHENA_TYPE_FILE_UNDO_INFO)

struct _AthenaFileUndoInfoOwnershipDetails {
	GFile *target_file;
	char *original_ownership;
	char *new_ownership;
};

static void
ownership_strings_func (AthenaFileUndoInfo *info,
			gchar **undo_label,
			gchar **undo_description,
			gchar **redo_label,
			gchar **redo_description)
{
	AthenaFileUndoInfoOwnership *self = ATHENA_FILE_UNDO_INFO_OWNERSHIP (info);
	AthenaFileUndoOp op_type = athena_file_undo_info_get_op_type (info);
	gchar *name;

	name = g_file_get_parse_name (self->priv->target_file);

	if (op_type == ATHENA_FILE_UNDO_OP_CHANGE_OWNER) {
		*undo_description = g_strdup_printf (_("Restore group of '%s' to '%s'"),
						     name, self->priv->original_ownership);
		*redo_description = g_strdup_printf (_("Set group of '%s' to '%s'"),
						     name, self->priv->new_ownership);

		*undo_label = g_strdup (_("_Undo Change Group"));
		*redo_label = g_strdup (_("_Redo Change Group"));
	} else if (op_type == ATHENA_FILE_UNDO_OP_CHANGE_GROUP) {
		*undo_description = g_strdup_printf (_("Restore owner of '%s' to '%s'"),
						     name, self->priv->original_ownership);
		*redo_description = g_strdup_printf (_("Set owner of '%s' to '%s'"),
						     name, self->priv->new_ownership);

		*undo_label = g_strdup (_("_Undo Change Owner"));
		*redo_label = g_strdup (_("_Redo Change Owner"));
	}

	g_free (name);
}

static void
ownership_real_func (AthenaFileUndoInfoOwnership *self,
		     const gchar *ownership)
{
	AthenaFileUndoOp op_type = athena_file_undo_info_get_op_type (ATHENA_FILE_UNDO_INFO (self));
	AthenaFile *file;

	file = athena_file_get (self->priv->target_file);

	if (op_type == ATHENA_FILE_UNDO_OP_CHANGE_OWNER) {
		athena_file_set_owner (file,
					 ownership,
					 file_undo_info_operation_callback, self);
	} else if (op_type == ATHENA_FILE_UNDO_OP_CHANGE_GROUP) {
		athena_file_set_group (file,
					 ownership,
					 file_undo_info_operation_callback, self);
	}

	athena_file_unref (file);
}

static void
ownership_redo_func (AthenaFileUndoInfo *info,
		     GtkWindow *parent_window)
{
	AthenaFileUndoInfoOwnership *self = ATHENA_FILE_UNDO_INFO_OWNERSHIP (info);
	ownership_real_func (self, self->priv->new_ownership);
}

static void
ownership_undo_func (AthenaFileUndoInfo *info,
		     GtkWindow *parent_window)
{
	AthenaFileUndoInfoOwnership *self = ATHENA_FILE_UNDO_INFO_OWNERSHIP (info);
	ownership_real_func (self, self->priv->original_ownership);
}

static void
athena_file_undo_info_ownership_init (AthenaFileUndoInfoOwnership *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, athena_file_undo_info_ownership_get_type (),
						  AthenaFileUndoInfoOwnershipDetails);
}

static void
athena_file_undo_info_ownership_finalize (GObject *obj)
{
	AthenaFileUndoInfoOwnership *self = ATHENA_FILE_UNDO_INFO_OWNERSHIP (obj);

	g_clear_object (&self->priv->target_file);
	g_free (self->priv->original_ownership);
	g_free (self->priv->new_ownership);

	G_OBJECT_CLASS (athena_file_undo_info_ownership_parent_class)->finalize (obj);
}

static void
athena_file_undo_info_ownership_class_init (AthenaFileUndoInfoOwnershipClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS (klass);
	AthenaFileUndoInfoClass *iclass = ATHENA_FILE_UNDO_INFO_CLASS (klass);

	oclass->finalize = athena_file_undo_info_ownership_finalize;

	iclass->undo_func = ownership_undo_func;
	iclass->redo_func = ownership_redo_func;
	iclass->strings_func = ownership_strings_func;

	g_type_class_add_private (klass, sizeof (AthenaFileUndoInfoOwnershipDetails));
}

AthenaFileUndoInfo *
athena_file_undo_info_ownership_new (AthenaFileUndoOp  op_type,
				       GFile              *file,
				       const char         *current_data,
				       const char         *new_data)
{
	AthenaFileUndoInfoOwnership *retval;

	retval = g_object_new (ATHENA_TYPE_FILE_UNDO_INFO_OWNERSHIP,
			       "item-count", 1,
			       "op-type", op_type,
			       NULL);

	retval->priv->target_file = g_object_ref (file);
	retval->priv->original_ownership = g_strdup (current_data);
	retval->priv->new_ownership = g_strdup (new_data);

	return ATHENA_FILE_UNDO_INFO (retval);
}
