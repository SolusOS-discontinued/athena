/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* athena-view.h
 *
 * Copyright (C) 1999, 2000  Free Software Foundaton
 * Copyright (C) 2000, 2001  Eazel, Inc.
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
 * Authors: Ettore Perazzoli
 * 	    Darin Adler <darin@bentspoon.com>
 * 	    John Sullivan <sullivan@eazel.com>
 *          Pavel Cisler <pavel@eazel.com>
 */

#ifndef ATHENA_VIEW_H
#define ATHENA_VIEW_H

#include <gtk/gtk.h>
#include <gio/gio.h>

#include <libathena-private/athena-directory.h>
#include <libathena-private/athena-file.h>
#include <libathena-private/athena-icon-container.h>
#include <libathena-private/athena-link.h>

typedef struct AthenaView AthenaView;
typedef struct AthenaViewClass AthenaViewClass;

#include "athena-window.h"
#include "athena-window-slot.h"

#define ATHENA_TYPE_VIEW athena_view_get_type()
#define ATHENA_VIEW(obj)\
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_VIEW, AthenaView))
#define ATHENA_VIEW_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_VIEW, AthenaViewClass))
#define ATHENA_IS_VIEW(obj)\
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_VIEW))
#define ATHENA_IS_VIEW_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_VIEW))
#define ATHENA_VIEW_GET_CLASS(obj)\
	(G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_VIEW, AthenaViewClass))

typedef struct AthenaViewDetails AthenaViewDetails;

struct AthenaView {
	GtkScrolledWindow parent;

	AthenaViewDetails *details;
};

struct AthenaViewClass {
	GtkScrolledWindowClass parent_class;

	/* The 'clear' signal is emitted to empty the view of its contents.
	 * It must be replaced by each subclass.
	 */
	void 	(* clear) 		 (AthenaView *view);
	
	/* The 'begin_file_changes' signal is emitted before a set of files
	 * are added to the view. It can be replaced by a subclass to do any 
	 * necessary preparation for a set of new files. The default
	 * implementation does nothing.
	 */
	void 	(* begin_file_changes) (AthenaView *view);
	
	/* The 'add_file' signal is emitted to add one file to the view.
	 * It must be replaced by each subclass.
	 */
	void    (* add_file) 		 (AthenaView *view, 
					  AthenaFile *file,
					  AthenaDirectory *directory);
	void    (* remove_file)		 (AthenaView *view, 
					  AthenaFile *file,
					  AthenaDirectory *directory);

	/* The 'file_changed' signal is emitted to signal a change in a file,
	 * including the file being removed.
	 * It must be replaced by each subclass.
	 */
	void 	(* file_changed)         (AthenaView *view, 
					  AthenaFile *file,
					  AthenaDirectory *directory);

	/* The 'end_file_changes' signal is emitted after a set of files
	 * are added to the view. It can be replaced by a subclass to do any 
	 * necessary cleanup (typically, cleanup for code in begin_file_changes).
	 * The default implementation does nothing.
	 */
	void 	(* end_file_changes)    (AthenaView *view);
	
	/* The 'begin_loading' signal is emitted before any of the contents
	 * of a directory are added to the view. It can be replaced by a 
	 * subclass to do any necessary preparation to start dealing with a
	 * new directory. The default implementation does nothing.
	 */
	void 	(* begin_loading) 	 (AthenaView *view);

	/* The 'end_loading' signal is emitted after all of the contents
	 * of a directory are added to the view. It can be replaced by a 
	 * subclass to do any necessary clean-up. The default implementation 
	 * does nothing.
	 *
	 * If all_files_seen is true, the handler may assume that
	 * no load error ocurred, and all files of the underlying
	 * directory were loaded.
	 *
	 * Otherwise, end_loading was emitted due to cancellation,
	 * which usually means that not all files are available.
	 */
	void 	(* end_loading) 	 (AthenaView *view,
					  gboolean all_files_seen);

	/* The 'load_error' signal is emitted when the directory model
	 * reports an error in the process of monitoring the directory's
	 * contents.  The load error indicates that the process of 
	 * loading the contents has ended, but the directory is still
	 * being monitored. The default implementation handles common
	 * load failures like ACCESS_DENIED.
	 */
	void    (* load_error)           (AthenaView *view,
					  GError *error);

	/* Function pointers that don't have corresponding signals */

        /* reset_to_defaults is a function pointer that subclasses must 
         * override to set sort order, zoom level, etc to match default
         * values. 
         */
        void     (* reset_to_defaults)	         (AthenaView *view);

	/* get_backing uri is a function pointer for subclasses to
	 * override. Subclasses may replace it with a function that
	 * returns the URI for the location where to create new folders,
	 * files, links and paste the clipboard to.
	 */

	char *	(* get_backing_uri)		(AthenaView *view);

	/* get_selection is not a signal; it is just a function pointer for
	 * subclasses to replace (override). Subclasses must replace it
	 * with a function that returns a newly-allocated GList of
	 * AthenaFile pointers.
	 */
	GList *	(* get_selection) 	 	(AthenaView *view);
	
	/* get_selection_for_file_transfer  is a function pointer for
	 * subclasses to replace (override). Subclasses must replace it
	 * with a function that returns a newly-allocated GList of
	 * AthenaFile pointers. The difference from get_selection is
	 * that any files in the selection that also has a parent folder
	 * in the selection is not included.
	 */
	GList *	(* get_selection_for_file_transfer)(AthenaView *view);
	
        /* select_all is a function pointer that subclasses must override to
         * select all of the items in the view */
        void     (* select_all)	         	(AthenaView *view);

        /* set_selection is a function pointer that subclasses must
         * override to select the specified items (and unselect all
         * others). The argument is a list of AthenaFiles. */

        void     (* set_selection)	 	(AthenaView *view, 
        					 GList *selection);
        					 
        /* invert_selection is a function pointer that subclasses must
         * override to invert selection. */

        void     (* invert_selection)	 	(AthenaView *view);        					 

	/* Return an array of locations of selected icons in their view. */
	GArray * (* get_selected_icon_locations) (AthenaView *view);

	guint    (* get_item_count)             (AthenaView *view);

        /* bump_zoom_level is a function pointer that subclasses must override
         * to change the zoom level of an object. */
        void    (* bump_zoom_level)      	(AthenaView *view,
					  	 int zoom_increment);

        /* zoom_to_level is a function pointer that subclasses must override
         * to set the zoom level of an object to the specified level. */
        void    (* zoom_to_level) 		(AthenaView *view, 
        				         AthenaZoomLevel level);

        AthenaZoomLevel (* get_zoom_level)    (AthenaView *view);

	/* restore_default_zoom_level is a function pointer that subclasses must override
         * to restore the zoom level of an object to a default setting. */
        void    (* restore_default_zoom_level) (AthenaView *view);

        /* can_zoom_in is a function pointer that subclasses must override to
         * return whether the view is at maximum size (furthest-in zoom level) */
        gboolean (* can_zoom_in)	 	(AthenaView *view);

        /* can_zoom_out is a function pointer that subclasses must override to
         * return whether the view is at minimum size (furthest-out zoom level) */
        gboolean (* can_zoom_out)	 	(AthenaView *view);
        
        /* reveal_selection is a function pointer that subclasses may
         * override to make sure the selected items are sufficiently
         * apparent to the user (e.g., scrolled into view). By default,
         * this does nothing.
         */
        void     (* reveal_selection)	 	(AthenaView *view);

        /* merge_menus is a function pointer that subclasses can override to
         * add their own menu items to the window's menu bar.
         * If overridden, subclasses must call parent class's function.
         */
        void    (* merge_menus)         	(AthenaView *view);
        void    (* unmerge_menus)         	(AthenaView *view);

        /* update_menus is a function pointer that subclasses can override to
         * update the sensitivity or wording of menu items in the menu bar.
         * It is called (at least) whenever the selection changes. If overridden, 
         * subclasses must call parent class's function.
         */
        void    (* update_menus)         	(AthenaView *view);

	/* sort_files is a function pointer that subclasses can override
	 * to provide a sorting order to determine which files should be
	 * presented when only a partial list is provided.
	 */
	int     (* compare_files)              (AthenaView *view,
						AthenaFile    *a,
						AthenaFile    *b);

	/* using_manual_layout is a function pointer that subclasses may
	 * override to control whether or not items can be freely positioned
	 * on the user-visible area.
	 * Note that this value is not guaranteed to be constant within the
	 * view's lifecycle. */
	gboolean (* using_manual_layout)     (AthenaView *view);

	/* is_read_only is a function pointer that subclasses may
	 * override to control whether or not the user is allowed to
	 * change the contents of the currently viewed directory. The
	 * default implementation checks the permissions of the
	 * directory.
	 */
	gboolean (* is_read_only)	        (AthenaView *view);

	/* is_empty is a function pointer that subclasses must
	 * override to report whether the view contains any items.
	 */
	gboolean (* is_empty)                   (AthenaView *view);

	gboolean (* can_rename_file)            (AthenaView *view,
						 AthenaFile *file);
	/* select_all specifies whether the whole filename should be selected
	 * or only its basename (i.e. everything except the extension)
	 * */
	void	 (* start_renaming_file)        (AthenaView *view,
					  	 AthenaFile *file,
						 gboolean select_all);

	/* convert *point from widget's coordinate system to a coordinate
	 * system used for specifying file operation positions, which is view-specific.
	 *
	 * This is used by the the icon view, which converts the screen position to a zoom
	 * level-independent coordinate system.
	 */
	void (* widget_to_file_operation_position) (AthenaView *view,
						    GdkPoint     *position);

	/* Preference change callbacks, overriden by icon and list views. 
	 * Icon and list views respond by synchronizing to the new preference
	 * values and forcing an update if appropriate.
	 */
	void	(* click_policy_changed)	   (AthenaView *view);
	void	(* sort_directories_first_changed) (AthenaView *view);

	/* Get the id string for this view. Its a constant string, not memory managed */
	const char *   (* get_view_id)            (AthenaView          *view);

	/* Return the uri of the first visible file */	
	char *         (* get_first_visible_file) (AthenaView          *view);
	/* Scroll the view so that the file specified by the uri is at the top
	   of the view */
	void           (* scroll_to_file)	  (AthenaView          *view,
						   const char            *uri);

        /* Signals used only for keybindings */
        gboolean (* trash)                         (AthenaView *view);
        gboolean (* delete)                        (AthenaView *view);
};

/* GObject support */
GType               athena_view_get_type                         (void);

/* Functions callable from the user interface and elsewhere. */
AthenaWindow     *athena_view_get_athena_window              (AthenaView  *view);
AthenaWindowSlot *athena_view_get_athena_window_slot     (AthenaView  *view);
char *              athena_view_get_uri                          (AthenaView  *view);

void                athena_view_display_selection_info           (AthenaView  *view);

GdkAtom	            athena_view_get_copied_files_atom            (AthenaView  *view);
gboolean            athena_view_get_active                       (AthenaView  *view);

/* Wrappers for signal emitters. These are normally called 
 * only by AthenaView itself. They have corresponding signals
 * that observers might want to connect with.
 */
gboolean            athena_view_get_loading                      (AthenaView  *view);

/* Hooks for subclasses to call. These are normally called only by 
 * AthenaView and its subclasses 
 */
void                athena_view_activate_files                   (AthenaView        *view,
								    GList                  *files,
								    AthenaWindowOpenFlags flags,
								    gboolean                confirm_multiple);
void                athena_view_preview_files                    (AthenaView        *view,
								    GList               *files,
								    GArray              *locations);
void                athena_view_start_batching_selection_changes (AthenaView  *view);
void                athena_view_stop_batching_selection_changes  (AthenaView  *view);
void                athena_view_notify_selection_changed         (AthenaView  *view);
GtkUIManager *      athena_view_get_ui_manager                   (AthenaView  *view);
AthenaDirectory  *athena_view_get_model                        (AthenaView  *view);
AthenaFile       *athena_view_get_directory_as_file            (AthenaView  *view);
void                athena_view_pop_up_background_context_menu   (AthenaView  *view,
								    GdkEventButton   *event);
void                athena_view_pop_up_selection_context_menu    (AthenaView  *view,
								    GdkEventButton   *event); 
gboolean            athena_view_should_show_file                 (AthenaView  *view,
								    AthenaFile     *file);
gboolean	    athena_view_should_sort_directories_first    (AthenaView  *view);
void                athena_view_ignore_hidden_file_preferences   (AthenaView  *view);
void                athena_view_set_show_foreign                 (AthenaView  *view,
								    gboolean          show_foreign);
gboolean            athena_view_handle_scroll_event              (AthenaView  *view,
								    GdkEventScroll   *event);

void                athena_view_freeze_updates                   (AthenaView  *view);
void                athena_view_unfreeze_updates                 (AthenaView  *view);
gboolean            athena_view_get_is_renaming                  (AthenaView  *view);
void                athena_view_set_is_renaming                  (AthenaView  *view,
								    gboolean       renaming);
void                athena_view_add_subdirectory                (AthenaView  *view,
								   AthenaDirectory*directory);
void                athena_view_remove_subdirectory             (AthenaView  *view,
								   AthenaDirectory*directory);

gboolean            athena_view_is_editable                     (AthenaView *view);

/* AthenaView methods */
const char *      athena_view_get_view_id                (AthenaView      *view);

/* file operations */
char *            athena_view_get_backing_uri            (AthenaView      *view);
void              athena_view_move_copy_items            (AthenaView      *view,
							    const GList       *item_uris,
							    GArray            *relative_item_points,
							    const char        *target_uri,
							    int                copy_action,
							    int                x,
							    int                y);
void              athena_view_new_file_with_initial_contents (AthenaView *view,
								const char *parent_uri,
								const char *filename,
								const char *initial_contents,
								int length,
								GdkPoint *pos);

/* selection handling */
int               athena_view_get_selection_count        (AthenaView      *view);
GList *           athena_view_get_selection              (AthenaView      *view);
void              athena_view_set_selection              (AthenaView      *view,
							    GList             *selection);


void              athena_view_load_location              (AthenaView      *view,
							    GFile             *location);
void              athena_view_stop_loading               (AthenaView      *view);

char **           athena_view_get_emblem_names_to_exclude (AthenaView     *view);
char *            athena_view_get_first_visible_file     (AthenaView      *view);
void              athena_view_scroll_to_file             (AthenaView      *view,
							    const char        *uri);
char *            athena_view_get_title                  (AthenaView      *view);
gboolean          athena_view_supports_zooming           (AthenaView      *view);
void              athena_view_bump_zoom_level            (AthenaView      *view,
							    int                zoom_increment);
void              athena_view_zoom_to_level              (AthenaView      *view,
							    AthenaZoomLevel  level);
void              athena_view_restore_default_zoom_level (AthenaView      *view);
gboolean          athena_view_can_zoom_in                (AthenaView      *view);
gboolean          athena_view_can_zoom_out               (AthenaView      *view);
AthenaZoomLevel athena_view_get_zoom_level             (AthenaView      *view);
void              athena_view_pop_up_location_context_menu (AthenaView    *view,
							      GdkEventButton  *event,
							      const char      *location);
void              athena_view_grab_focus                 (AthenaView      *view);
void              athena_view_update_menus               (AthenaView      *view);

#endif /* ATHENA_VIEW_H */
