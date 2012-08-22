/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-vfs-file.h: Subclass of AthenaFile to implement the
   the case of a VFS file.
 
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

#ifndef ATHENA_VFS_FILE_H
#define ATHENA_VFS_FILE_H

#include <libathena-private/athena-file.h>

#define ATHENA_TYPE_VFS_FILE athena_vfs_file_get_type()
#define ATHENA_VFS_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_VFS_FILE, AthenaVFSFile))
#define ATHENA_VFS_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_VFS_FILE, AthenaVFSFileClass))
#define ATHENA_IS_VFS_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_VFS_FILE))
#define ATHENA_IS_VFS_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_VFS_FILE))
#define ATHENA_VFS_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_VFS_FILE, AthenaVFSFileClass))

typedef struct AthenaVFSFileDetails AthenaVFSFileDetails;

typedef struct {
	AthenaFile parent_slot;
} AthenaVFSFile;

typedef struct {
	AthenaFileClass parent_slot;
} AthenaVFSFileClass;

GType   athena_vfs_file_get_type (void);

#endif /* ATHENA_VFS_FILE_H */
