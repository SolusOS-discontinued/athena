/*
 *  athena-file-info.h - Information about a file 
 *
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
 */

/* AthenaFileInfo is an interface to the AthenaFile object.  It 
 * provides access to the asynchronous data in the AthenaFile.
 * Extensions are passed objects of this type for operations. */

#ifndef ATHENA_FILE_INFO_H
#define ATHENA_FILE_INFO_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define ATHENA_TYPE_FILE_INFO           (athena_file_info_get_type ())
#define ATHENA_FILE_INFO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_FILE_INFO, AthenaFileInfo))
#define ATHENA_IS_FILE_INFO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_FILE_INFO))
#define ATHENA_FILE_INFO_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATHENA_TYPE_FILE_INFO, AthenaFileInfoIface))


#ifndef ATHENA_FILE_DEFINED
#define ATHENA_FILE_DEFINED
/* Using AthenaFile for the vtable to make implementing this in 
 * AthenaFile easier */
typedef struct AthenaFile          AthenaFile;
#endif

typedef AthenaFile                  AthenaFileInfo;
typedef struct _AthenaFileInfoIface AthenaFileInfoIface;


struct _AthenaFileInfoIface 
{
	GTypeInterface g_iface;

	gboolean          (*is_gone)              (AthenaFileInfo *file);
	
	char *            (*get_name)             (AthenaFileInfo *file);
	char *            (*get_uri)              (AthenaFileInfo *file);
	char *            (*get_parent_uri)       (AthenaFileInfo *file);
	char *            (*get_uri_scheme)       (AthenaFileInfo *file);
	
	char *            (*get_mime_type)        (AthenaFileInfo *file);
	gboolean          (*is_mime_type)         (AthenaFileInfo *file,
						   const char       *mime_Type);
	gboolean          (*is_directory)         (AthenaFileInfo *file);
	
	void              (*add_emblem)           (AthenaFileInfo *file,
						   const char       *emblem_name);
	char *            (*get_string_attribute) (AthenaFileInfo *file,
						   const char       *attribute_name);
	void              (*add_string_attribute) (AthenaFileInfo *file,
						   const char       *attribute_name,
						   const char       *value);
	void              (*invalidate_extension_info) (AthenaFileInfo *file);
	
	char *            (*get_activation_uri)   (AthenaFileInfo *file);

	GFileType         (*get_file_type)        (AthenaFileInfo *file);
	GFile *           (*get_location)         (AthenaFileInfo *file);
	GFile *           (*get_parent_location)  (AthenaFileInfo *file);
	AthenaFileInfo* (*get_parent_info)      (AthenaFileInfo *file);
	GMount *          (*get_mount)            (AthenaFileInfo *file);
	gboolean          (*can_write)            (AthenaFileInfo *file);
  
};

GList            *athena_file_info_list_copy            (GList            *files);
void              athena_file_info_list_free            (GList            *files);
GType             athena_file_info_get_type             (void);

/* Return true if the file has been deleted */
gboolean          athena_file_info_is_gone              (AthenaFileInfo *file);

/* Name and Location */
GFileType         athena_file_info_get_file_type        (AthenaFileInfo *file);
GFile *           athena_file_info_get_location         (AthenaFileInfo *file);
char *            athena_file_info_get_name             (AthenaFileInfo *file);
char *            athena_file_info_get_uri              (AthenaFileInfo *file);
char *            athena_file_info_get_activation_uri   (AthenaFileInfo *file);
GFile *           athena_file_info_get_parent_location  (AthenaFileInfo *file);
char *            athena_file_info_get_parent_uri       (AthenaFileInfo *file);
GMount *          athena_file_info_get_mount            (AthenaFileInfo *file);
char *            athena_file_info_get_uri_scheme       (AthenaFileInfo *file);
/* It's not safe to call this recursively multiple times, as it works
 * only for files already cached by Athena.
 */
AthenaFileInfo* athena_file_info_get_parent_info      (AthenaFileInfo *file);

/* File Type */
char *            athena_file_info_get_mime_type        (AthenaFileInfo *file);
gboolean          athena_file_info_is_mime_type         (AthenaFileInfo *file,
							   const char       *mime_type);
gboolean          athena_file_info_is_directory         (AthenaFileInfo *file);
gboolean          athena_file_info_can_write            (AthenaFileInfo *file);


/* Modifying the AthenaFileInfo */
void              athena_file_info_add_emblem           (AthenaFileInfo *file,
							   const char       *emblem_name);
char *            athena_file_info_get_string_attribute (AthenaFileInfo *file,
							   const char       *attribute_name);
void              athena_file_info_add_string_attribute (AthenaFileInfo *file,
							   const char       *attribute_name,
							   const char       *value);

/* Invalidating file info */
void              athena_file_info_invalidate_extension_info (AthenaFileInfo *file);

AthenaFileInfo *athena_file_info_lookup                (GFile *location);
AthenaFileInfo *athena_file_info_create                (GFile *location);
AthenaFileInfo *athena_file_info_lookup_for_uri        (const char *uri);
AthenaFileInfo *athena_file_info_create_for_uri        (const char *uri);

G_END_DECLS

#endif
