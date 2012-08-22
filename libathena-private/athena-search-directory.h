/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-search-directory.h: Subclass of AthenaDirectory to implement
   a virtual directory consisting of the search directory and the search
   icons
 
   Copyright (C) 2005 Novell, Inc
  
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
*/

#ifndef ATHENA_SEARCH_DIRECTORY_H
#define ATHENA_SEARCH_DIRECTORY_H

#include <libathena-private/athena-directory.h>
#include <libathena-private/athena-query.h>

#define ATHENA_TYPE_SEARCH_DIRECTORY athena_search_directory_get_type()
#define ATHENA_SEARCH_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_SEARCH_DIRECTORY, AthenaSearchDirectory))
#define ATHENA_SEARCH_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_SEARCH_DIRECTORY, AthenaSearchDirectoryClass))
#define ATHENA_IS_SEARCH_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_SEARCH_DIRECTORY))
#define ATHENA_IS_SEARCH_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_SEARCH_DIRECTORY))
#define ATHENA_SEARCH_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_SEARCH_DIRECTORY, AthenaSearchDirectoryClass))

typedef struct AthenaSearchDirectoryDetails AthenaSearchDirectoryDetails;

typedef struct {
	AthenaDirectory parent_slot;
	AthenaSearchDirectoryDetails *details;
} AthenaSearchDirectory;

typedef struct {
	AthenaDirectoryClass parent_slot;
} AthenaSearchDirectoryClass;

GType   athena_search_directory_get_type             (void);

char   *athena_search_directory_generate_new_uri     (void);

AthenaSearchDirectory *athena_search_directory_new_from_saved_search (const char *uri);

gboolean       athena_search_directory_is_saved_search (AthenaSearchDirectory *search);
gboolean       athena_search_directory_is_modified     (AthenaSearchDirectory *search);
void           athena_search_directory_save_search     (AthenaSearchDirectory *search);
void           athena_search_directory_save_to_file    (AthenaSearchDirectory *search,
							  const char              *save_file_uri);

AthenaQuery *athena_search_directory_get_query       (AthenaSearchDirectory *search);
void           athena_search_directory_set_query       (AthenaSearchDirectory *search,
							  AthenaQuery           *query);

#endif /* ATHENA_SEARCH_DIRECTORY_H */
