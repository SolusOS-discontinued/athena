/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* athena-global-preferences.h - Athena specific preference keys and
                                   functions.

   Copyright (C) 1999, 2000, 2001 Eazel, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Authors: Ramiro Estrugo <ramiro@eazel.com>
*/

#ifndef ATHENA_GLOBAL_PREFERENCES_H
#define ATHENA_GLOBAL_PREFERENCES_H

#include <libathena-private/athena-global-preferences.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/* Trash options */
#define ATHENA_PREFERENCES_CONFIRM_TRASH			"confirm-trash"
#define ATHENA_PREFERENCES_ENABLE_DELETE			"enable-delete"

/* Desktop options */
#define ATHENA_PREFERENCES_DESKTOP_IS_HOME_DIR                "desktop-is-home-dir"

/* Display  */
#define ATHENA_PREFERENCES_SHOW_HIDDEN_FILES			"show-hidden-files"
#define ATHENA_PREFERENCES_SHOW_ADVANCED_PERMISSIONS		"show-advanced-permissions"
#define ATHENA_PREFERENCES_DATE_FORMAT			"date-format"

/* Mouse */
#define ATHENA_PREFERENCES_MOUSE_USE_EXTRA_BUTTONS		"mouse-use-extra-buttons"
#define ATHENA_PREFERENCES_MOUSE_FORWARD_BUTTON		"mouse-forward-button"
#define ATHENA_PREFERENCES_MOUSE_BACK_BUTTON			"mouse-back-button"

typedef enum
{
	ATHENA_DATE_FORMAT_LOCALE,
	ATHENA_DATE_FORMAT_ISO,
	ATHENA_DATE_FORMAT_INFORMAL
} AthenaDateFormat;

typedef enum
{
	ATHENA_NEW_TAB_POSITION_AFTER_CURRENT_TAB,
	ATHENA_NEW_TAB_POSITION_END,
} AthenaNewTabPosition;

/* Sidebar panels  */
#define ATHENA_PREFERENCES_TREE_SHOW_ONLY_DIRECTORIES         "show-only-directories"

/* Single/Double click preference  */
#define ATHENA_PREFERENCES_CLICK_POLICY			"click-policy"

/* Activating executable text files */
#define ATHENA_PREFERENCES_EXECUTABLE_TEXT_ACTIVATION		"executable-text-activation"

/* Installing new packages when unknown mime type activated */
#define ATHENA_PREFERENCES_INSTALL_MIME_ACTIVATION		"install-mime-activation"

/* Spatial or browser mode */
#define ATHENA_PREFERENCES_ALWAYS_USE_BROWSER			"always-use-browser"
#define ATHENA_PREFERENCES_NEW_TAB_POSITION			"tabs-open-position"

#define ATHENA_PREFERENCES_ALWAYS_USE_LOCATION_ENTRY		"always-use-location-entry"
#define ATHENA_PREFERENCES_SHOW_UP_ICON_TOOLBAR		"show-up-icon-toolbar"
#define ATHENA_PREFERENCES_SHOW_EDIT_ICON_TOOLBAR		"show-edit-icon-toolbar"
#define ATHENA_PREFERENCES_SHOW_RELOAD_ICON_TOOLBAR		"show-reload-icon-toolbar"
#define ATHENA_PREFERENCES_SHOW_HOME_ICON_TOOLBAR		"show-home-icon-toolbar"
#define ATHENA_PREFERENCES_SHOW_COMPUTER_ICON_TOOLBAR		"show-computer-icon-toolbar"
#define ATHENA_PREFERENCES_SHOW_SEARCH_ICON_TOOLBAR		"show-search-icon-toolbar"

/* Which views should be displayed for new windows */
#define ATHENA_WINDOW_STATE_START_WITH_STATUS_BAR		"start-with-status-bar"
#define ATHENA_WINDOW_STATE_START_WITH_SIDEBAR		"start-with-sidebar"
#define ATHENA_WINDOW_STATE_START_WITH_TOOLBAR		"start-with-toolbar"
#define ATHENA_WINDOW_STATE_SIDE_PANE_VIEW                    "side-pane-view"
#define ATHENA_WINDOW_STATE_GEOMETRY				"geometry"
#define ATHENA_WINDOW_STATE_MAXIMIZED				"maximized"
#define ATHENA_WINDOW_STATE_SIDEBAR_WIDTH			"sidebar-width"

/* Sorting order */
#define ATHENA_PREFERENCES_SORT_DIRECTORIES_FIRST		"sort-directories-first"
#define ATHENA_PREFERENCES_DEFAULT_SORT_ORDER			"default-sort-order"
#define ATHENA_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER	"default-sort-in-reverse-order"

/* The default folder viewer - one of the two enums below */
#define ATHENA_PREFERENCES_DEFAULT_FOLDER_VIEWER		"default-folder-viewer"

enum
{
	ATHENA_DEFAULT_FOLDER_VIEWER_ICON_VIEW,
	ATHENA_DEFAULT_FOLDER_VIEWER_COMPACT_VIEW,
	ATHENA_DEFAULT_FOLDER_VIEWER_LIST_VIEW,
	ATHENA_DEFAULT_FOLDER_VIEWER_OTHER
};

/* These IIDs are used by the preferences code and in athena-application.c */
#define ATHENA_ICON_VIEW_IID		"OAFIID:Athena_File_Manager_Icon_View"
#define ATHENA_COMPACT_VIEW_IID	"OAFIID:Athena_File_Manager_Compact_View"
#define ATHENA_LIST_VIEW_IID		"OAFIID:Athena_File_Manager_List_View"


/* Icon View */
#define ATHENA_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL		"default-zoom-level"

#define ATHENA_PREFERENCES_ICON_VIEW_LABELS_BESIDE_ICONS		"labels-beside-icons"

/* Which text attributes appear beneath icon names */
#define ATHENA_PREFERENCES_ICON_VIEW_CAPTIONS				"captions"

/* The default size for thumbnail icons */
#define ATHENA_PREFERENCES_ICON_VIEW_THUMBNAIL_SIZE			"thumbnail-size"

/* ellipsization preferences */
#define ATHENA_PREFERENCES_ICON_VIEW_TEXT_ELLIPSIS_LIMIT		"text-ellipsis-limit"
#define ATHENA_PREFERENCES_DESKTOP_TEXT_ELLIPSIS_LIMIT		"text-ellipsis-limit"

/* Compact View */
#define ATHENA_PREFERENCES_COMPACT_VIEW_DEFAULT_ZOOM_LEVEL		"default-zoom-level"
#define ATHENA_PREFERENCES_COMPACT_VIEW_ALL_COLUMNS_SAME_WIDTH	"all-columns-have-same-width"

/* List View */
#define ATHENA_PREFERENCES_LIST_VIEW_DEFAULT_ZOOM_LEVEL		"default-zoom-level"
#define ATHENA_PREFERENCES_LIST_VIEW_DEFAULT_VISIBLE_COLUMNS		"default-visible-columns"
#define ATHENA_PREFERENCES_LIST_VIEW_DEFAULT_COLUMN_ORDER		"default-column-order"

enum
{
	ATHENA_CLICK_POLICY_SINGLE,
	ATHENA_CLICK_POLICY_DOUBLE
};

enum
{
	ATHENA_EXECUTABLE_TEXT_LAUNCH,
	ATHENA_EXECUTABLE_TEXT_DISPLAY,
	ATHENA_EXECUTABLE_TEXT_ASK
};

typedef enum
{
	ATHENA_SPEED_TRADEOFF_ALWAYS,
	ATHENA_SPEED_TRADEOFF_LOCAL_ONLY,
	ATHENA_SPEED_TRADEOFF_NEVER
} AthenaSpeedTradeoffValue;

#define ATHENA_PREFERENCES_SHOW_TEXT_IN_ICONS		"show-icon-text"
#define ATHENA_PREFERENCES_SHOW_DIRECTORY_ITEM_COUNTS "show-directory-item-counts"
#define ATHENA_PREFERENCES_SHOW_IMAGE_FILE_THUMBNAILS	"show-image-thumbnails"
#define ATHENA_PREFERENCES_IMAGE_FILE_THUMBNAIL_LIMIT	"thumbnail-limit"

typedef enum
{
	ATHENA_COMPLEX_SEARCH_BAR,
	ATHENA_SIMPLE_SEARCH_BAR
} AthenaSearchBarMode;

#define ATHENA_PREFERENCES_DESKTOP_FONT		   "font"
#define ATHENA_PREFERENCES_DESKTOP_HOME_VISIBLE          "home-icon-visible"
#define ATHENA_PREFERENCES_DESKTOP_HOME_NAME             "home-icon-name"
#define ATHENA_PREFERENCES_DESKTOP_COMPUTER_VISIBLE      "computer-icon-visible"
#define ATHENA_PREFERENCES_DESKTOP_COMPUTER_NAME         "computer-icon-name"
#define ATHENA_PREFERENCES_DESKTOP_TRASH_VISIBLE         "trash-icon-visible"
#define ATHENA_PREFERENCES_DESKTOP_TRASH_NAME            "trash-icon-name"
#define ATHENA_PREFERENCES_DESKTOP_VOLUMES_VISIBLE	   "volumes-visible"
#define ATHENA_PREFERENCES_DESKTOP_NETWORK_VISIBLE       "network-icon-visible"
#define ATHENA_PREFERENCES_DESKTOP_NETWORK_NAME          "network-icon-name"
#define ATHENA_PREFERENCES_DESKTOP_BACKGROUND_FADE       "background-fade"

/* bulk rename utility */
#define ATHENA_PREFERENCES_BULK_RENAME_TOOL              "bulk-rename-tool"

/* Lockdown */
#define ATHENA_PREFERENCES_LOCKDOWN_COMMAND_LINE         "disable-command-line"

/* Desktop background */
#define ATHENA_PREFERENCES_SHOW_DESKTOP		   "show-desktop-icons"


void athena_global_preferences_init                      (void);
char *athena_global_preferences_get_default_folder_viewer_preference_as_iid (void);

GSettings *athena_preferences;
GSettings *athena_icon_view_preferences;
GSettings *athena_list_view_preferences;
GSettings *athena_compact_view_preferences;
GSettings *athena_desktop_preferences;
GSettings *athena_tree_sidebar_preferences;
GSettings *athena_window_state;
GSettings *gnome_lockdown_preferences;
GSettings *gnome_background_preferences;

G_END_DECLS

#endif /* ATHENA_GLOBAL_PREFERENCES_H */
