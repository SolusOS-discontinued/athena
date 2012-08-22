/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Athena - Canvas item for floating selection.
 *
 * Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation
 * Copyright (C) 2011 Red Hat Inc.
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
 *
 * Authors: Federico Mena <federico@nuclecu.unam.mx>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 */

#ifndef __ATHENA_SELECTION_CANVAS_ITEM_H__
#define __ATHENA_SELECTION_CANVAS_ITEM_H__

#include <eel/eel-canvas.h>

G_BEGIN_DECLS

#define ATHENA_TYPE_SELECTION_CANVAS_ITEM athena_selection_canvas_item_get_type()
#define ATHENA_SELECTION_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_SELECTION_CANVAS_ITEM, AthenaSelectionCanvasItem))
#define ATHENA_SELECTION_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_SELECTION_CANVAS_ITEM, AthenaSelectionCanvasItemClass))
#define ATHENA_IS_SELECTION_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_SELECTION_CANVAS_ITEM))
#define ATHENA_IS_SELECTION_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_SELECTION_CANVAS_ITEM))
#define ATHENA_SELECTION_CANVAS_ITEM_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_SELECTION_CANVAS_ITEM, AthenaSelectionCanvasItemClass))

typedef struct _AthenaSelectionCanvasItem AthenaSelectionCanvasItem;
typedef struct _AthenaSelectionCanvasItemClass AthenaSelectionCanvasItemClass;
typedef struct _AthenaSelectionCanvasItemDetails AthenaSelectionCanvasItemDetails;

struct _AthenaSelectionCanvasItem {
	EelCanvasItem item;
	AthenaSelectionCanvasItemDetails *priv;
	gpointer user_data;
};

struct _AthenaSelectionCanvasItemClass {
	EelCanvasItemClass parent_class;
};

/* GObject */
GType       athena_selection_canvas_item_get_type                 (void);

void athena_selection_canvas_item_fade_out (AthenaSelectionCanvasItem *self,
					      guint transition_time);

G_END_DECLS

#endif /* __ATHENA_SELECTION_CANVAS_ITEM_H__ */
