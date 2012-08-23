/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Athena
 *
 * Copyright (C) 2011, Red Hat, Inc.
 *
 * Athena is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Athena is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#include <config.h>

#include "athena-toolbar.h"

#include "athena-location-bar.h"
#include "athena-pathbar.h"
#include "athena-window-private.h"
#include "athena-actions.h"

#include <libathena-private/athena-global-preferences.h>
#include <libathena-private/athena-ui-utilities.h>

struct _AthenaToolbarPriv {
	GtkWidget *toolbar;

	GtkActionGroup *action_group;
	GtkUIManager *ui_manager;

	GtkWidget *path_bar;
	GtkWidget *location_bar;
	GtkWidget *search_bar;

	/* Buttons for the toolbar */
	GtkToolItem *back_forward;
	GtkToolItem *search;
	GtkToolItem *view_toggler;

	gboolean show_main_bar;
	gboolean show_location_entry;
	gboolean show_search_bar;
};

enum {
	PROP_ACTION_GROUP = 1,
	PROP_SHOW_LOCATION_ENTRY,
	PROP_SHOW_SEARCH_BAR,
	PROP_SHOW_MAIN_BAR,
	NUM_PROPERTIES
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

G_DEFINE_TYPE (AthenaToolbar, athena_toolbar, GTK_TYPE_BOX);

static void
toolbar_update_appearance (AthenaToolbar *self)
{
	GtkAction *action;
	GtkWidget *widgetitem;
	gboolean icon_toolbar;

	gboolean show_location_entry;

	show_location_entry = self->priv->show_location_entry ||
		g_settings_get_boolean (athena_preferences, ATHENA_PREFERENCES_ALWAYS_USE_LOCATION_ENTRY);

	gtk_widget_set_visible (self->priv->toolbar,
				self->priv->show_main_bar);

	gtk_widget_set_visible (self->priv->location_bar,
				show_location_entry);
	gtk_widget_set_visible (self->priv->path_bar,
				!show_location_entry);

	gtk_widget_set_visible (self->priv->search_bar,
				self->priv->show_search_bar);


	widgetitem = gtk_ui_manager_get_widget (self->priv->ui_manager, "/Toolbar/Up");
	icon_toolbar = g_settings_get_boolean (athena_preferences, ATHENA_PREFERENCES_SHOW_UP_ICON_TOOLBAR);
	if ( icon_toolbar == FALSE ) { gtk_widget_hide (widgetitem); }
	else {gtk_widget_show (widgetitem);}

	widgetitem = gtk_ui_manager_get_widget (self->priv->ui_manager, "/Toolbar/Reload");
	icon_toolbar = g_settings_get_boolean (athena_preferences, ATHENA_PREFERENCES_SHOW_RELOAD_ICON_TOOLBAR);
	if ( icon_toolbar == FALSE ) { gtk_widget_hide (widgetitem); }
	else {gtk_widget_show (widgetitem);}

	widgetitem = gtk_ui_manager_get_widget (self->priv->ui_manager, "/Toolbar/Edit");
	icon_toolbar = g_settings_get_boolean (athena_preferences, ATHENA_PREFERENCES_SHOW_EDIT_ICON_TOOLBAR);
	if ( icon_toolbar == FALSE ) { gtk_widget_hide (widgetitem); }
	else {gtk_widget_show (widgetitem);}

	widgetitem = gtk_ui_manager_get_widget (self->priv->ui_manager, "/Toolbar/Home");
	icon_toolbar = g_settings_get_boolean (athena_preferences, ATHENA_PREFERENCES_SHOW_HOME_ICON_TOOLBAR);
	if ( icon_toolbar == FALSE ) { gtk_widget_hide (widgetitem); }
	else {gtk_widget_show (widgetitem);}

	widgetitem = gtk_ui_manager_get_widget (self->priv->ui_manager, "/Toolbar/Computer");
	icon_toolbar = g_settings_get_boolean (athena_preferences, ATHENA_PREFERENCES_SHOW_COMPUTER_ICON_TOOLBAR);
	if ( icon_toolbar == FALSE ) { gtk_widget_hide (widgetitem); }
	else {gtk_widget_show (widgetitem);}

	widgetitem = self->priv->search;
	icon_toolbar = g_settings_get_boolean (athena_preferences, ATHENA_PREFERENCES_SHOW_SEARCH_ICON_TOOLBAR);
	if ( icon_toolbar == FALSE ) { gtk_widget_hide (widgetitem); }
	else {gtk_widget_show (widgetitem);}
}

static GtkWidget *
toolbar_create_toolbutton (AthenaToolbar *self,
			   gboolean create_toggle,
			   const gchar *name,
			   const gchar *icon_name)
{
	GtkWidget *button, *image;
	GtkAction *action;

	if (create_toggle) {
		button = gtk_toggle_button_new ();
	} else {
		button = gtk_button_new ();
	}

	// Don't use focus for toolbar buttons, don't want them trapping the keyboard etc.
	gtk_button_set_focus_on_click (GTK_BUTTON (button), FALSE);

	image = gtk_image_new ();
	gtk_image_set_from_icon_name (GTK_IMAGE (image), icon_name,
					GTK_ICON_SIZE_SMALL_TOOLBAR);

	gtk_container_add (GTK_CONTAINER (button), image); // Force image to show
	g_object_set (image, "margin", 3, NULL); // 3px margin around the image
	gtk_widget_show (GTK_WIDGET (image));

	action = gtk_action_group_get_action (self->priv->action_group, name);
	gtk_activatable_set_related_action (GTK_ACTIVATABLE (button), action);
	gtk_button_set_label (GTK_BUTTON (button), NULL);

	return button;
}

static void
athena_toolbar_constructed (GObject *obj)
{
	AthenaToolbar *self = ATHENA_TOOLBAR (obj);
	GtkToolItem *item;
	GtkWidget *hbox, *toolbar, *search;
	GtkStyleContext *context;

	GtkWidget *sep_space;

	GtkWidget *box;
	GtkToolItem *back_forward, *view_toggler;
	GtkWidget *tool_button;
	GtkWidget *search_item;

	G_OBJECT_CLASS (athena_toolbar_parent_class)->constructed (obj);

	gtk_style_context_set_junction_sides (gtk_widget_get_style_context (GTK_WIDGET (self)),
					      GTK_JUNCTION_BOTTOM);

	/* add the UI */
	self->priv->ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_add_ui_from_resource (self->priv->ui_manager, "/org/gnome/athena/athena-toolbar-ui.xml", NULL);
	gtk_ui_manager_insert_action_group (self->priv->ui_manager, self->priv->action_group, 0);

	toolbar = gtk_ui_manager_get_widget (self->priv->ui_manager, "/Toolbar");
	self->priv->toolbar = toolbar;
	/** Remove this, should be up to the user what the icon size is.
	gtk_toolbar_set_icon_size (GTK_TOOLBAR (toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR); */

	context = gtk_widget_get_style_context (toolbar);
	gtk_style_context_add_class (context, GTK_STYLE_CLASS_PRIMARY_TOOLBAR);

	/* Back and Forward - Seen in Nautilus 3.5.9 */
	back_forward = gtk_tool_item_new ();
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	/* Back */
	tool_button = toolbar_create_toolbutton (self,  FALSE, ATHENA_ACTION_BACK, "go-previous");
	gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (tool_button));

	/* Forward */
	tool_button = toolbar_create_toolbutton (self, FALSE, ATHENA_ACTION_FORWARD, "go-next");
	gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (tool_button));

	gtk_style_context_add_class (gtk_widget_get_style_context (box),
				     GTK_STYLE_CLASS_RAISED);
	gtk_style_context_add_class (gtk_widget_get_style_context (box),
				     GTK_STYLE_CLASS_LINKED);

	gtk_container_add (GTK_CONTAINER (back_forward), box);
	gtk_widget_show_all (GTK_WIDGET (back_forward));

	gtk_toolbar_insert (GTK_TOOLBAR (self->priv->toolbar), back_forward, 0);
	self->priv->back_forward = back_forward;
	/* End of back/forward button */

	/* Search button */
	tool_button = toolbar_create_toolbutton (self, TRUE, ATHENA_ACTION_SEARCH, "edit-find");
	search_item = gtk_tool_item_new ();
	gtk_container_add (GTK_CONTAINER (search_item), GTK_WIDGET (tool_button));
	gtk_widget_show_all (GTK_WIDGET (search_item));
	gtk_style_context_add_class (gtk_widget_get_style_context (search_item), GTK_STYLE_CLASS_RAISED);
	gtk_widget_set_name (search_item, "athena-search-button");
	self->priv->search = search_item;
	gtk_toolbar_insert (GTK_TOOLBAR (self->priv->toolbar), search_item, 1);
	gtk_widget_set_margin_left (GTK_WIDGET (search_item), 20);

	/* End of search button */

	sep_space = gtk_ui_manager_get_widget(self->priv->ui_manager, "/Toolbar/BeforeSearch");
	gtk_separator_tool_item_set_draw (GTK_SEPARATOR_TOOL_ITEM (sep_space), FALSE);
	gtk_tool_item_set_expand (GTK_SEPARATOR_TOOL_ITEM (sep_space), TRUE);

	gtk_box_pack_start (GTK_BOX (self), self->priv->toolbar, TRUE, TRUE, 0);
	gtk_widget_show_all (self->priv->toolbar);

	/* Create the view toggler */
	view_toggler = gtk_tool_item_new ();
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_style_context_add_class (gtk_widget_get_style_context (box),
				     GTK_STYLE_CLASS_RAISED);
	gtk_style_context_add_class (gtk_widget_get_style_context (box),
				     GTK_STYLE_CLASS_LINKED);

	// View buttons.
	tool_button = toolbar_create_toolbutton (self, TRUE, ATHENA_ACTION_VIEW_LIST, "view-list-compact");
	gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (tool_button));
	tool_button = toolbar_create_toolbutton (self, TRUE, ATHENA_ACTION_VIEW_ICONS, "view-list-icons");
	gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (tool_button));
	tool_button = toolbar_create_toolbutton (self, TRUE, ATHENA_ACTION_VIEW_DETAILS, "view-list-details");
	gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (tool_button));

	// Quick pointer to the view_toggler
	self->priv->view_toggler = view_toggler;

	// Whack it all onto the end of the toolbar now
	gtk_container_add (GTK_CONTAINER (view_toggler), box);
	gtk_widget_show_all (GTK_WIDGET (view_toggler));
	gtk_toolbar_insert (GTK_TOOLBAR (self->priv->toolbar), view_toggler, -1);

	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (hbox);

	/* regular path bar */
	self->priv->path_bar = g_object_new (ATHENA_TYPE_PATH_BAR, NULL);
	gtk_widget_set_margin_left (GTK_WIDGET (self->priv->path_bar), 20);
	gtk_box_pack_start (GTK_BOX (hbox), self->priv->path_bar, TRUE, TRUE, 0);

	/* entry-like location bar */
	self->priv->location_bar = athena_location_bar_new ();
	gtk_box_pack_start (GTK_BOX (hbox), self->priv->location_bar, TRUE, TRUE, 0);

	item = gtk_tool_item_new ();
	gtk_tool_item_set_expand (item, TRUE);
	gtk_container_add (GTK_CONTAINER (item), hbox);
	/* append to the end of the toolbar so navigation buttons are at the beginning */
	gtk_toolbar_insert (GTK_TOOLBAR (self->priv->toolbar), item, 8);
	gtk_widget_show (GTK_WIDGET (item));

	/* search bar */
	self->priv->search_bar = athena_search_bar_new ();
	gtk_box_pack_start (GTK_BOX (self), self->priv->search_bar, TRUE, TRUE, 0);

	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_ALWAYS_USE_LOCATION_ENTRY,
				  G_CALLBACK (toolbar_update_appearance), self);

	/* athena patch */
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_SHOW_UP_ICON_TOOLBAR,
				  G_CALLBACK (toolbar_update_appearance), self);
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_SHOW_EDIT_ICON_TOOLBAR,
				  G_CALLBACK (toolbar_update_appearance), self);
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_SHOW_RELOAD_ICON_TOOLBAR,
				  G_CALLBACK (toolbar_update_appearance), self);
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_SHOW_HOME_ICON_TOOLBAR,
				  G_CALLBACK (toolbar_update_appearance), self);
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_SHOW_COMPUTER_ICON_TOOLBAR,
				  G_CALLBACK (toolbar_update_appearance), self);
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_SHOW_SEARCH_ICON_TOOLBAR,
				  G_CALLBACK (toolbar_update_appearance), self);
	g_signal_connect_swapped (athena_preferences,
				  "changed::" ATHENA_PREFERENCES_SHOW_LABEL_SEARCH_ICON_TOOLBAR,
				  G_CALLBACK (toolbar_update_appearance), self);

	toolbar_update_appearance (self);

}

static void
athena_toolbar_init (AthenaToolbar *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ATHENA_TYPE_TOOLBAR,
						  AthenaToolbarPriv);
	self->priv->show_main_bar = TRUE;	
}

static void
athena_toolbar_get_property (GObject *object,
			       guint property_id,
			       GValue *value,
			       GParamSpec *pspec)
{
	AthenaToolbar *self = ATHENA_TOOLBAR (object);

	switch (property_id) {
	case PROP_SHOW_LOCATION_ENTRY:
		g_value_set_boolean (value, self->priv->show_location_entry);
		break;
	case PROP_SHOW_SEARCH_BAR:
		g_value_set_boolean (value, self->priv->show_search_bar);
		break;
	case PROP_SHOW_MAIN_BAR:
		g_value_set_boolean (value, self->priv->show_main_bar);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
athena_toolbar_set_property (GObject *object,
			       guint property_id,
			       const GValue *value,
			       GParamSpec *pspec)
{
	AthenaToolbar *self = ATHENA_TOOLBAR (object);

	switch (property_id) {
	case PROP_ACTION_GROUP:
		self->priv->action_group = g_value_dup_object (value);
		break;
	case PROP_SHOW_LOCATION_ENTRY:
		athena_toolbar_set_show_location_entry (self, g_value_get_boolean (value));
		break;
	case PROP_SHOW_SEARCH_BAR:
		athena_toolbar_set_show_search_bar (self, g_value_get_boolean (value));
		break;
	case PROP_SHOW_MAIN_BAR:
		athena_toolbar_set_show_main_bar (self, g_value_get_boolean (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

static void
athena_toolbar_dispose (GObject *obj)
{
	AthenaToolbar *self = ATHENA_TOOLBAR (obj);

	g_clear_object (&self->priv->ui_manager);
	g_clear_object (&self->priv->action_group);

	g_signal_handlers_disconnect_by_func (athena_preferences,
					      toolbar_update_appearance, self);

	G_OBJECT_CLASS (athena_toolbar_parent_class)->dispose (obj);
}

static void
athena_toolbar_class_init (AthenaToolbarClass *klass)
{
	GObjectClass *oclass;

	oclass = G_OBJECT_CLASS (klass);
	oclass->get_property = athena_toolbar_get_property;
	oclass->set_property = athena_toolbar_set_property;
	oclass->constructed = athena_toolbar_constructed;
	oclass->dispose = athena_toolbar_dispose;

	properties[PROP_ACTION_GROUP] =
		g_param_spec_object ("action-group",
				     "The action group",
				     "The action group to get actions from",
				     GTK_TYPE_ACTION_GROUP,
				     G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY |
				     G_PARAM_STATIC_STRINGS);
	properties[PROP_SHOW_LOCATION_ENTRY] =
		g_param_spec_boolean ("show-location-entry",
				      "Whether to show the location entry",
				      "Whether to show the location entry instead of the pathbar",
				      FALSE,
				      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
	properties[PROP_SHOW_SEARCH_BAR] =
		g_param_spec_boolean ("show-search-bar",
				      "Whether to show the search bar",
				      "Whether to show the search bar beside the toolbar",
				      FALSE,
				      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
	properties[PROP_SHOW_MAIN_BAR] =
		g_param_spec_boolean ("show-main-bar",
				      "Whether to show the main bar",
				      "Whether to show the main toolbar",
				      TRUE,
				      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
	
	g_type_class_add_private (klass, sizeof (AthenaToolbarClass));
	g_object_class_install_properties (oclass, NUM_PROPERTIES, properties);
}

GtkWidget *
athena_toolbar_new (GtkActionGroup *action_group)
{
	return g_object_new (ATHENA_TYPE_TOOLBAR,
			     "action-group", action_group,
			     "orientation", GTK_ORIENTATION_VERTICAL,
			     NULL);
}

GtkWidget *
athena_toolbar_get_path_bar (AthenaToolbar *self)
{
	return self->priv->path_bar;
}

GtkWidget *
athena_toolbar_get_location_bar (AthenaToolbar *self)
{
	return self->priv->location_bar;
}

GtkWidget *
athena_toolbar_get_search_bar (AthenaToolbar *self)
{
	return self->priv->search_bar;
}

GtkWidget *
athena_toolbar_get_show_location_entry (AthenaToolbar *self)
{
	return self->priv->show_location_entry;
}

void
athena_toolbar_set_show_main_bar (AthenaToolbar *self,
				    gboolean show_main_bar)
{
	if (show_main_bar != self->priv->show_main_bar) {
		self->priv->show_main_bar = show_main_bar;
		toolbar_update_appearance (self);

		g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SHOW_MAIN_BAR]);
	}
}

void
athena_toolbar_set_show_location_entry (AthenaToolbar *self,
					  gboolean show_location_entry)
{
	if (show_location_entry != self->priv->show_location_entry) {
		self->priv->show_location_entry = show_location_entry;
		toolbar_update_appearance (self);

		g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SHOW_LOCATION_ENTRY]);
	}
}

void
athena_toolbar_set_show_search_bar (AthenaToolbar *self,
				      gboolean show_search_bar)
{
	if (show_search_bar != self->priv->show_search_bar) {
		self->priv->show_search_bar = show_search_bar;
		toolbar_update_appearance (self);

		g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SHOW_SEARCH_BAR]);
	}
}
