/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-file-conflict-dialog: dialog that handles file conflicts
   during transfer operations.

   Copyright (C) 2008, Cosimo Cecchi

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   
   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   
   Authors: Cosimo Cecchi <cosimoc@gnome.org>
*/

#ifndef ATHENA_FILE_CONFLICT_DIALOG_H
#define ATHENA_FILE_CONFLICT_DIALOG_H

#include <glib-object.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#define ATHENA_TYPE_FILE_CONFLICT_DIALOG \
	(athena_file_conflict_dialog_get_type ())
#define ATHENA_FILE_CONFLICT_DIALOG(o) \
	(G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_FILE_CONFLICT_DIALOG,\
				     AthenaFileConflictDialog))
#define ATHENA_FILE_CONFLICT_DIALOG_CLASS(k) \
	(G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_FILE_CONFLICT_DIALOG,\
				 AthenaFileConflictDialogClass))
#define ATHENA_IS_FILE_CONFLICT_DIALOG(o) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_FILE_CONFLICT_DIALOG))
#define ATHENA_IS_FILE_CONFLICT_DIALOG_CLASS(k) \
	(G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_FILE_CONFLICT_DIALOG))
#define ATHENA_FILE_CONFLICT_DIALOG_GET_CLASS(o) \
	(G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_FILE_CONFLICT_DIALOG,\
				    AthenaFileConflictDialogClass))

typedef struct _AthenaFileConflictDialog        AthenaFileConflictDialog;
typedef struct _AthenaFileConflictDialogClass   AthenaFileConflictDialogClass;
typedef struct _AthenaFileConflictDialogDetails AthenaFileConflictDialogDetails;

struct _AthenaFileConflictDialog {
	GtkDialog parent;
	AthenaFileConflictDialogDetails *details;
};

struct _AthenaFileConflictDialogClass {
	GtkDialogClass parent_class;
};

enum
{
	CONFLICT_RESPONSE_SKIP = 1,
	CONFLICT_RESPONSE_REPLACE = 2,
	CONFLICT_RESPONSE_RENAME = 3,
};

GType athena_file_conflict_dialog_get_type (void) G_GNUC_CONST;

GtkWidget* athena_file_conflict_dialog_new              (GtkWindow *parent,
							   GFile *source,
							   GFile *destination,
							   GFile *dest_dir);
char*      athena_file_conflict_dialog_get_new_name     (AthenaFileConflictDialog *dialog);
gboolean   athena_file_conflict_dialog_get_apply_to_all (AthenaFileConflictDialog *dialog);

#endif /* ATHENA_FILE_CONFLICT_DIALOG_H */
