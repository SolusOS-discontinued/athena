/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-desktop-file.h: Subclass of AthenaFile to implement the
   the case of a desktop icon file
 
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

#ifndef ATHENA_DESKTOP_ICON_FILE_H
#define ATHENA_DESKTOP_ICON_FILE_H

#include <libathena-private/athena-file.h>
#include <libathena-private/athena-desktop-link.h>

#define ATHENA_TYPE_DESKTOP_ICON_FILE athena_desktop_icon_file_get_type()
#define ATHENA_DESKTOP_ICON_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_DESKTOP_ICON_FILE, AthenaDesktopIconFile))
#define ATHENA_DESKTOP_ICON_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_DESKTOP_ICON_FILE, AthenaDesktopIconFileClass))
#define ATHENA_IS_DESKTOP_ICON_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_DESKTOP_ICON_FILE))
#define ATHENA_IS_DESKTOP_ICON_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_DESKTOP_ICON_FILE))
#define ATHENA_DESKTOP_ICON_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_DESKTOP_ICON_FILE, AthenaDesktopIconFileClass))

typedef struct AthenaDesktopIconFileDetails AthenaDesktopIconFileDetails;

typedef struct {
	AthenaFile parent_slot;
	AthenaDesktopIconFileDetails *details;
} AthenaDesktopIconFile;

typedef struct {
	AthenaFileClass parent_slot;
} AthenaDesktopIconFileClass;

GType   athena_desktop_icon_file_get_type (void);

AthenaDesktopIconFile *athena_desktop_icon_file_new      (AthenaDesktopLink     *link);
void                     athena_desktop_icon_file_update   (AthenaDesktopIconFile *icon_file);
void                     athena_desktop_icon_file_remove   (AthenaDesktopIconFile *icon_file);
AthenaDesktopLink     *athena_desktop_icon_file_get_link (AthenaDesktopIconFile *icon_file);

#endif /* ATHENA_DESKTOP_ICON_FILE_H */
