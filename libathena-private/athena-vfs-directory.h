/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-vfs-directory.h: Subclass of AthenaDirectory to implement the
   the case of a VFS directory.
 
   Copyright (C) 1999, 2000 Eazel, Inc.
  
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
  
   Author: Darin Adler <darin@bentspoon.com>
*/

#ifndef ATHENA_VFS_DIRECTORY_H
#define ATHENA_VFS_DIRECTORY_H

#include <libathena-private/athena-directory.h>

#define ATHENA_TYPE_VFS_DIRECTORY athena_vfs_directory_get_type()
#define ATHENA_VFS_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_VFS_DIRECTORY, AthenaVFSDirectory))
#define ATHENA_VFS_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_VFS_DIRECTORY, AthenaVFSDirectoryClass))
#define ATHENA_IS_VFS_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_VFS_DIRECTORY))
#define ATHENA_IS_VFS_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_VFS_DIRECTORY))
#define ATHENA_VFS_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_VFS_DIRECTORY, AthenaVFSDirectoryClass))

typedef struct AthenaVFSDirectoryDetails AthenaVFSDirectoryDetails;

typedef struct {
	AthenaDirectory parent_slot;
} AthenaVFSDirectory;

typedef struct {
	AthenaDirectoryClass parent_slot;
} AthenaVFSDirectoryClass;

GType   athena_vfs_directory_get_type (void);

#endif /* ATHENA_VFS_DIRECTORY_H */
