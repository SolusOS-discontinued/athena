/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* AthenaUndoTransaction - An object for an undoable transaction.
 *                           Used internally by undo machinery.
 *                           Not public.
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

#ifndef ATHENA_UNDO_TRANSACTION_H
#define ATHENA_UNDO_TRANSACTION_H

#include <libathena-private/athena-undo.h>

#define ATHENA_TYPE_UNDO_TRANSACTION athena_undo_transaction_get_type()
#define ATHENA_UNDO_TRANSACTION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_UNDO_TRANSACTION, AthenaUndoTransaction))
#define ATHENA_UNDO_TRANSACTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_UNDO_TRANSACTION, AthenaUndoTransactionClass))
#define ATHENA_IS_UNDO_TRANSACTION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_UNDO_TRANSACTION))
#define ATHENA_IS_UNDO_TRANSACTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_UNDO_TRANSACTION))
#define ATHENA_UNDO_TRANSACTION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_UNDO_TRANSACTION, AthenaUndoTransactionClass))

/* The typedef for AthenaUndoTransaction is in athena-undo.h
   to avoid circular deps */
typedef struct _AthenaUndoTransactionClass AthenaUndoTransactionClass;

struct _AthenaUndoTransaction {
	GObject parent_slot;
	
	char *operation_name;
	char *undo_menu_item_label;
	char *undo_menu_item_hint;
	char *redo_menu_item_label;
	char *redo_menu_item_hint;
	GList *atom_list;

	AthenaUndoManager *owner;
};

struct _AthenaUndoTransactionClass {
	GObjectClass parent_slot;
};

GType                    athena_undo_transaction_get_type            (void);
AthenaUndoTransaction *athena_undo_transaction_new                 (const char              *operation_name,
									const char              *undo_menu_item_label,
									const char              *undo_menu_item_hint,
									const char              *redo_menu_item_label,
									const char              *redo_menu_item_hint);
void                     athena_undo_transaction_add_atom            (AthenaUndoTransaction *transaction,
									const AthenaUndoAtom  *atom);
void                     athena_undo_transaction_add_to_undo_manager (AthenaUndoTransaction *transaction,
									AthenaUndoManager     *manager);
void                     athena_undo_transaction_unregister_object   (GObject                 *atom_target);
void                     athena_undo_transaction_undo                (AthenaUndoTransaction *transaction);

#endif /* ATHENA_UNDO_TRANSACTION_H */
