/*
 *  athena-column.h - Info columns exported by 
 *                      AthenaColumnProvider objects.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

#ifndef ATHENA_COLUMN_H
#define ATHENA_COLUMN_H

#include <glib-object.h>
#include "athena-extension-types.h"

G_BEGIN_DECLS

#define ATHENA_TYPE_COLUMN            (athena_column_get_type())
#define ATHENA_COLUMN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_COLUMN, AthenaColumn))
#define ATHENA_COLUMN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_COLUMN, AthenaColumnClass))
#define ATHENA_INFO_IS_COLUMN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_COLUMN))
#define ATHENA_INFO_IS_COLUMN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), ATHENA_TYPE_COLUMN))
#define ATHENA_COLUMN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), ATHENA_TYPE_COLUMN, AthenaColumnClass))

typedef struct _AthenaColumn        AthenaColumn;
typedef struct _AthenaColumnDetails AthenaColumnDetails;
typedef struct _AthenaColumnClass   AthenaColumnClass;

struct _AthenaColumn {
	GObject parent;

	AthenaColumnDetails *details;
};

struct _AthenaColumnClass {
	GObjectClass parent;
};

GType             athena_column_get_type        (void);
AthenaColumn *  athena_column_new             (const char     *name,
						   const char     *attribute,
						   const char     *label,
						   const char     *description);

/* AthenaColumn has the following properties:
 *   name (string)        - the identifier for the column
 *   attribute (string)   - the file attribute to be displayed in the 
 *                          column
 *   label (string)       - the user-visible label for the column
 *   description (string) - a user-visible description of the column
 *   xalign (float)       - x-alignment of the column 
 */

G_END_DECLS

#endif
