/*
 *  Copyright © 2002 Christophe Fergeau
 *  Copyright © 2003 Marco Pesenti Gritti
 *  Copyright © 2003, 2004 Christian Persch
 *    (ephy-notebook.c)
 *
 *  Copyright © 2008 Free Software Foundation, Inc.
 *    (athena-notebook.c)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  $Id: athena-notebook.h 8210 2008-04-11 20:05:25Z chpe $
 */

#ifndef ATHENA_NOTEBOOK_H
#define ATHENA_NOTEBOOK_H

#include <glib.h>
#include <gtk/gtk.h>
#include "athena-window-slot.h"

G_BEGIN_DECLS

#define ATHENA_TYPE_NOTEBOOK		(athena_notebook_get_type ())
#define ATHENA_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_NOTEBOOK, AthenaNotebook))
#define ATHENA_NOTEBOOK_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_NOTEBOOK, AthenaNotebookClass))
#define ATHENA_IS_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_NOTEBOOK))
#define ATHENA_IS_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_NOTEBOOK))
#define ATHENA_NOTEBOOK_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_NOTEBOOK, AthenaNotebookClass))

typedef struct _AthenaNotebookClass	AthenaNotebookClass;
typedef struct _AthenaNotebook		AthenaNotebook;
typedef struct _AthenaNotebookPrivate	AthenaNotebookPrivate;

struct _AthenaNotebook
{
	GtkNotebook parent;

	/*< private >*/
        AthenaNotebookPrivate *priv;
};

struct _AthenaNotebookClass
{
        GtkNotebookClass parent_class;

	/* Signals */
	void	 (* tab_close_request)  (AthenaNotebook *notebook,
					 AthenaWindowSlot *slot);
};

GType		athena_notebook_get_type		(void);

int		athena_notebook_add_tab	(AthenaNotebook *nb,
						 AthenaWindowSlot *slot,
						 int position,
						 gboolean jump_to);
	
void		athena_notebook_set_show_tabs	(AthenaNotebook *nb,
						 gboolean show_tabs);

void		athena_notebook_set_dnd_enabled (AthenaNotebook *nb,
						   gboolean enabled);
void		athena_notebook_sync_tab_label (AthenaNotebook *nb,
						  AthenaWindowSlot *slot);
void		athena_notebook_sync_loading   (AthenaNotebook *nb,
						  AthenaWindowSlot *slot);

void		athena_notebook_reorder_current_child_relative (AthenaNotebook *notebook,
								  int offset);
void		athena_notebook_set_current_page_relative (AthenaNotebook *notebook,
							     int offset);

gboolean        athena_notebook_can_reorder_current_child_relative (AthenaNotebook *notebook,
								      int offset);
gboolean        athena_notebook_can_set_current_page_relative (AthenaNotebook *notebook,
								 int offset);

G_END_DECLS

#endif /* ATHENA_NOTEBOOK_H */

