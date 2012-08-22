/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-list-model.h - a GtkTreeModel for file lists. 

   Copyright (C) 2001, 2002 Anders Carlsson

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

   Authors: Anders Carlsson <andersca@gnu.org>
*/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <libathena-private/athena-file.h>
#include <libathena-private/athena-directory.h>
#include <libathena-extension/athena-column.h>

#ifndef ATHENA_LIST_MODEL_H
#define ATHENA_LIST_MODEL_H

#define ATHENA_TYPE_LIST_MODEL athena_list_model_get_type()
#define ATHENA_LIST_MODEL(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_LIST_MODEL, AthenaListModel))
#define ATHENA_LIST_MODEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_LIST_MODEL, AthenaListModelClass))
#define ATHENA_IS_LIST_MODEL(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_LIST_MODEL))
#define ATHENA_IS_LIST_MODEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_LIST_MODEL))
#define ATHENA_LIST_MODEL_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_LIST_MODEL, AthenaListModelClass))

enum {
	ATHENA_LIST_MODEL_FILE_COLUMN,
	ATHENA_LIST_MODEL_SUBDIRECTORY_COLUMN,
	ATHENA_LIST_MODEL_SMALLEST_ICON_COLUMN,
	ATHENA_LIST_MODEL_SMALLER_ICON_COLUMN,
	ATHENA_LIST_MODEL_SMALL_ICON_COLUMN,
	ATHENA_LIST_MODEL_STANDARD_ICON_COLUMN,
	ATHENA_LIST_MODEL_LARGE_ICON_COLUMN,
	ATHENA_LIST_MODEL_LARGER_ICON_COLUMN,
	ATHENA_LIST_MODEL_LARGEST_ICON_COLUMN,
	ATHENA_LIST_MODEL_FILE_NAME_IS_EDITABLE_COLUMN,
	ATHENA_LIST_MODEL_NUM_COLUMNS
};

typedef struct AthenaListModelDetails AthenaListModelDetails;

typedef struct AthenaListModel {
	GObject parent_instance;
	AthenaListModelDetails *details;
} AthenaListModel;

typedef struct {
	GObjectClass parent_class;

	void (* subdirectory_unloaded)(AthenaListModel *model,
				       AthenaDirectory *subdirectory);
} AthenaListModelClass;

GType    athena_list_model_get_type                          (void);
gboolean athena_list_model_add_file                          (AthenaListModel          *model,
								AthenaFile         *file,
								AthenaDirectory    *directory);
void     athena_list_model_file_changed                      (AthenaListModel          *model,
								AthenaFile         *file,
								AthenaDirectory    *directory);
gboolean athena_list_model_is_empty                          (AthenaListModel          *model);
guint    athena_list_model_get_length                        (AthenaListModel          *model);
void     athena_list_model_remove_file                       (AthenaListModel          *model,
								AthenaFile         *file,
								AthenaDirectory    *directory);
void     athena_list_model_clear                             (AthenaListModel          *model);
gboolean athena_list_model_get_tree_iter_from_file           (AthenaListModel          *model,
								AthenaFile         *file,
								AthenaDirectory    *directory,
								GtkTreeIter          *iter);
GList *  athena_list_model_get_all_iters_for_file            (AthenaListModel          *model,
								AthenaFile         *file);
gboolean athena_list_model_get_first_iter_for_file           (AthenaListModel          *model,
								AthenaFile         *file,
								GtkTreeIter          *iter);
void     athena_list_model_set_should_sort_directories_first (AthenaListModel          *model,
								gboolean              sort_directories_first);

int      athena_list_model_get_sort_column_id_from_attribute (AthenaListModel *model,
								GQuark       attribute);
GQuark   athena_list_model_get_attribute_from_sort_column_id (AthenaListModel *model,
								int sort_column_id);
void     athena_list_model_sort_files                        (AthenaListModel *model,
								GList **files);

AthenaZoomLevel athena_list_model_get_zoom_level_from_column_id (int               column);
int               athena_list_model_get_column_id_from_zoom_level (AthenaZoomLevel zoom_level);

AthenaFile *    athena_list_model_file_for_path (AthenaListModel *model, GtkTreePath *path);
gboolean          athena_list_model_load_subdirectory (AthenaListModel *model, GtkTreePath *path, AthenaDirectory **directory);
void              athena_list_model_unload_subdirectory (AthenaListModel *model, GtkTreeIter *iter);

void              athena_list_model_set_drag_view (AthenaListModel *model,
						     GtkTreeView *view,
						     int begin_x, 
						     int begin_y);

GtkTargetList *   athena_list_model_get_drag_target_list (void);

int               athena_list_model_compare_func (AthenaListModel *model,
						    AthenaFile *file1,
						    AthenaFile *file2);


int               athena_list_model_add_column (AthenaListModel *model,
						  AthenaColumn *column);
int               athena_list_model_get_column_number (AthenaListModel *model,
							 const char *column_name);

void              athena_list_model_subdirectory_done_loading (AthenaListModel       *model,
								 AthenaDirectory *directory);

void              athena_list_model_set_highlight_for_files (AthenaListModel *model,
							       GList *files);
						   
#endif /* ATHENA_LIST_MODEL_H */
