/*
 *  athena-menu.h - Menus exported by AthenaMenuProvider objects.
 *
 *  Copyright (C) 2005 Raffaele Sandrini
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 *  Author:  Raffaele Sandrini <rasa@gmx.ch>
 *
 */

#include <config.h>
#include "athena-menu.h"
#include "athena-extension-i18n.h"

#include <glib.h>

#define ATHENA_MENU_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), ATHENA_TYPE_MENU, AthenaMenuPrivate))
G_DEFINE_TYPE (AthenaMenu, athena_menu, G_TYPE_OBJECT);

struct _AthenaMenuPrivate {
	GList *item_list;
};

void
athena_menu_append_item (AthenaMenu *menu, AthenaMenuItem *item)
{
	g_return_if_fail (menu != NULL);
	g_return_if_fail (item != NULL);
	
	menu->priv->item_list = g_list_append (menu->priv->item_list, g_object_ref (item));
}

/**
 * athena_menu_get_items:
 * @menu: a #AthenaMenu
 *
 * Returns: (element-type AthenaMenuItem) (transfer full): the provided #AthenaMenuItem list
 */
GList *
athena_menu_get_items (AthenaMenu *menu)
{
	GList *item_list;

	g_return_val_if_fail (menu != NULL, NULL);
	
	item_list = g_list_copy (menu->priv->item_list);
	g_list_foreach (item_list, (GFunc)g_object_ref, NULL);
	
	return item_list;
}

/**
 * athena_menu_item_list_free:
 * @item_list: (element-type AthenaMenuItem): a list of #AthenaMenuItem
 *
 */
void
athena_menu_item_list_free (GList *item_list)
{
	g_return_if_fail (item_list != NULL);
	
	g_list_foreach (item_list, (GFunc)g_object_unref, NULL);
	g_list_free (item_list);
}

/* Type initialization */

static void
athena_menu_finalize (GObject *object)
{
	AthenaMenu *menu = ATHENA_MENU (object);

	if (menu->priv->item_list) {
		g_list_free (menu->priv->item_list);
	}

	G_OBJECT_CLASS (athena_menu_parent_class)->finalize (object);
}

static void
athena_menu_init (AthenaMenu *menu)
{
	menu->priv = ATHENA_MENU_GET_PRIVATE (menu);

	menu->priv->item_list = NULL;
}

static void
athena_menu_class_init (AthenaMenuClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (AthenaMenuPrivate));
	
	object_class->finalize = athena_menu_finalize;
}

/* public constructors */

AthenaMenu *
athena_menu_new (void)
{
	AthenaMenu *obj;
	
	obj = ATHENA_MENU (g_object_new (ATHENA_TYPE_MENU, NULL));
	
	return obj;
}
