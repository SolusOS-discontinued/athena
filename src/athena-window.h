/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Athena
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *
 *  Athena is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Athena is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Authors: Elliot Lee <sopwith@redhat.com>
 *           Darin Adler <darin@bentspoon.com>
 *
 */
/* athena-window.h: Interface of the main window object */

#ifndef ATHENA_WINDOW_H
#define ATHENA_WINDOW_H

#include <gtk/gtk.h>
#include <eel/eel-glib-extensions.h>
#include <libathena-private/athena-bookmark.h>
#include <libathena-private/athena-search-directory.h>

#include "athena-navigation-state.h"
#include "athena-view.h"
#include "athena-window-types.h"

#define ATHENA_TYPE_WINDOW athena_window_get_type()
#define ATHENA_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_WINDOW, AthenaWindow))
#define ATHENA_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_WINDOW, AthenaWindowClass))
#define ATHENA_IS_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_WINDOW))
#define ATHENA_IS_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_WINDOW))
#define ATHENA_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_WINDOW, AthenaWindowClass))

typedef enum {
        ATHENA_WINDOW_SHOW_HIDDEN_FILES_DEFAULT,
        ATHENA_WINDOW_SHOW_HIDDEN_FILES_ENABLE,
        ATHENA_WINDOW_SHOW_HIDDEN_FILES_DISABLE
} AthenaWindowShowHiddenFilesMode;

typedef enum {
        ATHENA_WINDOW_NOT_SHOWN,
        ATHENA_WINDOW_POSITION_SET,
        ATHENA_WINDOW_SHOULD_SHOW
} AthenaWindowShowState;

typedef enum {
	ATHENA_WINDOW_OPEN_SLOT_NONE = 0,
	ATHENA_WINDOW_OPEN_SLOT_APPEND = 1
}  AthenaWindowOpenSlotFlags;

#define ATHENA_WINDOW_SIDEBAR_PLACES "places"
#define ATHENA_WINDOW_SIDEBAR_TREE "tree"

typedef struct AthenaWindowDetails AthenaWindowDetails;

typedef struct {
        GtkWindowClass parent_spot;

	/* Function pointers for overriding, without corresponding signals */

        void   (* sync_title) (AthenaWindow *window,
			       AthenaWindowSlot *slot);
        AthenaIconInfo * (* get_icon) (AthenaWindow *window,
                                         AthenaWindowSlot *slot);

        void   (* prompt_for_location) (AthenaWindow *window, const char *initial);
        void   (* close) (AthenaWindow *window);

        /* Signals used only for keybindings */
        gboolean (* go_up)  (AthenaWindow *window,
                             gboolean close);
	void     (* reload) (AthenaWindow *window);
} AthenaWindowClass;

struct AthenaWindow {
        GtkWindow parent_object;
        
        AthenaWindowDetails *details;
};

GType            athena_window_get_type             (void);
void             athena_window_close                (AthenaWindow    *window);

void             athena_window_connect_content_view (AthenaWindow    *window,
						       AthenaView      *view);
void             athena_window_disconnect_content_view (AthenaWindow    *window,
							  AthenaView      *view);

void             athena_window_go_to                (AthenaWindow    *window,
                                                       GFile             *location);
void             athena_window_go_to_full           (AthenaWindow    *window,
                                                       GFile             *location,
                                                       AthenaWindowGoToCallback callback,
                                                       gpointer           user_data);
void             athena_window_new_tab              (AthenaWindow    *window);

GtkUIManager *   athena_window_get_ui_manager       (AthenaWindow    *window);
GtkActionGroup * athena_window_get_main_action_group (AthenaWindow   *window);
AthenaNavigationState * 
                 athena_window_get_navigation_state (AthenaWindow    *window);

void                 athena_window_report_load_complete     (AthenaWindow *window,
                                                               AthenaView *view);

AthenaWindowSlot * athena_window_get_extra_slot       (AthenaWindow *window);
AthenaWindowShowHiddenFilesMode
                     athena_window_get_hidden_files_mode (AthenaWindow *window);
void                 athena_window_set_hidden_files_mode (AthenaWindow *window,
                                                            AthenaWindowShowHiddenFilesMode  mode);
void                 athena_window_report_load_underway  (AthenaWindow *window,
                                                            AthenaView *view);
void                 athena_window_view_visible          (AthenaWindow *window,
                                                            AthenaView *view);
AthenaWindowSlot * athena_window_get_active_slot       (AthenaWindow *window);
void                 athena_window_push_status           (AthenaWindow *window,
                                                            const char *text);

void     athena_window_hide_sidebar         (AthenaWindow *window);
void     athena_window_show_sidebar         (AthenaWindow *window);

void     athena_window_hide_menubar         (AthenaWindow *window);
void     athena_window_show_menubar         (AthenaWindow *window);

void     athena_window_back_or_forward      (AthenaWindow *window,
                                               gboolean        back,
                                               guint           distance,
                                               gboolean        new_tab);
void     athena_window_split_view_on        (AthenaWindow *window);
void     athena_window_split_view_off       (AthenaWindow *window);
gboolean athena_window_split_view_showing   (AthenaWindow *window);

gboolean athena_window_disable_chrome_mapping (GValue *value,
                                                 GVariant *variant,
                                                 gpointer user_data);

#endif
