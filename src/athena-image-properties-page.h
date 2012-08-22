/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* 
 * Copyright (C) 2004 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 */

#ifndef ATHENA_IMAGE_PROPERTIES_PAGE_H
#define ATHENA_IMAGE_PROPERTIES_PAGE_H

#include <gtk/gtk.h>

#define ATHENA_TYPE_IMAGE_PROPERTIES_PAGE athena_image_properties_page_get_type()
#define ATHENA_IMAGE_PROPERTIES_PAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_IMAGE_PROPERTIES_PAGE, AthenaImagePropertiesPage))
#define ATHENA_IMAGE_PROPERTIES_PAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_IMAGE_PROPERTIES_PAGE, AthenaImagePropertiesPageClass))
#define ATHENA_IS_IMAGE_PROPERTIES_PAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_IMAGE_PROPERTIES_PAGE))
#define ATHENA_IS_IMAGE_PROPERTIES_PAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_IMAGE_PROPERTIES_PAGE))
#define ATHENA_IMAGE_PROPERTIES_PAGE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_IMAGE_PROPERTIES_PAGE, AthenaImagePropertiesPageClass))

typedef struct AthenaImagePropertiesPageDetails AthenaImagePropertiesPageDetails;

typedef struct {
	GtkBox parent;
	AthenaImagePropertiesPageDetails *details;
} AthenaImagePropertiesPage;

typedef struct {
	GtkBoxClass parent;
} AthenaImagePropertiesPageClass;

GType athena_image_properties_page_get_type (void);
void  athena_image_properties_page_register (void);

#endif /* ATHENA_IMAGE_PROPERTIES_PAGE_H */
