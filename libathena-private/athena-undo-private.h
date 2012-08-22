/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* xxx
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

#ifndef ATHENA_UNDO_PRIVATE_H
#define ATHENA_UNDO_PRIVATE_H

#include <libathena-private/athena-undo.h>
#include <libathena-private/athena-undo-manager.h>
#include <glib-object.h>

AthenaUndoManager * athena_undo_get_undo_manager    (GObject               *attached_object);
void                  athena_undo_attach_undo_manager (GObject               *object,
							 AthenaUndoManager   *manager);

#endif /* ATHENA_UNDO_PRIVATE_H */
