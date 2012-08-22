/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-file-management-properties.h - Function to show the athena preference dialog.

   Copyright (C) 2002 Jan Arne Petersen

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Authors: Jan Arne Petersen <jpetersen@uni-bonn.de>
*/

#ifndef ATHENA_FILE_MANAGEMENT_PROPERTIES_H
#define ATHENA_FILE_MANAGEMENT_PROPERTIES_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

void athena_file_management_properties_dialog_show (GtkWindow *window);

G_END_DECLS

#endif /* ATHENA_FILE_MANAGEMENT_PROPERTIES_H */
