/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* AthenaEntry: one-line text editing widget. This consists of bug fixes
 * and other improvements to GtkEntry, and all the changes could be rolled
 * into GtkEntry some day.
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Author: John Sullivan <sullivan@eazel.com>
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

#ifndef ATHENA_ENTRY_H
#define ATHENA_ENTRY_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ATHENA_TYPE_ENTRY athena_entry_get_type()
#define ATHENA_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_ENTRY, AthenaEntry))
#define ATHENA_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_ENTRY, AthenaEntryClass))
#define ATHENA_IS_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_ENTRY))
#define ATHENA_IS_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_ENTRY))
#define ATHENA_ENTRY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_ENTRY, AthenaEntryClass))

typedef struct AthenaEntryDetails AthenaEntryDetails;

typedef struct {
	GtkEntry parent;
	AthenaEntryDetails *details;
} AthenaEntry;

typedef struct {
	GtkEntryClass parent_class;

	void (*user_changed)      (AthenaEntry *entry);
	void (*selection_changed) (AthenaEntry *entry);
} AthenaEntryClass;

GType       athena_entry_get_type                 (void);
GtkWidget  *athena_entry_new                      (void);
GtkWidget  *athena_entry_new_with_max_length      (guint16        max);
void        athena_entry_set_text                 (AthenaEntry *entry,
						     const char    *text);
void        athena_entry_select_all               (AthenaEntry *entry);
void        athena_entry_select_all_at_idle       (AthenaEntry *entry);
void        athena_entry_set_special_tab_handling (AthenaEntry *entry,
						     gboolean       special_tab_handling);

G_END_DECLS

#endif /* ATHENA_ENTRY_H */
