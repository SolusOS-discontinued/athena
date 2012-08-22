/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-list-view.h - interface for list view of directory.

   Copyright (C) 2000 Eazel, Inc.
   Copyright (C) 2001 Anders Carlsson <andersca@gnu.org>
   
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

   Authors: John Sullivan <sullivan@eazel.com>
            Anders Carlsson <andersca@gnu.org>
*/

#ifndef ATHENA_LIST_VIEW_H
#define ATHENA_LIST_VIEW_H

#include "athena-view.h"

#define ATHENA_TYPE_LIST_VIEW athena_list_view_get_type()
#define ATHENA_LIST_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_LIST_VIEW, AthenaListView))
#define ATHENA_LIST_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_LIST_VIEW, AthenaListViewClass))
#define ATHENA_IS_LIST_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_LIST_VIEW))
#define ATHENA_IS_LIST_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_LIST_VIEW))
#define ATHENA_LIST_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_LIST_VIEW, AthenaListViewClass))

#define ATHENA_LIST_VIEW_ID "OAFIID:Athena_File_Manager_List_View"

typedef struct AthenaListViewDetails AthenaListViewDetails;

typedef struct {
	AthenaView parent_instance;
	AthenaListViewDetails *details;
} AthenaListView;

typedef struct {
	AthenaViewClass parent_class;
} AthenaListViewClass;

GType athena_list_view_get_type (void);
void  athena_list_view_register (void);
GtkTreeView* athena_list_view_get_tree_view (AthenaListView *list_view);

#endif /* ATHENA_LIST_VIEW_H */
