/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-search-directory-file.h: Subclass of AthenaFile to implement the
   the case of the search directory
 
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

#ifndef ATHENA_SEARCH_DIRECTORY_FILE_H
#define ATHENA_SEARCH_DIRECTORY_FILE_H

#include <libathena-private/athena-file.h>

#define ATHENA_TYPE_SEARCH_DIRECTORY_FILE athena_search_directory_file_get_type()
#define ATHENA_SEARCH_DIRECTORY_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_SEARCH_DIRECTORY_FILE, AthenaSearchDirectoryFile))
#define ATHENA_SEARCH_DIRECTORY_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_SEARCH_DIRECTORY_FILE, AthenaSearchDirectoryFileClass))
#define ATHENA_IS_SEARCH_DIRECTORY_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_SEARCH_DIRECTORY_FILE))
#define ATHENA_IS_SEARCH_DIRECTORY_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_SEARCH_DIRECTORY_FILE))
#define ATHENA_SEARCH_DIRECTORY_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_SEARCH_DIRECTORY_FILE, AthenaSearchDirectoryFileClass))

typedef struct AthenaSearchDirectoryFileDetails AthenaSearchDirectoryFileDetails;

typedef struct {
	AthenaFile parent_slot;
	AthenaSearchDirectoryFileDetails *details;
} AthenaSearchDirectoryFile;

typedef struct {
	AthenaFileClass parent_slot;
} AthenaSearchDirectoryFileClass;

GType   athena_search_directory_file_get_type (void);
void    athena_search_directory_file_update_display_name (AthenaSearchDirectoryFile *search_file);

#endif /* ATHENA_SEARCH_DIRECTORY_FILE_H */
