/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-cell-renderer-text-ellipsized.c: Cell renderer for text which
   will use pango ellipsization but deactivate it temporarily for the size
   calculation to get the size based on the actual text length.
 
   Copyright (C) 2007 Martin Wehner
  
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

   Author: Martin Wehner <martin.wehner@gmail.com>
*/

#ifndef ATHENA_CELL_RENDERER_TEXT_ELLIPSIZED_H
#define ATHENA_CELL_RENDERER_TEXT_ELLIPSIZED_H

#include <gtk/gtk.h>

#define ATHENA_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED athena_cell_renderer_text_ellipsized_get_type()
#define ATHENA_CELL_RENDERER_TEXT_ELLIPSIZED(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED, AthenaCellRendererTextEllipsized))
#define ATHENA_CELL_RENDERER_TEXT_ELLIPSIZED_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED, AthenaCellRendererTextEllipsizedClass))
#define ATHENA_IS_CELL_RENDERER_TEXT_ELLIPSIZED(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED))
#define ATHENA_IS_CELL_RENDERER_TEXT_ELLIPSIZED_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED))
#define ATHENA_CELL_RENDERER_TEXT_ELLIPSIZED_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED, AthenaCellRendererTextEllipsizedClass))


typedef struct _AthenaCellRendererTextEllipsized AthenaCellRendererTextEllipsized;
typedef struct _AthenaCellRendererTextEllipsizedClass AthenaCellRendererTextEllipsizedClass;

struct _AthenaCellRendererTextEllipsized {
	GtkCellRendererText parent;
};

struct _AthenaCellRendererTextEllipsizedClass {
	GtkCellRendererTextClass parent_class;
};

GType		 athena_cell_renderer_text_ellipsized_get_type (void);
GtkCellRenderer *athena_cell_renderer_text_ellipsized_new      (void);

#endif /* ATHENA_CELL_RENDERER_TEXT_ELLIPSIZED_H */
