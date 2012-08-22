/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
   athena-mime-application-chooser.c: Manages applications for mime types
 
   Copyright (C) 2004 Novell, Inc.
 
   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but APPLICATIONOUT ANY WARRANTY; applicationout even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along application the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Authors: Dave Camp <dave@novell.com>
*/

#ifndef ATHENA_MIME_APPLICATION_CHOOSER_H
#define ATHENA_MIME_APPLICATION_CHOOSER_H

#include <gtk/gtk.h>

#define ATHENA_TYPE_MIME_APPLICATION_CHOOSER         (athena_mime_application_chooser_get_type ())
#define ATHENA_MIME_APPLICATION_CHOOSER(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_MIME_APPLICATION_CHOOSER, AthenaMimeApplicationChooser))
#define ATHENA_MIME_APPLICATION_CHOOSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_MIME_APPLICATION_CHOOSER, AthenaMimeApplicationChooserClass))
#define ATHENA_IS_MIME_APPLICATION_CHOOSER(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_MIME_APPLICATION_CHOOSER)

typedef struct _AthenaMimeApplicationChooser        AthenaMimeApplicationChooser;
typedef struct _AthenaMimeApplicationChooserClass   AthenaMimeApplicationChooserClass;
typedef struct _AthenaMimeApplicationChooserDetails AthenaMimeApplicationChooserDetails;

struct _AthenaMimeApplicationChooser {
	GtkBox parent;
	AthenaMimeApplicationChooserDetails *details;
};

struct _AthenaMimeApplicationChooserClass {
	GtkBoxClass parent_class;
};

GType      athena_mime_application_chooser_get_type (void);
GtkWidget * athena_mime_application_chooser_new (const char *uri,
						   GList *files,
						   const char *mime_type);

#endif /* ATHENA_MIME_APPLICATION_CHOOSER_H */
