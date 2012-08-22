/*
 *  athena-menu.h - Menus exported by AthenaMenuProvider objects.
 *
 *  Copyright (C) 2005 Raffaele Sandrini
 *  Copyright (C) 2003 Novell, Inc.
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
 *  Author:  Dave Camp <dave@ximian.com>
 *           Raffaele Sandrini <rasa@gmx.ch>
 *
 */

#ifndef ATHENA_MENU_H
#define ATHENA_MENU_H

#include <glib-object.h>
#include "athena-extension-types.h"


G_BEGIN_DECLS

/* AthenaMenu defines */
#define ATHENA_TYPE_MENU         (athena_menu_get_type ())
#define ATHENA_MENU(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_MENU, AthenaMenu))
#define ATHENA_MENU_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ATHENA_TYPE_MENU, AthenaMenuClass))
#define ATHENA_IS_MENU(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_MENU))
#define ATHENA_IS_MENU_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ATHENA_TYPE_MENU))
#define ATHENA_MENU_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ATHENA_TYPE_MENU, AthenaMenuClass))
/* AthenaMenuItem defines */
#define ATHENA_TYPE_MENU_ITEM            (athena_menu_item_get_type())
#define ATHENA_MENU_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_MENU_ITEM, AthenaMenuItem))
#define ATHENA_MENU_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_MENU_ITEM, AthenaMenuItemClass))
#define ATHENA_MENU_IS_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_MENU_ITEM))
#define ATHENA_MENU_IS_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), ATHENA_TYPE_MENU_ITEM))
#define ATHENA_MENU_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), ATHENA_TYPE_MENU_ITEM, AthenaMenuItemClass))


/* AthenaMenu types */
typedef struct _AthenaMenu		AthenaMenu;
typedef struct _AthenaMenuPrivate	AthenaMenuPrivate;
typedef struct _AthenaMenuClass	AthenaMenuClass;
/* AthenaMenuItem types */
typedef struct _AthenaMenuItem        AthenaMenuItem;
typedef struct _AthenaMenuItemDetails AthenaMenuItemDetails;
typedef struct _AthenaMenuItemClass   AthenaMenuItemClass;


/* AthenaMenu structs */
struct _AthenaMenu {
	GObject parent;
	AthenaMenuPrivate *priv;
};

struct _AthenaMenuClass {
	GObjectClass parent_class;
};

/* AthenaMenuItem structs */
struct _AthenaMenuItem {
	GObject parent;

	AthenaMenuItemDetails *details;
};

struct _AthenaMenuItemClass {
	GObjectClass parent;

	void (*activate) (AthenaMenuItem *item);
};


/* AthenaMenu methods */
GType		athena_menu_get_type	(void);
AthenaMenu *	athena_menu_new	(void);

void	athena_menu_append_item	(AthenaMenu      *menu,
					 AthenaMenuItem  *item);
GList*	athena_menu_get_items		(AthenaMenu *menu);
void	athena_menu_item_list_free	(GList *item_list);

/* AthenaMenuItem methods */
GType             athena_menu_item_get_type      (void);
AthenaMenuItem *athena_menu_item_new           (const char       *name,
						    const char       *label,
						    const char       *tip,
						    const char       *icon);

void              athena_menu_item_activate      (AthenaMenuItem *item);
void              athena_menu_item_set_submenu   (AthenaMenuItem *item,
						    AthenaMenu     *menu);
/* AthenaMenuItem has the following properties:
 *   name (string)        - the identifier for the menu item
 *   label (string)       - the user-visible label of the menu item
 *   tip (string)         - the tooltip of the menu item 
 *   icon (string)        - the name of the icon to display in the menu item
 *   sensitive (boolean)  - whether the menu item is sensitive or not
 *   priority (boolean)   - used for toolbar items, whether to show priority
 *                          text.
 *   menu (AthenaMenu)  - The menu belonging to this item. May be null.
 */

G_END_DECLS

#endif /* ATHENA_MENU_H */
