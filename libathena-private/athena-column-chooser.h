/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-column-choose.h - A column chooser widget

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

#ifndef ATHENA_COLUMN_CHOOSER_H
#define ATHENA_COLUMN_CHOOSER_H

#include <gtk/gtk.h>
#include <libathena-private/athena-file.h>

#define ATHENA_TYPE_COLUMN_CHOOSER athena_column_chooser_get_type()
#define ATHENA_COLUMN_CHOOSER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_COLUMN_CHOOSER, AthenaColumnChooser))
#define ATHENA_COLUMN_CHOOSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_COLUMN_CHOOSER, AthenaColumnChooserClass))
#define ATHENA_IS_COLUMN_CHOOSER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_COLUMN_CHOOSER))
#define ATHENA_IS_COLUMN_CHOOSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_COLUMN_CHOOSER))
#define ATHENA_COLUMN_CHOOSER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_COLUMN_CHOOSER, AthenaColumnChooserClass))

typedef struct _AthenaColumnChooserDetails AthenaColumnChooserDetails;

typedef struct {
	GtkBox parent;
	
	AthenaColumnChooserDetails *details;
} AthenaColumnChooser;

typedef struct {
        GtkBoxClass parent_slot;

	void (*changed) (AthenaColumnChooser *chooser);
	void (*use_default) (AthenaColumnChooser *chooser);
} AthenaColumnChooserClass;

GType      athena_column_chooser_get_type            (void);
GtkWidget *athena_column_chooser_new                 (AthenaFile *file);
void       athena_column_chooser_set_settings    (AthenaColumnChooser   *chooser,
						    char                   **visible_columns, 
						    char                   **column_order);
void       athena_column_chooser_get_settings    (AthenaColumnChooser *chooser,
						    char                  ***visible_columns, 
						    char                  ***column_order);

#endif /* ATHENA_COLUMN_CHOOSER_H */
