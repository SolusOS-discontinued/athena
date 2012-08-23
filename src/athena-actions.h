/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  Athena
 *
 *  Copyright (C) 2004 Red Hat, Inc.
 *
 *  Athena is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Athena is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *
 */

#ifndef ATHENA_ACTIONS_H
#define ATHENA_ACTIONS_H

#define ATHENA_ACTION_STOP "Stop"
#define ATHENA_ACTION_RELOAD "Reload"
#define ATHENA_ACTION_BACK "Back"
#define ATHENA_ACTION_COMPUTER "Computer"
#define ATHENA_ACTION_UP "Up"
#define ATHENA_ACTION_UP_ACCEL "UpAccel"
#define ATHENA_ACTION_UP_ACCEL "UpAccel"
#define ATHENA_ACTION_FORWARD "Forward"
#define ATHENA_ACTION_SHOW_HIDE_TOOLBAR "Show Hide Toolbar"
#define ATHENA_ACTION_SHOW_HIDE_SIDEBAR "Show Hide Sidebar"
#define ATHENA_ACTION_SHOW_HIDE_STATUSBAR "Show Hide Statusbar"
#define ATHENA_ACTION_SHOW_HIDE_LOCATION_BAR "Show Hide Location Bar"
#define ATHENA_ACTION_SHOW_HIDE_EXTRA_PANE "Show Hide Extra Pane"
#define ATHENA_ACTION_GO_TO_BURN_CD "Go to Burn CD"
#define ATHENA_ACTION_GO_TO_LOCATION "Go to Location"
#define ATHENA_ACTION_GO_HOME "Home"
#define ATHENA_ACTION_ADD_BOOKMARK "Add Bookmark"
#define ATHENA_ACTION_EDIT_BOOKMARKS "Edit Bookmarks"
#define ATHENA_ACTION_EDIT "Edit"
#define ATHENA_ACTION_HOME "Home"
#define ATHENA_ACTION_ZOOM_IN "Zoom In"
#define ATHENA_ACTION_ZOOM_OUT "Zoom Out"
#define ATHENA_ACTION_ZOOM_NORMAL "Zoom Normal"
#define ATHENA_ACTION_SHOW_HIDDEN_FILES "Show Hidden Files"
#define ATHENA_ACTION_CLOSE "Close"
#define ATHENA_ACTION_SEARCH "Search"
#define ATHENA_ACTION_FOLDER_WINDOW "Folder Window"
#define ATHENA_ACTION_NEW_TAB "New Tab"

/* Added specifically for the view-toggler on the toolbar */
#define ATHENA_ACTION_VIEW_ICONS "View Icons"
#define ATHENA_ACTION_VIEW_LIST "View List"
#define ATHENA_ACTION_VIEW_DETAILS "View Details"

#define ATHENA_ACTION_OPEN "Open"
#define ATHENA_ACTION_OPEN_ALTERNATE "OpenAlternate"
#define ATHENA_ACTION_OPEN_IN_NEW_TAB "OpenInNewTab"
#define ATHENA_ACTION_LOCATION_OPEN_ALTERNATE "LocationOpenAlternate"
#define ATHENA_ACTION_LOCATION_OPEN_IN_NEW_TAB "LocationOpenInNewTab"
#define ATHENA_ACTION_OTHER_APPLICATION1 "OtherApplication1"
#define ATHENA_ACTION_OTHER_APPLICATION2 "OtherApplication2"
#define ATHENA_ACTION_NEW_FOLDER "New Folder"
#define ATHENA_ACTION_PROPERTIES "Properties"
#define ATHENA_ACTION_PROPERTIES_ACCEL "PropertiesAccel"
#define ATHENA_ACTION_LOCATION_PROPERTIES "LocationProperties"
#define ATHENA_ACTION_NO_TEMPLATES "No Templates"
#define ATHENA_ACTION_EMPTY_TRASH "Empty Trash"
#define ATHENA_ACTION_SAVE_SEARCH "Save Search"
#define ATHENA_ACTION_SAVE_SEARCH_AS "Save Search As"
#define ATHENA_ACTION_CUT "Cut"
#define ATHENA_ACTION_LOCATION_CUT "LocationCut"
#define ATHENA_ACTION_COPY "Copy"
#define ATHENA_ACTION_LOCATION_COPY "LocationCopy"
#define ATHENA_ACTION_PASTE "Paste"
#define ATHENA_ACTION_PASTE_FILES_INTO "Paste Files Into"
#define ATHENA_ACTION_COPY_TO_NEXT_PANE "Copy to next pane"
#define ATHENA_ACTION_MOVE_TO_NEXT_PANE "Move to next pane"
#define ATHENA_ACTION_COPY_TO_HOME "Copy to Home"
#define ATHENA_ACTION_MOVE_TO_HOME "Move to Home"
#define ATHENA_ACTION_COPY_TO_DESKTOP "Copy to Desktop"
#define ATHENA_ACTION_MOVE_TO_DESKTOP "Move to Desktop"
#define ATHENA_ACTION_LOCATION_PASTE_FILES_INTO "LocationPasteFilesInto"
#define ATHENA_ACTION_RENAME "Rename"
#define ATHENA_ACTION_DUPLICATE "Duplicate"
#define ATHENA_ACTION_CREATE_LINK "Create Link"
#define ATHENA_ACTION_SELECT_ALL "Select All"
#define ATHENA_ACTION_INVERT_SELECTION "Invert Selection"
#define ATHENA_ACTION_SELECT_PATTERN "Select Pattern"
#define ATHENA_ACTION_TRASH "Trash"
#define ATHENA_ACTION_LOCATION_TRASH "LocationTrash"
#define ATHENA_ACTION_DELETE "Delete"
#define ATHENA_ACTION_LOCATION_DELETE "LocationDelete"
#define ATHENA_ACTION_RESTORE_FROM_TRASH "Restore From Trash"
#define ATHENA_ACTION_LOCATION_RESTORE_FROM_TRASH "LocationRestoreFromTrash"
#define ATHENA_ACTION_SHOW_HIDDEN_FILES "Show Hidden Files"
#define ATHENA_ACTION_CONNECT_TO_SERVER_LINK "Connect To Server Link"
#define ATHENA_ACTION_MOUNT_VOLUME "Mount Volume"
#define ATHENA_ACTION_UNMOUNT_VOLUME "Unmount Volume"
#define ATHENA_ACTION_EJECT_VOLUME "Eject Volume"
#define ATHENA_ACTION_START_VOLUME "Start Volume"
#define ATHENA_ACTION_STOP_VOLUME "Stop Volume"
#define ATHENA_ACTION_POLL "Poll"
#define ATHENA_ACTION_SELF_MOUNT_VOLUME "Self Mount Volume"
#define ATHENA_ACTION_SELF_UNMOUNT_VOLUME "Self Unmount Volume"
#define ATHENA_ACTION_SELF_EJECT_VOLUME "Self Eject Volume"
#define ATHENA_ACTION_SELF_START_VOLUME "Self Start Volume"
#define ATHENA_ACTION_SELF_STOP_VOLUME "Self Stop Volume"
#define ATHENA_ACTION_SELF_POLL "Self Poll"
#define ATHENA_ACTION_LOCATION_MOUNT_VOLUME "Location Mount Volume"
#define ATHENA_ACTION_LOCATION_UNMOUNT_VOLUME "Location Unmount Volume"
#define ATHENA_ACTION_LOCATION_EJECT_VOLUME "Location Eject Volume"
#define ATHENA_ACTION_LOCATION_START_VOLUME "Location Start Volume"
#define ATHENA_ACTION_LOCATION_STOP_VOLUME "Location Stop Volume"
#define ATHENA_ACTION_LOCATION_POLL "Location Poll"
#define ATHENA_ACTION_SCRIPTS "Scripts"
#define ATHENA_ACTION_NEW_LAUNCHER "New Launcher"
#define ATHENA_ACTION_NEW_LAUNCHER_DESKTOP "New Launcher"
#define ATHENA_ACTION_NEW_DOCUMENTS "New Documents"
#define ATHENA_ACTION_NEW_EMPTY_DOCUMENT "New Empty Document"
#define ATHENA_ACTION_EMPTY_TRASH_CONDITIONAL "Empty Trash Conditional"
#define ATHENA_ACTION_MANUAL_LAYOUT "Manual Layout"
#define ATHENA_ACTION_REVERSED_ORDER "Reversed Order"
#define ATHENA_ACTION_CLEAN_UP "Clean Up"
#define ATHENA_ACTION_KEEP_ALIGNED "Keep Aligned"
#define ATHENA_ACTION_ARRANGE_ITEMS "Arrange Items"
#define ATHENA_ACTION_STRETCH "Stretch"
#define ATHENA_ACTION_UNSTRETCH "Unstretch"
#define ATHENA_ACTION_ZOOM_ITEMS "Zoom Items"
#define ATHENA_ACTION_SORT_TRASH_TIME "Sort by Trash Time"

#endif /* ATHENA_ACTIONS_H */
