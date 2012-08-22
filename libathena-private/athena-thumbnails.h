/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-thumbnails.h: Thumbnail code for icon factory.
 
   Copyright (C) 2000 Eazel, Inc.
  
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
  
   Author: Andy Hertzfeld <andy@eazel.com>
*/

#ifndef ATHENA_THUMBNAILS_H
#define ATHENA_THUMBNAILS_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libathena-private/athena-file.h>

typedef struct AthenaThumbnailAsyncLoadHandle AthenaThumbnailAsyncLoadHandle;

typedef void (* AthenaThumbnailAsyncLoadFunc) (AthenaThumbnailAsyncLoadHandle *handle,
						 const char *path,
						 GdkPixbuf  *pixbuf,
						 double      scale_x,
						 double      scale_y,
						 gpointer    user_data);


#define ATHENA_THUMBNAIL_FRAME_LEFT 3
#define ATHENA_THUMBNAIL_FRAME_TOP 3
#define ATHENA_THUMBNAIL_FRAME_RIGHT 3
#define ATHENA_THUMBNAIL_FRAME_BOTTOM 3

/* Returns NULL if there's no thumbnail yet. */
void       athena_create_thumbnail                (AthenaFile *file);
gboolean   athena_can_thumbnail                   (AthenaFile *file);
gboolean   athena_can_thumbnail_internally        (AthenaFile *file);
gboolean   athena_thumbnail_is_mimetype_limited_by_size
						    (const char *mime_type);
void       athena_thumbnail_frame_image           (GdkPixbuf **pixbuf);
GdkPixbuf *athena_thumbnail_unframe_image         (GdkPixbuf  *pixbuf);
GdkPixbuf *athena_thumbnail_load_image            (const char *path,
						     guint       base_size,
						     guint       nominal_size,
						     gboolean    force_nominal,
						     double     *scale_x_out,
						     double     *scale_y_out);
AthenaThumbnailAsyncLoadHandle *
	   athena_thumbnail_load_image_async	    (const char *path,
						     guint       base_size,
						     guint       nominal_size,
						     gboolean    force_nominal,
						     AthenaThumbnailAsyncLoadFunc load_func,
						     gpointer    load_func_user_data);
void       athena_thumbnail_load_image_cancel     (AthenaThumbnailAsyncLoadHandle *handle);
void       athena_update_thumbnail_file_copied    (const char   *source_file_uri,
						     const char   *destination_file_uri);
void       athena_update_thumbnail_file_renamed   (const char   *source_file_uri,
						     const char   *destination_file_uri);
void       athena_remove_thumbnail_for_file       (const char   *file_uri);

/* Queue handling: */
void       athena_thumbnail_remove_from_queue     (const char   *file_uri);
void       athena_thumbnail_remove_all_from_queue (void);
void       athena_thumbnail_prioritize            (const char   *file_uri);


#endif /* ATHENA_THUMBNAILS_H */
