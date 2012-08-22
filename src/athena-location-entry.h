/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Athena
 *
 * Copyright (C) 2000 Eazel, Inc.
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
 *
 * Author: Maciej Stachowiak <mjs@eazel.com>
 *         Ettore Perazzoli <ettore@gnu.org>
 */

#ifndef ATHENA_LOCATION_ENTRY_H
#define ATHENA_LOCATION_ENTRY_H

#include <libathena-private/athena-entry.h>

#define ATHENA_TYPE_LOCATION_ENTRY athena_location_entry_get_type()
#define ATHENA_LOCATION_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_LOCATION_ENTRY, AthenaLocationEntry))
#define ATHENA_LOCATION_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_LOCATION_ENTRY, AthenaLocationEntryClass))
#define ATHENA_IS_LOCATION_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_LOCATION_ENTRY))
#define ATHENA_IS_LOCATION_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_LOCATION_ENTRY))
#define ATHENA_LOCATION_ENTRY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_LOCATION_ENTRY, AthenaLocationEntryClass))

typedef struct AthenaLocationEntryDetails AthenaLocationEntryDetails;

typedef struct AthenaLocationEntry {
	AthenaEntry parent;
	AthenaLocationEntryDetails *details;
} AthenaLocationEntry;

typedef struct {
	AthenaEntryClass parent_class;
} AthenaLocationEntryClass;

typedef enum {
	ATHENA_LOCATION_ENTRY_ACTION_GOTO,
	ATHENA_LOCATION_ENTRY_ACTION_CLEAR
} AthenaLocationEntryAction;

GType      athena_location_entry_get_type     	(void);
GtkWidget* athena_location_entry_new          	(void);
void       athena_location_entry_set_special_text     (AthenaLocationEntry *entry,
							 const char            *special_text);
void       athena_location_entry_set_secondary_action (AthenaLocationEntry *entry,
							 AthenaLocationEntryAction secondary_action);
void       athena_location_entry_update_current_location (AthenaLocationEntry *entry,
							    const char *path);

#endif /* ATHENA_LOCATION_ENTRY_H */
