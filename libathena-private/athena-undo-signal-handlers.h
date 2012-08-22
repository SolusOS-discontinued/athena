/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Signal handlers to enable undo in Gtk Widgets.
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

#ifndef ATHENA_UNDO_SIGNAL_HANDLERS_H
#define ATHENA_UNDO_SIGNAL_HANDLERS_H

#include <libathena-private/athena-entry.h>

void athena_undo_set_up_athena_entry_for_undo 	(AthenaEntry	*entry);
void athena_undo_tear_down_athena_entry_for_undo 	(AthenaEntry	*entry);
void athena_undo_set_up_editable_for_undo 		(GtkEditable 	*editable);
void athena_undo_tear_down_editable_for_undo 		(GtkEditable 	*editable);

#endif /* ATHENA_UNDO_SIGNAL_HANDLERS_H */
