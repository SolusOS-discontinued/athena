/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Athena - Icon canvas item class for icon container.
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef ATHENA_ICON_CANVAS_ITEM_H
#define ATHENA_ICON_CANVAS_ITEM_H

#include <eel/eel-canvas.h>
#include <eel/eel-art-extensions.h>

G_BEGIN_DECLS

#define ATHENA_TYPE_ICON_CANVAS_ITEM athena_icon_canvas_item_get_type()
#define ATHENA_ICON_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_ICON_CANVAS_ITEM, AthenaIconCanvasItem))
#define ATHENA_ICON_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_ICON_CANVAS_ITEM, AthenaIconCanvasItemClass))
#define ATHENA_IS_ICON_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_ICON_CANVAS_ITEM))
#define ATHENA_IS_ICON_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_ICON_CANVAS_ITEM))
#define ATHENA_ICON_CANVAS_ITEM_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_ICON_CANVAS_ITEM, AthenaIconCanvasItemClass))

typedef struct AthenaIconCanvasItem AthenaIconCanvasItem;
typedef struct AthenaIconCanvasItemClass AthenaIconCanvasItemClass;
typedef struct AthenaIconCanvasItemDetails AthenaIconCanvasItemDetails;

struct AthenaIconCanvasItem {
	EelCanvasItem item;
	AthenaIconCanvasItemDetails *details;
	gpointer user_data;
};

struct AthenaIconCanvasItemClass {
	EelCanvasItemClass parent_class;
};

/* not namespaced due to their length */
typedef enum {
	BOUNDS_USAGE_FOR_LAYOUT,
	BOUNDS_USAGE_FOR_ENTIRE_ITEM,
	BOUNDS_USAGE_FOR_DISPLAY
} AthenaIconCanvasItemBoundsUsage;

/* GObject */
GType       athena_icon_canvas_item_get_type                 (void);

/* attributes */
void        athena_icon_canvas_item_set_image                (AthenaIconCanvasItem       *item,
								GdkPixbuf                    *image);
cairo_surface_t* athena_icon_canvas_item_get_drag_surface    (AthenaIconCanvasItem       *item);
void        athena_icon_canvas_item_set_emblems              (AthenaIconCanvasItem       *item,
								GList                        *emblem_pixbufs);
void        athena_icon_canvas_item_set_show_stretch_handles (AthenaIconCanvasItem       *item,
								gboolean                      show_stretch_handles);
void        athena_icon_canvas_item_set_attach_points        (AthenaIconCanvasItem       *item,
								GdkPoint                     *attach_points,
								int                           n_attach_points);
void        athena_icon_canvas_item_set_embedded_text_rect   (AthenaIconCanvasItem       *item,
								const GdkRectangle           *text_rect);
void        athena_icon_canvas_item_set_embedded_text        (AthenaIconCanvasItem       *item,
								const char                   *text);
double      athena_icon_canvas_item_get_max_text_width       (AthenaIconCanvasItem       *item);
const char *athena_icon_canvas_item_get_editable_text        (AthenaIconCanvasItem       *icon_item);
void        athena_icon_canvas_item_set_renaming             (AthenaIconCanvasItem       *icon_item,
								gboolean                      state);

/* geometry and hit testing */
gboolean    athena_icon_canvas_item_hit_test_rectangle       (AthenaIconCanvasItem       *item,
								EelIRect                      canvas_rect);
gboolean    athena_icon_canvas_item_hit_test_stretch_handles (AthenaIconCanvasItem       *item,
								gdouble                       world_x,
								gdouble                       world_y,
								GtkCornerType                *corner);
void        athena_icon_canvas_item_invalidate_label         (AthenaIconCanvasItem       *item);
void        athena_icon_canvas_item_invalidate_label_size    (AthenaIconCanvasItem       *item);
EelDRect    athena_icon_canvas_item_get_icon_rectangle       (const AthenaIconCanvasItem *item);
EelDRect    athena_icon_canvas_item_get_text_rectangle       (AthenaIconCanvasItem       *item,
								gboolean                      for_layout);
void        athena_icon_canvas_item_get_bounds_for_layout    (AthenaIconCanvasItem       *item,
								double *x1, double *y1, double *x2, double *y2);
void        athena_icon_canvas_item_get_bounds_for_entire_item (AthenaIconCanvasItem       *item,
								  double *x1, double *y1, double *x2, double *y2);
void        athena_icon_canvas_item_update_bounds            (AthenaIconCanvasItem       *item,
								double i2w_dx, double i2w_dy);
void        athena_icon_canvas_item_set_is_visible           (AthenaIconCanvasItem       *item,
								gboolean                      visible);
/* whether the entire label text must be visible at all times */
void        athena_icon_canvas_item_set_entire_text          (AthenaIconCanvasItem       *icon_item,
								gboolean                      entire_text);

G_END_DECLS

#endif /* ATHENA_ICON_CANVAS_ITEM_H */
