/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-column-utilities.h - Utilities related to column specifications

   Copyright (C) 2004 Novell, Inc.

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the column COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Authors: Dave Camp <dave@ximian.com>
*/

#ifndef ATHENA_COLUMN_UTILITIES_H
#define ATHENA_COLUMN_UTILITIES_H

#include <libathena-extension/athena-column.h>
#include <libathena-private/athena-file.h>

GList *athena_get_all_columns       (void);
GList *athena_get_common_columns    (void);
GList *athena_get_columns_for_file (AthenaFile *file);
GList *athena_column_list_copy      (GList       *columns);
void   athena_column_list_free      (GList       *columns);

GList *athena_sort_columns          (GList       *columns,
				       char       **column_order);


#endif /* ATHENA_COLUMN_UTILITIES_H */
