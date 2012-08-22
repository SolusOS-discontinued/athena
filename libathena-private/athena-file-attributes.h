/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   athena-file-attributes.h: #defines and other file-attribute-related info
 
   Copyright (C) 2000 Eazel, Inc.
  
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

#ifndef ATHENA_FILE_ATTRIBUTES_H
#define ATHENA_FILE_ATTRIBUTES_H

/* Names for AthenaFile attributes. These are used when registering
 * interest in changes to the attributes or when waiting for them.
 */

typedef enum {
	ATHENA_FILE_ATTRIBUTE_INFO = 1 << 0, /* All standard info */
	ATHENA_FILE_ATTRIBUTE_LINK_INFO = 1 << 1, /* info from desktop links */
	ATHENA_FILE_ATTRIBUTE_DEEP_COUNTS = 1 << 2,
	ATHENA_FILE_ATTRIBUTE_DIRECTORY_ITEM_COUNT = 1 << 3,
	ATHENA_FILE_ATTRIBUTE_DIRECTORY_ITEM_MIME_TYPES = 1 << 4,
	ATHENA_FILE_ATTRIBUTE_TOP_LEFT_TEXT = 1 << 5,
	ATHENA_FILE_ATTRIBUTE_LARGE_TOP_LEFT_TEXT = 1 << 6,
	ATHENA_FILE_ATTRIBUTE_EXTENSION_INFO = 1 << 7,
	ATHENA_FILE_ATTRIBUTE_THUMBNAIL = 1 << 8,
	ATHENA_FILE_ATTRIBUTE_MOUNT = 1 << 9,
	ATHENA_FILE_ATTRIBUTE_FILESYSTEM_INFO = 1 << 10,
} AthenaFileAttributes;

#endif /* ATHENA_FILE_ATTRIBUTES_H */
