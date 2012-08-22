/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* AthenaUndoManager - Manages undo and redo transactions.
 *                       This is the public interface used by the application.                      
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Author: Gene Z. Ragan <gzr@eazel.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef ATHENA_UNDO_MANAGER_H
#define ATHENA_UNDO_MANAGER_H

#include <libathena-private/athena-undo.h>

#define ATHENA_TYPE_UNDO_MANAGER athena_undo_manager_get_type()
#define ATHENA_UNDO_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_UNDO_MANAGER, AthenaUndoManager))
#define ATHENA_UNDO_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_UNDO_MANAGER, AthenaUndoManagerClass))
#define ATHENA_IS_UNDO_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_UNDO_MANAGER))
#define ATHENA_IS_UNDO_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_UNDO_MANAGER))
#define ATHENA_UNDO_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_UNDO_MANAGER, AthenaUndoManagerClass))
	
typedef struct AthenaUndoManagerDetails AthenaUndoManagerDetails;

typedef struct {
	GObject parent;
	AthenaUndoManagerDetails *details;
} AthenaUndoManager;

typedef struct {
	GObjectClass parent_slot;
	void (* changed) (GObject *object, gpointer data);
} AthenaUndoManagerClass;

GType                athena_undo_manager_get_type                           (void);
AthenaUndoManager *athena_undo_manager_new                                (void);

/* Undo operations. */
void                 athena_undo_manager_undo                               (AthenaUndoManager *undo_manager);

/* Attach the undo manager to a Gtk object so that object and the widgets inside it can participate in undo. */
void                 athena_undo_manager_attach                             (AthenaUndoManager *manager,
									       GObject             *object);

void		athena_undo_manager_append (AthenaUndoManager *manager,
					      AthenaUndoTransaction *transaction);
void            athena_undo_manager_forget (AthenaUndoManager *manager,
					      AthenaUndoTransaction *transaction);

#endif /* ATHENA_UNDO_MANAGER_H */
