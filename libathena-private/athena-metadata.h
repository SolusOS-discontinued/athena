/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-metadata.h: #defines and other metadata-related info
 
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
  
   Author: John Sullivan <sullivan@eazel.com>
*/

#ifndef ATHENA_METADATA_H
#define ATHENA_METADATA_H

/* Keys for getting/setting Athena metadata. All metadata used in Athena
 * should define its key here, so we can keep track of the whole set easily.
 * Any updates here needs to be added in athena-metadata.c too.
 */

#include <glib.h>

/* Per-file */

#define ATHENA_METADATA_KEY_DEFAULT_VIEW		 	"athena-default-view"

#define ATHENA_METADATA_KEY_LOCATION_BACKGROUND_COLOR 	"folder-background-color"
#define ATHENA_METADATA_KEY_LOCATION_BACKGROUND_IMAGE 	"folder-background-image"

#define ATHENA_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL       	"athena-icon-view-zoom-level"
#define ATHENA_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT      	"athena-icon-view-auto-layout"
#define ATHENA_METADATA_KEY_ICON_VIEW_SORT_BY          	"athena-icon-view-sort-by"
#define ATHENA_METADATA_KEY_ICON_VIEW_SORT_REVERSED    	"athena-icon-view-sort-reversed"
#define ATHENA_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED            "athena-icon-view-keep-aligned"
#define ATHENA_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP	"athena-icon-view-layout-timestamp"

#define ATHENA_METADATA_KEY_LIST_VIEW_ZOOM_LEVEL       	"athena-list-view-zoom-level"
#define ATHENA_METADATA_KEY_LIST_VIEW_SORT_COLUMN      	"athena-list-view-sort-column"
#define ATHENA_METADATA_KEY_LIST_VIEW_SORT_REVERSED    	"athena-list-view-sort-reversed"
#define ATHENA_METADATA_KEY_LIST_VIEW_VISIBLE_COLUMNS    	"athena-list-view-visible-columns"
#define ATHENA_METADATA_KEY_LIST_VIEW_COLUMN_ORDER    	"athena-list-view-column-order"

#define ATHENA_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL		"athena-compact-view-zoom-level"

#define ATHENA_METADATA_KEY_WINDOW_GEOMETRY			"athena-window-geometry"
#define ATHENA_METADATA_KEY_WINDOW_SCROLL_POSITION		"athena-window-scroll-position"
#define ATHENA_METADATA_KEY_WINDOW_SHOW_HIDDEN_FILES		"athena-window-show-hidden-files"
#define ATHENA_METADATA_KEY_WINDOW_MAXIMIZED			"athena-window-maximized"
#define ATHENA_METADATA_KEY_WINDOW_STICKY			"athena-window-sticky"
#define ATHENA_METADATA_KEY_WINDOW_KEEP_ABOVE			"athena-window-keep-above"

#define ATHENA_METADATA_KEY_SIDEBAR_BACKGROUND_COLOR   	"athena-sidebar-background-color"
#define ATHENA_METADATA_KEY_SIDEBAR_BACKGROUND_IMAGE   	"athena-sidebar-background-image"
#define ATHENA_METADATA_KEY_SIDEBAR_BUTTONS			"athena-sidebar-buttons"

#define ATHENA_METADATA_KEY_ICON_POSITION              	"athena-icon-position"
#define ATHENA_METADATA_KEY_ICON_POSITION_TIMESTAMP		"athena-icon-position-timestamp"
#define ATHENA_METADATA_KEY_ANNOTATION                 	"annotation"
#define ATHENA_METADATA_KEY_ICON_SCALE                 	"icon-scale"
#define ATHENA_METADATA_KEY_CUSTOM_ICON                	"custom-icon"
#define ATHENA_METADATA_KEY_CUSTOM_ICON_NAME                	"custom-icon-name"
#define ATHENA_METADATA_KEY_SCREEN				"screen"
#define ATHENA_METADATA_KEY_EMBLEMS				"emblems"

guint athena_metadata_get_id (const char *metadata);

#endif /* ATHENA_METADATA_H */
