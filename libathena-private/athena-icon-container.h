/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* gnome-icon-container.h - Icon container widget.

   Copyright (C) 1999, 2000 Free Software Foundation
   Copyright (C) 2000 Eazel, Inc.

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

   Authors: Ettore Perazzoli <ettore@gnu.org>, Darin Adler <darin@bentspoon.com>
*/

#ifndef ATHENA_ICON_CONTAINER_H
#define ATHENA_ICON_CONTAINER_H

#include <eel/eel-canvas.h>
#include <libathena-private/athena-icon-info.h>

#define ATHENA_TYPE_ICON_CONTAINER athena_icon_container_get_type()
#define ATHENA_ICON_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_ICON_CONTAINER, AthenaIconContainer))
#define ATHENA_ICON_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_ICON_CONTAINER, AthenaIconContainerClass))
#define ATHENA_IS_ICON_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_ICON_CONTAINER))
#define ATHENA_IS_ICON_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_ICON_CONTAINER))
#define ATHENA_ICON_CONTAINER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_ICON_CONTAINER, AthenaIconContainerClass))


#define ATHENA_ICON_CONTAINER_ICON_DATA(pointer) \
	((AthenaIconData *) (pointer))

typedef struct AthenaIconData AthenaIconData;

typedef void (* AthenaIconCallback) (AthenaIconData *icon_data,
				       gpointer callback_data);

typedef struct {
	int x;
	int y;
	double scale;
} AthenaIconPosition;

typedef enum {
	ATHENA_ICON_LAYOUT_L_R_T_B,
	ATHENA_ICON_LAYOUT_R_L_T_B,
	ATHENA_ICON_LAYOUT_T_B_L_R,
	ATHENA_ICON_LAYOUT_T_B_R_L
} AthenaIconLayoutMode;

typedef enum {
	ATHENA_ICON_LABEL_POSITION_UNDER,
	ATHENA_ICON_LABEL_POSITION_BESIDE
} AthenaIconLabelPosition;

#define	ATHENA_ICON_CONTAINER_TYPESELECT_FLUSH_DELAY 1000000

typedef struct AthenaIconContainerDetails AthenaIconContainerDetails;

typedef struct {
	EelCanvas canvas;
	AthenaIconContainerDetails *details;
} AthenaIconContainer;

typedef struct {
	EelCanvasClass parent_slot;

	/* Operations on the container. */
	int          (* button_press) 	          (AthenaIconContainer *container,
						   GdkEventButton *event);
	void         (* context_click_background) (AthenaIconContainer *container,
						   GdkEventButton *event);
	void         (* middle_click) 		  (AthenaIconContainer *container,
						   GdkEventButton *event);

	/* Operations on icons. */
	void         (* activate)	  	  (AthenaIconContainer *container,
						   AthenaIconData *data);
	void         (* activate_alternate)       (AthenaIconContainer *container,
						   AthenaIconData *data);
	void         (* activate_previewer)       (AthenaIconContainer *container,
						   GList *files,
						   GArray *locations);
	void         (* context_click_selection)  (AthenaIconContainer *container,
						   GdkEventButton *event);
	void	     (* move_copy_items)	  (AthenaIconContainer *container,
						   const GList *item_uris,
						   GdkPoint *relative_item_points,
						   const char *target_uri,
						   GdkDragAction action,
						   int x,
						   int y);
	void	     (* handle_netscape_url)	  (AthenaIconContainer *container,
						   const char *url,
						   const char *target_uri,
						   GdkDragAction action,
						   int x,
						   int y);
	void	     (* handle_uri_list)    	  (AthenaIconContainer *container,
						   const char *uri_list,
						   const char *target_uri,
						   GdkDragAction action,
						   int x,
						   int y);
	void	     (* handle_text)		  (AthenaIconContainer *container,
						   const char *text,
						   const char *target_uri,
						   GdkDragAction action,
						   int x,
						   int y);
	void	     (* handle_raw)		  (AthenaIconContainer *container,
						   char *raw_data,
						   int length,
						   const char *target_uri,
						   const char *direct_save_uri,
						   GdkDragAction action,
						   int x,
						   int y);

	/* Queries on the container for subclass/client.
	 * These must be implemented. The default "do nothing" is not good enough.
	 */
	char *	     (* get_container_uri)	  (AthenaIconContainer *container);

	/* Queries on icons for subclass/client.
	 * These must be implemented. The default "do nothing" is not
	 * good enough, these are _not_ signals.
	 */
	AthenaIconInfo *(* get_icon_images)     (AthenaIconContainer *container,
						   AthenaIconData *data,
						   int icon_size,
						   char **embedded_text,
						   gboolean for_drag_accept,
						   gboolean need_large_embeddded_text,
						   gboolean *embedded_text_needs_loading,
						   gboolean *has_window_open);
	void         (* get_icon_text)            (AthenaIconContainer *container,
						   AthenaIconData *data,
						   char **editable_text,
						   char **additional_text,
						   gboolean include_invisible);
	char *       (* get_icon_description)     (AthenaIconContainer *container,
						   AthenaIconData *data);
	int          (* compare_icons)            (AthenaIconContainer *container,
						   AthenaIconData *icon_a,
						   AthenaIconData *icon_b);
	int          (* compare_icons_by_name)    (AthenaIconContainer *container,
						   AthenaIconData *icon_a,
						   AthenaIconData *icon_b);
	void         (* freeze_updates)           (AthenaIconContainer *container);
	void         (* unfreeze_updates)         (AthenaIconContainer *container);
	void         (* start_monitor_top_left)   (AthenaIconContainer *container,
						   AthenaIconData *data,
						   gconstpointer client,
						   gboolean large_text);
	void         (* stop_monitor_top_left)    (AthenaIconContainer *container,
						   AthenaIconData *data,
						   gconstpointer client);
	void         (* prioritize_thumbnailing)  (AthenaIconContainer *container,
						   AthenaIconData *data);

	/* Queries on icons for subclass/client.
	 * These must be implemented => These are signals !
	 * The default "do nothing" is not good enough.
	 */
	gboolean     (* can_accept_item)	  (AthenaIconContainer *container,
						   AthenaIconData *target, 
						   const char *item_uri);
	gboolean     (* get_stored_icon_position) (AthenaIconContainer *container,
						   AthenaIconData *data,
						   AthenaIconPosition *position);
	char *       (* get_icon_uri)             (AthenaIconContainer *container,
						   AthenaIconData *data);
	char *       (* get_icon_drop_target_uri) (AthenaIconContainer *container,
						   AthenaIconData *data);

	/* If icon data is NULL, the layout timestamp of the container should be retrieved.
	 * That is the time when the container displayed a fully loaded directory with
	 * all icon positions assigned.
	 *
	 * If icon data is not NULL, the position timestamp of the icon should be retrieved.
	 * That is the time when the file (i.e. icon data payload) was last displayed in a
	 * fully loaded directory with all icon positions assigned.
	 */
	gboolean     (* get_stored_layout_timestamp) (AthenaIconContainer *container,
						      AthenaIconData *data,
						      time_t *time);
	/* If icon data is NULL, the layout timestamp of the container should be stored.
	 * If icon data is not NULL, the position timestamp of the container should be stored.
	 */
	gboolean     (* store_layout_timestamp) (AthenaIconContainer *container,
						 AthenaIconData *data,
						 const time_t *time);

	/* Notifications for the whole container. */
	void	     (* band_select_started)	  (AthenaIconContainer *container);
	void	     (* band_select_ended)	  (AthenaIconContainer *container);
	void         (* selection_changed) 	  (AthenaIconContainer *container);
	void         (* layout_changed)           (AthenaIconContainer *container);

	/* Notifications for icons. */
	void         (* icon_position_changed)    (AthenaIconContainer *container,
						   AthenaIconData *data,
						   const AthenaIconPosition *position);
	void         (* icon_rename_started)      (AthenaIconContainer *container,
						   GtkWidget *renaming_widget);
	void         (* icon_rename_ended)        (AthenaIconContainer *container,
						   AthenaIconData *data,
						   const char *text);
	void	     (* icon_stretch_started)     (AthenaIconContainer *container,
						   AthenaIconData *data);
	void	     (* icon_stretch_ended)       (AthenaIconContainer *container,
						   AthenaIconData *data);
	int	     (* preview)		  (AthenaIconContainer *container,
						   AthenaIconData *data,
						   gboolean start_flag);
        void         (* icon_added)               (AthenaIconContainer *container,
                                                   AthenaIconData *data);
        void         (* icon_removed)             (AthenaIconContainer *container,
                                                   AthenaIconData *data);
        void         (* cleared)                  (AthenaIconContainer *container);
	gboolean     (* start_interactive_search) (AthenaIconContainer *container);
} AthenaIconContainerClass;

/* GtkObject */
GType             athena_icon_container_get_type                      (void);
GtkWidget *       athena_icon_container_new                           (void);


/* adding, removing, and managing icons */
void              athena_icon_container_clear                         (AthenaIconContainer  *view);
gboolean          athena_icon_container_add                           (AthenaIconContainer  *view,
									 AthenaIconData       *data);
void              athena_icon_container_layout_now                    (AthenaIconContainer *container);
gboolean          athena_icon_container_remove                        (AthenaIconContainer  *view,
									 AthenaIconData       *data);
void              athena_icon_container_for_each                      (AthenaIconContainer  *view,
									 AthenaIconCallback    callback,
									 gpointer                callback_data);
void              athena_icon_container_request_update                (AthenaIconContainer  *view,
									 AthenaIconData       *data);
void              athena_icon_container_request_update_all            (AthenaIconContainer  *container);
void              athena_icon_container_reveal                        (AthenaIconContainer  *container,
									 AthenaIconData       *data);
gboolean          athena_icon_container_is_empty                      (AthenaIconContainer  *container);
AthenaIconData *athena_icon_container_get_first_visible_icon        (AthenaIconContainer  *container);
void              athena_icon_container_scroll_to_icon                (AthenaIconContainer  *container,
									 AthenaIconData       *data);

void              athena_icon_container_begin_loading                 (AthenaIconContainer  *container);
void              athena_icon_container_end_loading                   (AthenaIconContainer  *container,
									 gboolean                all_icons_added);

/* control the layout */
gboolean          athena_icon_container_is_auto_layout                (AthenaIconContainer  *container);
void              athena_icon_container_set_auto_layout               (AthenaIconContainer  *container,
									 gboolean                auto_layout);

gboolean          athena_icon_container_is_keep_aligned               (AthenaIconContainer  *container);
void              athena_icon_container_set_keep_aligned              (AthenaIconContainer  *container,
									 gboolean                keep_aligned);
void              athena_icon_container_set_layout_mode               (AthenaIconContainer  *container,
									 AthenaIconLayoutMode  mode);
void              athena_icon_container_set_label_position            (AthenaIconContainer  *container,
									 AthenaIconLabelPosition pos);
void              athena_icon_container_sort                          (AthenaIconContainer  *container);
void              athena_icon_container_freeze_icon_positions         (AthenaIconContainer  *container);

int               athena_icon_container_get_max_layout_lines           (AthenaIconContainer  *container);
int               athena_icon_container_get_max_layout_lines_for_pango (AthenaIconContainer  *container);

void              athena_icon_container_set_highlighted_for_clipboard (AthenaIconContainer  *container,
									 GList                  *clipboard_icon_data);

/* operations on all icons */
void              athena_icon_container_unselect_all                  (AthenaIconContainer  *view);
void              athena_icon_container_select_all                    (AthenaIconContainer  *view);


/* operations on the selection */
GList     *       athena_icon_container_get_selection                 (AthenaIconContainer  *view);
void			  athena_icon_container_invert_selection				(AthenaIconContainer  *view);
void              athena_icon_container_set_selection                 (AthenaIconContainer  *view,
									 GList                  *selection);
GArray    *       athena_icon_container_get_selected_icon_locations   (AthenaIconContainer  *view);
gboolean          athena_icon_container_has_stretch_handles           (AthenaIconContainer  *container);
gboolean          athena_icon_container_is_stretched                  (AthenaIconContainer  *container);
void              athena_icon_container_show_stretch_handles          (AthenaIconContainer  *container);
void              athena_icon_container_unstretch                     (AthenaIconContainer  *container);
void              athena_icon_container_start_renaming_selected_item  (AthenaIconContainer  *container,
									 gboolean                select_all);

/* options */
AthenaZoomLevel athena_icon_container_get_zoom_level                (AthenaIconContainer  *view);
void              athena_icon_container_set_zoom_level                (AthenaIconContainer  *view,
									 int                     new_zoom_level);
void              athena_icon_container_set_single_click_mode         (AthenaIconContainer  *container,
									 gboolean                single_click_mode);
void              athena_icon_container_enable_linger_selection       (AthenaIconContainer  *view,
									 gboolean                enable);
gboolean          athena_icon_container_get_is_fixed_size             (AthenaIconContainer  *container);
void              athena_icon_container_set_is_fixed_size             (AthenaIconContainer  *container,
									 gboolean                is_fixed_size);
gboolean          athena_icon_container_get_is_desktop                (AthenaIconContainer  *container);
void              athena_icon_container_set_is_desktop                (AthenaIconContainer  *container,
									 gboolean                is_desktop);
void              athena_icon_container_reset_scroll_region           (AthenaIconContainer  *container);
void              athena_icon_container_set_font                      (AthenaIconContainer  *container,
									 const char             *font); 
void              athena_icon_container_set_font_size_table           (AthenaIconContainer  *container,
									 const int               font_size_table[ATHENA_ZOOM_LEVEL_LARGEST + 1]);
void              athena_icon_container_set_margins                   (AthenaIconContainer  *container,
									 int                     left_margin,
									 int                     right_margin,
									 int                     top_margin,
									 int                     bottom_margin);
void              athena_icon_container_set_use_drop_shadows          (AthenaIconContainer  *container,
									 gboolean                use_drop_shadows);
char*             athena_icon_container_get_icon_description          (AthenaIconContainer  *container,
                                                                         AthenaIconData       *data);
gboolean          athena_icon_container_get_allow_moves               (AthenaIconContainer  *container);
void              athena_icon_container_set_allow_moves               (AthenaIconContainer  *container,
									 gboolean                allow_moves);
void		  athena_icon_container_set_forced_icon_size		(AthenaIconContainer  *container,
									 int                     forced_icon_size);
void		  athena_icon_container_set_all_columns_same_width	(AthenaIconContainer  *container,
									 gboolean                all_columns_same_width);

gboolean	  athena_icon_container_is_layout_rtl			(AthenaIconContainer  *container);
gboolean	  athena_icon_container_is_layout_vertical		(AthenaIconContainer  *container);

gboolean          athena_icon_container_get_store_layout_timestamps   (AthenaIconContainer  *container);
void              athena_icon_container_set_store_layout_timestamps   (AthenaIconContainer  *container,
									 gboolean                store_layout);

void              athena_icon_container_widget_to_file_operation_position (AthenaIconContainer *container,
									     GdkPoint              *position);

#define CANVAS_WIDTH(container,allocation) ((allocation.width	  \
				- container->details->left_margin \
				- container->details->right_margin) \
				/  EEL_CANVAS (container)->pixels_per_unit)

#define CANVAS_HEIGHT(container,allocation) ((allocation.height \
			 - container->details->top_margin \
			 - container->details->bottom_margin) \
			 / EEL_CANVAS (container)->pixels_per_unit)

#endif /* ATHENA_ICON_CONTAINER_H */
