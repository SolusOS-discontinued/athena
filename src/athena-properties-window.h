/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-properties-window.h - interface for window that lets user modify 
                            icon properties

   Copyright (C) 2000 Eazel, Inc.

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

   Authors: Darin Adler <darin@bentspoon.com>
*/

#ifndef ATHENA_PROPERTIES_WINDOW_H
#define ATHENA_PROPERTIES_WINDOW_H

#include <gtk/gtk.h>
#include <libathena-private/athena-file.h>

typedef struct AthenaPropertiesWindow AthenaPropertiesWindow;

#define ATHENA_TYPE_PROPERTIES_WINDOW athena_properties_window_get_type()
#define ATHENA_PROPERTIES_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_PROPERTIES_WINDOW, AthenaPropertiesWindow))
#define ATHENA_PROPERTIES_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_PROPERTIES_WINDOW, AthenaPropertiesWindowClass))
#define ATHENA_IS_PROPERTIES_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_PROPERTIES_WINDOW))
#define ATHENA_IS_PROPERTIES_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_PROPERTIES_WINDOW))
#define ATHENA_PROPERTIES_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_PROPERTIES_WINDOW, AthenaPropertiesWindowClass))

typedef struct AthenaPropertiesWindowDetails AthenaPropertiesWindowDetails;

struct AthenaPropertiesWindow {
	GtkDialog window;
	AthenaPropertiesWindowDetails *details;	
};

struct AthenaPropertiesWindowClass {
	GtkDialogClass parent_class;
	
	/* Keybinding signals */
	void (* close)    (AthenaPropertiesWindow *window);
};

typedef struct AthenaPropertiesWindowClass AthenaPropertiesWindowClass;

GType   athena_properties_window_get_type   (void);

void 	athena_properties_window_present    (GList       *files,
					       GtkWidget   *parent_widget,
					       const gchar *startup_id);

#endif /* ATHENA_PROPERTIES_WINDOW_H */
