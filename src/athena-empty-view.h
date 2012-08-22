/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-empty-view.h - interface for empty view of directory.

   Copyright (C) 2006 Free Software Foundation, Inc.
   
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

   Authors: Christian Neumair <chris@gnome-de.org>
*/

#ifndef ATHENA_EMPTY_VIEW_H
#define ATHENA_EMPTY_VIEW_H

#include "athena-view.h"

#define ATHENA_TYPE_EMPTY_VIEW athena_empty_view_get_type()
#define ATHENA_EMPTY_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_EMPTY_VIEW, AthenaEmptyView))
#define ATHENA_EMPTY_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_EMPTY_VIEW, AthenaEmptyViewClass))
#define ATHENA_IS_EMPTY_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_EMPTY_VIEW))
#define ATHENA_IS_EMPTY_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_EMPTY_VIEW))
#define ATHENA_EMPTY_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_EMPTY_VIEW, AthenaEmptyViewClass))

#define ATHENA_EMPTY_VIEW_ID "OAFIID:Athena_File_Manager_Empty_View"

typedef struct AthenaEmptyViewDetails AthenaEmptyViewDetails;

typedef struct {
	AthenaView parent_instance;
	AthenaEmptyViewDetails *details;
} AthenaEmptyView;

typedef struct {
	AthenaViewClass parent_class;
} AthenaEmptyViewClass;

GType athena_empty_view_get_type (void);
void  athena_empty_view_register (void);

#endif /* ATHENA_EMPTY_VIEW_H */
