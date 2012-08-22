/*
 *  athena-property-page.h - Property pages exported by 
 *                             AthenaPropertyProvider objects.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

#ifndef ATHENA_PROPERTY_PAGE_H
#define ATHENA_PROPERTY_PAGE_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include "athena-extension-types.h"

G_BEGIN_DECLS

#define ATHENA_TYPE_PROPERTY_PAGE            (athena_property_page_get_type())
#define ATHENA_PROPERTY_PAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_PROPERTY_PAGE, AthenaPropertyPage))
#define ATHENA_PROPERTY_PAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_PROPERTY_PAGE, AthenaPropertyPageClass))
#define ATHENA_IS_PROPERTY_PAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_PROPERTY_PAGE))
#define ATHENA_IS_PROPERTY_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), ATHENA_TYPE_PROPERTY_PAGE))
#define ATHENA_PROPERTY_PAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), ATHENA_TYPE_PROPERTY_PAGE, AthenaPropertyPageClass))

typedef struct _AthenaPropertyPage        AthenaPropertyPage;
typedef struct _AthenaPropertyPageDetails AthenaPropertyPageDetails;
typedef struct _AthenaPropertyPageClass   AthenaPropertyPageClass;

struct _AthenaPropertyPage
{
	GObject parent;

	AthenaPropertyPageDetails *details;
};

struct _AthenaPropertyPageClass 
{
	GObjectClass parent;
};

GType                 athena_property_page_get_type  (void);
AthenaPropertyPage *athena_property_page_new       (const char           *name,
							GtkWidget            *label,
							GtkWidget            *page);

/* AthenaPropertyPage has the following properties:
 *   name (string)        - the identifier for the property page
 *   label (widget)       - the user-visible label of the property page
 *   page (widget)        - the property page to display
 */

G_END_DECLS

#endif
