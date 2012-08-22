/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-file-undo-operations.h - Manages undo/redo of file operations
 *
 * Copyright (C) 2007-2011 Amos Brocco
 * Copyright (C) 2010 Red Hat, Inc.
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

#ifndef __ATHENA_FILE_UNDO_OPERATIONS_H__
#define __ATHENA_FILE_UNDO_OPERATIONS_H__

#include <gio/gio.h>
#include <gtk/gtk.h>

typedef enum {
	ATHENA_FILE_UNDO_OP_COPY,
	ATHENA_FILE_UNDO_OP_DUPLICATE,
	ATHENA_FILE_UNDO_OP_MOVE,
	ATHENA_FILE_UNDO_OP_RENAME,
	ATHENA_FILE_UNDO_OP_CREATE_EMPTY_FILE,
	ATHENA_FILE_UNDO_OP_CREATE_FILE_FROM_TEMPLATE,
	ATHENA_FILE_UNDO_OP_CREATE_FOLDER,
	ATHENA_FILE_UNDO_OP_MOVE_TO_TRASH,
	ATHENA_FILE_UNDO_OP_RESTORE_FROM_TRASH,
	ATHENA_FILE_UNDO_OP_CREATE_LINK,
	ATHENA_FILE_UNDO_OP_RECURSIVE_SET_PERMISSIONS,
	ATHENA_FILE_UNDO_OP_SET_PERMISSIONS,
	ATHENA_FILE_UNDO_OP_CHANGE_GROUP,
	ATHENA_FILE_UNDO_OP_CHANGE_OWNER,
	ATHENA_FILE_UNDO_OP_NUM_TYPES,
} AthenaFileUndoOp;

#define ATHENA_TYPE_FILE_UNDO_INFO         (athena_file_undo_info_get_type ())
#define ATHENA_FILE_UNDO_INFO(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_UNDO_INFO, AthenaFileUndoInfo))
#define ATHENA_FILE_UNDO_INFO_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_UNDO_INFO, AthenaFileUndoInfoClass))
#define ATHENA_IS_FILE_UNDO_INFO(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_UNDO_INFO))
#define ATHENA_IS_FILE_UNDO_INFO_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_UNDO_INFO))
#define ATHENA_FILE_UNDO_INFO_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_UNDO_INFO, AthenaFileUndoInfoClass))

typedef struct _AthenaFileUndoInfo      AthenaFileUndoInfo;
typedef struct _AthenaFileUndoInfoClass AthenaFileUndoInfoClass;
typedef struct _AthenaFileUndoInfoDetails AthenaFileUndoInfoDetails;

struct _AthenaFileUndoInfo {
	GObject parent;
	AthenaFileUndoInfoDetails *priv;
};

struct _AthenaFileUndoInfoClass {
	GObjectClass parent_class;

	void (* undo_func) (AthenaFileUndoInfo *self,
			    GtkWindow            *parent_window);
	void (* redo_func) (AthenaFileUndoInfo *self,
			    GtkWindow            *parent_window);

	void (* strings_func) (AthenaFileUndoInfo *self,
			       gchar **undo_label,
			       gchar **undo_description,
			       gchar **redo_label,
			       gchar **redo_description);
};

GType athena_file_undo_info_get_type (void) G_GNUC_CONST;

void athena_file_undo_info_apply_async (AthenaFileUndoInfo *self,
					  gboolean undo,
					  GtkWindow *parent_window,
					  GAsyncReadyCallback callback,
					  gpointer user_data);
gboolean athena_file_undo_info_apply_finish (AthenaFileUndoInfo *self,
					       GAsyncResult *res,
					       gboolean *user_cancel,
					       GError **error);

void athena_file_undo_info_get_strings (AthenaFileUndoInfo *self,
					  gchar **undo_label,
					  gchar **undo_description,
					  gchar **redo_label,
					  gchar **redo_description);

/* copy/move/duplicate/link/restore from trash */
#define ATHENA_TYPE_FILE_UNDO_INFO_EXT         (athena_file_undo_info_ext_get_type ())
#define ATHENA_FILE_UNDO_INFO_EXT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_UNDO_INFO_EXT, AthenaFileUndoInfoExt))
#define ATHENA_FILE_UNDO_INFO_EXT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_UNDO_INFO_EXT, AthenaFileUndoInfoExtClass))
#define ATHENA_IS_FILE_UNDO_INFO_EXT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_UNDO_INFO_EXT))
#define ATHENA_IS_FILE_UNDO_INFO_EXT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_UNDO_INFO_EXT))
#define ATHENA_FILE_UNDO_INFO_EXT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_UNDO_INFO_EXT, AthenaFileUndoInfoExtClass))

typedef struct _AthenaFileUndoInfoExt      AthenaFileUndoInfoExt;
typedef struct _AthenaFileUndoInfoExtClass AthenaFileUndoInfoExtClass;
typedef struct _AthenaFileUndoInfoExtDetails AthenaFileUndoInfoExtDetails;

struct _AthenaFileUndoInfoExt {
	AthenaFileUndoInfo parent;
	AthenaFileUndoInfoExtDetails *priv;
};

struct _AthenaFileUndoInfoExtClass {
	AthenaFileUndoInfoClass parent_class;
};

GType athena_file_undo_info_ext_get_type (void) G_GNUC_CONST;
AthenaFileUndoInfo *athena_file_undo_info_ext_new (AthenaFileUndoOp op_type,
						       gint item_count,
						       GFile *src_dir,
						       GFile *target_dir);
void athena_file_undo_info_ext_add_origin_target_pair (AthenaFileUndoInfoExt *self,
							 GFile                   *origin,
							 GFile                   *target);

/* create new file/folder */
#define ATHENA_TYPE_FILE_UNDO_INFO_CREATE         (athena_file_undo_info_create_get_type ())
#define ATHENA_FILE_UNDO_INFO_CREATE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_UNDO_INFO_CREATE, AthenaFileUndoInfoCreate))
#define ATHENA_FILE_UNDO_INFO_CREATE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_UNDO_INFO_CREATE, AthenaFileUndoInfoCreateClass))
#define ATHENA_IS_FILE_UNDO_INFO_CREATE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_UNDO_INFO_CREATE))
#define ATHENA_IS_FILE_UNDO_INFO_CREATE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_UNDO_INFO_CREATE))
#define ATHENA_FILE_UNDO_INFO_CREATE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_UNDO_INFO_CREATE, AthenaFileUndoInfoCreateClass))

typedef struct _AthenaFileUndoInfoCreate      AthenaFileUndoInfoCreate;
typedef struct _AthenaFileUndoInfoCreateClass AthenaFileUndoInfoCreateClass;
typedef struct _AthenaFileUndoInfoCreateDetails AthenaFileUndoInfoCreateDetails;

struct _AthenaFileUndoInfoCreate {
	AthenaFileUndoInfo parent;
	AthenaFileUndoInfoCreateDetails *priv;
};

struct _AthenaFileUndoInfoCreateClass {
	AthenaFileUndoInfoClass parent_class;
};

GType athena_file_undo_info_create_get_type (void) G_GNUC_CONST;
AthenaFileUndoInfo *athena_file_undo_info_create_new (AthenaFileUndoOp op_type);
void athena_file_undo_info_create_set_data (AthenaFileUndoInfoCreate *self,
					      GFile                      *file,
					      const char                 *template,
					      gint                        length);

/* rename */
#define ATHENA_TYPE_FILE_UNDO_INFO_RENAME         (athena_file_undo_info_rename_get_type ())
#define ATHENA_FILE_UNDO_INFO_RENAME(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_UNDO_INFO_RENAME, AthenaFileUndoInfoRename))
#define ATHENA_FILE_UNDO_INFO_RENAME_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_UNDO_INFO_RENAME, AthenaFileUndoInfoRenameClass))
#define ATHENA_IS_FILE_UNDO_INFO_RENAME(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_UNDO_INFO_RENAME))
#define ATHENA_IS_FILE_UNDO_INFO_RENAME_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_UNDO_INFO_RENAME))
#define ATHENA_FILE_UNDO_INFO_RENAME_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_UNDO_INFO_RENAME, AthenaFileUndoInfoRenameClass))

typedef struct _AthenaFileUndoInfoRename      AthenaFileUndoInfoRename;
typedef struct _AthenaFileUndoInfoRenameClass AthenaFileUndoInfoRenameClass;
typedef struct _AthenaFileUndoInfoRenameDetails AthenaFileUndoInfoRenameDetails;

struct _AthenaFileUndoInfoRename {
	AthenaFileUndoInfo parent;
	AthenaFileUndoInfoRenameDetails *priv;
};

struct _AthenaFileUndoInfoRenameClass {
	AthenaFileUndoInfoClass parent_class;
};

GType athena_file_undo_info_rename_get_type (void) G_GNUC_CONST;
AthenaFileUndoInfo *athena_file_undo_info_rename_new (void);
void athena_file_undo_info_rename_set_data (AthenaFileUndoInfoRename *self,
					      GFile                      *old_file,
					      GFile                      *new_file);

/* trash */
#define ATHENA_TYPE_FILE_UNDO_INFO_TRASH         (athena_file_undo_info_trash_get_type ())
#define ATHENA_FILE_UNDO_INFO_TRASH(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_UNDO_INFO_TRASH, AthenaFileUndoInfoTrash))
#define ATHENA_FILE_UNDO_INFO_TRASH_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_UNDO_INFO_TRASH, AthenaFileUndoInfoTrashClass))
#define ATHENA_IS_FILE_UNDO_INFO_TRASH(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_UNDO_INFO_TRASH))
#define ATHENA_IS_FILE_UNDO_INFO_TRASH_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_UNDO_INFO_TRASH))
#define ATHENA_FILE_UNDO_INFO_TRASH_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_UNDO_INFO_TRASH, AthenaFileUndoInfoTrashClass))

typedef struct _AthenaFileUndoInfoTrash      AthenaFileUndoInfoTrash;
typedef struct _AthenaFileUndoInfoTrashClass AthenaFileUndoInfoTrashClass;
typedef struct _AthenaFileUndoInfoTrashDetails AthenaFileUndoInfoTrashDetails;

struct _AthenaFileUndoInfoTrash {
	AthenaFileUndoInfo parent;
	AthenaFileUndoInfoTrashDetails *priv;
};

struct _AthenaFileUndoInfoTrashClass {
	AthenaFileUndoInfoClass parent_class;
};

GType athena_file_undo_info_trash_get_type (void) G_GNUC_CONST;
AthenaFileUndoInfo *athena_file_undo_info_trash_new (gint item_count);
void athena_file_undo_info_trash_add_file (AthenaFileUndoInfoTrash *self,
					     GFile                     *file);

/* recursive permissions */
#define ATHENA_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS         (athena_file_undo_info_rec_permissions_get_type ())
#define ATHENA_FILE_UNDO_INFO_REC_PERMISSIONS(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS, AthenaFileUndoInfoRecPermissions))
#define ATHENA_FILE_UNDO_INFO_REC_PERMISSIONS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS, AthenaFileUndoInfoRecPermissionsClass))
#define ATHENA_IS_FILE_UNDO_INFO_REC_PERMISSIONS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS))
#define ATHENA_IS_FILE_UNDO_INFO_REC_PERMISSIONS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS))
#define ATHENA_FILE_UNDO_INFO_REC_PERMISSIONS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS, AthenaFileUndoInfoRecPermissionsClass))

typedef struct _AthenaFileUndoInfoRecPermissions      AthenaFileUndoInfoRecPermissions;
typedef struct _AthenaFileUndoInfoRecPermissionsClass AthenaFileUndoInfoRecPermissionsClass;
typedef struct _AthenaFileUndoInfoRecPermissionsDetails AthenaFileUndoInfoRecPermissionsDetails;

struct _AthenaFileUndoInfoRecPermissions {
	AthenaFileUndoInfo parent;
	AthenaFileUndoInfoRecPermissionsDetails *priv;
};

struct _AthenaFileUndoInfoRecPermissionsClass {
	AthenaFileUndoInfoClass parent_class;
};

GType athena_file_undo_info_rec_permissions_get_type (void) G_GNUC_CONST;
AthenaFileUndoInfo *athena_file_undo_info_rec_permissions_new (GFile   *dest,
								   guint32 file_permissions,
								   guint32 file_mask,
								   guint32 dir_permissions,
								   guint32 dir_mask);
void athena_file_undo_info_rec_permissions_add_file (AthenaFileUndoInfoRecPermissions *self,
						       GFile                              *file,
						       guint32                             permission);

/* single file change permissions */
#define ATHENA_TYPE_FILE_UNDO_INFO_PERMISSIONS         (athena_file_undo_info_permissions_get_type ())
#define ATHENA_FILE_UNDO_INFO_PERMISSIONS(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_UNDO_INFO_PERMISSIONS, AthenaFileUndoInfoPermissions))
#define ATHENA_FILE_UNDO_INFO_PERMISSIONS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_UNDO_INFO_PERMISSIONS, AthenaFileUndoInfoPermissionsClass))
#define ATHENA_IS_FILE_UNDO_INFO_PERMISSIONS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_UNDO_INFO_PERMISSIONS))
#define ATHENA_IS_FILE_UNDO_INFO_PERMISSIONS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_UNDO_INFO_PERMISSIONS))
#define ATHENA_FILE_UNDO_INFO_PERMISSIONS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_UNDO_INFO_PERMISSIONS, AthenaFileUndoInfoPermissionsClass))

typedef struct _AthenaFileUndoInfoPermissions      AthenaFileUndoInfoPermissions;
typedef struct _AthenaFileUndoInfoPermissionsClass AthenaFileUndoInfoPermissionsClass;
typedef struct _AthenaFileUndoInfoPermissionsDetails AthenaFileUndoInfoPermissionsDetails;

struct _AthenaFileUndoInfoPermissions {
	AthenaFileUndoInfo parent;
	AthenaFileUndoInfoPermissionsDetails *priv;
};

struct _AthenaFileUndoInfoPermissionsClass {
	AthenaFileUndoInfoClass parent_class;
};

GType athena_file_undo_info_permissions_get_type (void) G_GNUC_CONST;
AthenaFileUndoInfo *athena_file_undo_info_permissions_new (GFile   *file,
							       guint32  current_permissions,
							       guint32  new_permissions);

/* group and owner change */
#define ATHENA_TYPE_FILE_UNDO_INFO_OWNERSHIP         (athena_file_undo_info_ownership_get_type ())
#define ATHENA_FILE_UNDO_INFO_OWNERSHIP(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_UNDO_INFO_OWNERSHIP, AthenaFileUndoInfoOwnership))
#define ATHENA_FILE_UNDO_INFO_OWNERSHIP_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_UNDO_INFO_OWNERSHIP, AthenaFileUndoInfoOwnershipClass))
#define ATHENA_IS_FILE_UNDO_INFO_OWNERSHIP(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_UNDO_INFO_OWNERSHIP))
#define ATHENA_IS_FILE_UNDO_INFO_OWNERSHIP_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_UNDO_INFO_OWNERSHIP))
#define ATHENA_FILE_UNDO_INFO_OWNERSHIP_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_UNDO_INFO_OWNERSHIP, AthenaFileUndoInfoOwnershipClass))

typedef struct _AthenaFileUndoInfoOwnership      AthenaFileUndoInfoOwnership;
typedef struct _AthenaFileUndoInfoOwnershipClass AthenaFileUndoInfoOwnershipClass;
typedef struct _AthenaFileUndoInfoOwnershipDetails AthenaFileUndoInfoOwnershipDetails;

struct _AthenaFileUndoInfoOwnership {
	AthenaFileUndoInfo parent;
	AthenaFileUndoInfoOwnershipDetails *priv;
};

struct _AthenaFileUndoInfoOwnershipClass {
	AthenaFileUndoInfoClass parent_class;
};

GType athena_file_undo_info_ownership_get_type (void) G_GNUC_CONST;
AthenaFileUndoInfo *athena_file_undo_info_ownership_new (AthenaFileUndoOp  op_type,
							     GFile              *file,
							     const char         *current_data,
							     const char         *new_data);

#endif /* __ATHENA_FILE_UNDO_OPERATIONS_H__ */
