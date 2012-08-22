/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-file-undo-manager.h - Manages the undo/redo stack
 *
 * Copyright (C) 2007-2011 Amos Brocco
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
 * Author: Amos Brocco <amos.brocco@gmail.com>
 */

#ifndef __ATHENA_FILE_UNDO_MANAGER_H__
#define __ATHENA_FILE_UNDO_MANAGER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#include <libathena-private/athena-file-undo-operations.h>

typedef struct _AthenaFileUndoManager AthenaFileUndoManager;
typedef struct _AthenaFileUndoManagerClass AthenaFileUndoManagerClass;
typedef struct _AthenaFileUndoManagerPrivate AthenaFileUndoManagerPrivate;

#define ATHENA_TYPE_FILE_UNDO_MANAGER\
	(athena_file_undo_manager_get_type())
#define ATHENA_FILE_UNDO_MANAGER(object)\
	(G_TYPE_CHECK_INSTANCE_CAST((object), ATHENA_TYPE_FILE_UNDO_MANAGER,\
				    AthenaFileUndoManager))
#define ATHENA_FILE_UNDO_MANAGER_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_CAST((klass), ATHENA_TYPE_FILE_UNDO_MANAGER,\
				 AthenaFileUndoManagerClass))
#define ATHENA_IS_FILE_UNDO_MANAGER(object)\
	(G_TYPE_CHECK_INSTANCE_TYPE((object), ATHENA_TYPE_FILE_UNDO_MANAGER))
#define ATHENA_IS_FILE_UNDO_MANAGER_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_TYPE((klass), ATHENA_TYPE_FILE_UNDO_MANAGER))
#define ATHENA_FILE_UNDO_MANAGER_GET_CLASS(object)\
	(G_TYPE_INSTANCE_GET_CLASS((object), ATHENA_TYPE_FILE_UNDO_MANAGER,\
				   AthenaFileUndoManagerClass))

typedef enum {
	ATHENA_FILE_UNDO_MANAGER_STATE_NONE,
	ATHENA_FILE_UNDO_MANAGER_STATE_UNDO,
	ATHENA_FILE_UNDO_MANAGER_STATE_REDO
} AthenaFileUndoManagerState;

struct _AthenaFileUndoManager {
	GObject parent_instance;

	/* < private > */
	AthenaFileUndoManagerPrivate* priv;
};

struct _AthenaFileUndoManagerClass {
	GObjectClass parent_class;
};

GType athena_file_undo_manager_get_type (void) G_GNUC_CONST;

AthenaFileUndoManager * athena_file_undo_manager_get (void);

void athena_file_undo_manager_set_action (AthenaFileUndoInfo *info);
AthenaFileUndoInfo *athena_file_undo_manager_get_action (void);

AthenaFileUndoManagerState athena_file_undo_manager_get_state (void);

void athena_file_undo_manager_undo (GtkWindow *parent_window);
void athena_file_undo_manager_redo (GtkWindow *parent_window);

void athena_file_undo_manager_push_flag (void);
gboolean athena_file_undo_manager_pop_flag (void);

#endif /* __ATHENA_FILE_UNDO_MANAGER_H__ */
