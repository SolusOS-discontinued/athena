/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-desktop-link.h: Class that handles the links on the desktop
    
   Copyright (C) 2003 Red Hat, Inc.
  
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
  
   Author: Alexander Larsson <alexl@redhat.com>
*/

#ifndef ATHENA_DESKTOP_LINK_H
#define ATHENA_DESKTOP_LINK_H

#include <libathena-private/athena-file.h>
#include <gio/gio.h>

#define ATHENA_TYPE_DESKTOP_LINK athena_desktop_link_get_type()
#define ATHENA_DESKTOP_LINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_DESKTOP_LINK, AthenaDesktopLink))
#define ATHENA_DESKTOP_LINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_DESKTOP_LINK, AthenaDesktopLinkClass))
#define ATHENA_IS_DESKTOP_LINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_DESKTOP_LINK))
#define ATHENA_IS_DESKTOP_LINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_DESKTOP_LINK))
#define ATHENA_DESKTOP_LINK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_DESKTOP_LINK, AthenaDesktopLinkClass))

typedef struct AthenaDesktopLinkDetails AthenaDesktopLinkDetails;

typedef struct {
	GObject parent_slot;
	AthenaDesktopLinkDetails *details;
} AthenaDesktopLink;

typedef struct {
	GObjectClass parent_slot;
} AthenaDesktopLinkClass;

typedef enum {
	ATHENA_DESKTOP_LINK_HOME,
	ATHENA_DESKTOP_LINK_COMPUTER,
	ATHENA_DESKTOP_LINK_TRASH,
	ATHENA_DESKTOP_LINK_MOUNT,
	ATHENA_DESKTOP_LINK_NETWORK
} AthenaDesktopLinkType;

GType   athena_desktop_link_get_type (void);

AthenaDesktopLink *   athena_desktop_link_new                     (AthenaDesktopLinkType  type);
AthenaDesktopLink *   athena_desktop_link_new_from_mount          (GMount                 *mount);
AthenaDesktopLinkType athena_desktop_link_get_link_type           (AthenaDesktopLink     *link);
char *                  athena_desktop_link_get_file_name           (AthenaDesktopLink     *link);
char *                  athena_desktop_link_get_display_name        (AthenaDesktopLink     *link);
GIcon *                 athena_desktop_link_get_icon                (AthenaDesktopLink     *link);
GFile *                 athena_desktop_link_get_activation_location (AthenaDesktopLink     *link);
char *                  athena_desktop_link_get_activation_uri      (AthenaDesktopLink     *link);
gboolean                athena_desktop_link_get_date                (AthenaDesktopLink     *link,
								       AthenaDateType         date_type,
								       time_t                  *date);
GMount *                athena_desktop_link_get_mount               (AthenaDesktopLink     *link);
gboolean                athena_desktop_link_can_rename              (AthenaDesktopLink     *link);
gboolean                athena_desktop_link_rename                  (AthenaDesktopLink     *link,
								       const char              *name);


#endif /* ATHENA_DESKTOP_LINK_H */
