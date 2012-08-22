/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 *  Athena
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Author : Mr Jamie McCracken (jamiemcc at blueyonder dot co dot uk)
 *
 */
#ifndef _ATHENA_PLACES_SIDEBAR_H
#define _ATHENA_PLACES_SIDEBAR_H

#include "athena-window.h"

#include <gtk/gtk.h>

#define ATHENA_PLACES_SIDEBAR_ID    "places"

#define ATHENA_TYPE_PLACES_SIDEBAR athena_places_sidebar_get_type()
#define ATHENA_PLACES_SIDEBAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_PLACES_SIDEBAR, AthenaPlacesSidebar))
#define ATHENA_PLACES_SIDEBAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_PLACES_SIDEBAR, AthenaPlacesSidebarClass))
#define ATHENA_IS_PLACES_SIDEBAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_PLACES_SIDEBAR))
#define ATHENA_IS_PLACES_SIDEBAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_PLACES_SIDEBAR))
#define ATHENA_PLACES_SIDEBAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_PLACES_SIDEBAR, AthenaPlacesSidebarClass))


GType athena_places_sidebar_get_type (void);
GtkWidget * athena_places_sidebar_new (AthenaWindow *window);


#endif
