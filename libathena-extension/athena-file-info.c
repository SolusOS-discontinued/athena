/*
 *  athena-file-info.c - Information about a file 
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

#include <config.h>
#include "athena-file-info.h"
#include "athena-extension-private.h"

AthenaFileInfo *(*athena_file_info_getter) (GFile *location, gboolean create);

/**
 * athena_file_info_list_copy:
 * @files: (element-type AthenaFileInfo): the files to copy
 *
 * Returns: (element-type AthenaFileInfo) (transfer full): a copy of @files.
 *  Use #athena_file_info_list_free to free the list and unref its contents.
 */
GList *
athena_file_info_list_copy (GList *files)
{
	GList *ret;
	GList *l;
	
	ret = g_list_copy (files);
	for (l = ret; l != NULL; l = l->next) {
		g_object_ref (G_OBJECT (l->data));
	}

	return ret;
}

/**
 * athena_file_info_list_free:
 * @files: (element-type AthenaFileInfo): a list created with
 *   #athena_file_info_list_copy
 *
 */
void              
athena_file_info_list_free (GList *files)
{
	GList *l;
	
	for (l = files; l != NULL; l = l->next) {
		g_object_unref (G_OBJECT (l->data));
	}
	
	g_list_free (files);
}

static void
athena_file_info_base_init (gpointer g_class)
{
}

GType                   
athena_file_info_get_type (void)
{
	static GType type = 0;

	if (!type) {
		const GTypeInfo info = {
			sizeof (AthenaFileInfoIface),
			athena_file_info_base_init,
			NULL,
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		
		type = g_type_register_static (G_TYPE_INTERFACE, 
					       "AthenaFileInfo",
					       &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}

	return type;
}

gboolean
athena_file_info_is_gone (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), FALSE);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->is_gone != NULL, FALSE);
	
	return ATHENA_FILE_INFO_GET_IFACE (file)->is_gone (file);
}

GFileType
athena_file_info_get_file_type (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), G_FILE_TYPE_UNKNOWN);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_file_type != NULL, G_FILE_TYPE_UNKNOWN);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_file_type (file);
}

char *
athena_file_info_get_name (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_name != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_name (file);
}

/**
 * athena_file_info_get_location:
 * @file: a #AthenaFileInfo
 *
 * Returns: (transfer full): a #GFile for the location of @file
 */
GFile *
athena_file_info_get_location (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_location != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_location (file);
}
char *
athena_file_info_get_uri (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_uri != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_uri (file);
}

char *
athena_file_info_get_activation_uri (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_activation_uri != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_activation_uri (file);
}

/**
 * athena_file_info_get_parent_location:
 * @file: a #AthenaFileInfo
 *
 * Returns: (allow-none) (transfer full): a #GFile for the parent location of @file, 
 *   or %NULL if @file has no parent
 */
GFile *
athena_file_info_get_parent_location (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_parent_location != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_parent_location (file);
}

char *
athena_file_info_get_parent_uri (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_parent_uri != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_parent_uri (file);
}

/**
 * athena_file_info_get_parent_info:
 * @file: a #AthenaFileInfo
 *
 * Returns: (allow-none) (transfer full): a #AthenaFileInfo for the parent of @file, 
 *   or %NULL if @file has no parent
 */
AthenaFileInfo *
athena_file_info_get_parent_info (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_parent_info != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_parent_info (file);
}

/**
 * athena_file_info_get_mount:
 * @file: a #AthenaFileInfo
 *
 * Returns: (allow-none) (transfer full): a #GMount for the mount of @file, 
 *   or %NULL if @file has no mount
 */
GMount *
athena_file_info_get_mount (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_mount != NULL, NULL);
    
	return ATHENA_FILE_INFO_GET_IFACE (file)->get_mount (file);
}

char *
athena_file_info_get_uri_scheme (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_uri_scheme != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_uri_scheme (file);
}

char *
athena_file_info_get_mime_type (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_mime_type != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_mime_type (file);
}

gboolean
athena_file_info_is_mime_type (AthenaFileInfo *file,
				 const char *mime_type)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), FALSE);
	g_return_val_if_fail (mime_type != NULL, FALSE);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->is_mime_type != NULL, FALSE);

	return ATHENA_FILE_INFO_GET_IFACE (file)->is_mime_type (file,
								  mime_type);
}

gboolean
athena_file_info_is_directory (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), FALSE);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->is_directory != NULL, FALSE);

	return ATHENA_FILE_INFO_GET_IFACE (file)->is_directory (file);
}

gboolean
athena_file_info_can_write (AthenaFileInfo *file)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), FALSE);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->can_write != NULL, FALSE);

	return ATHENA_FILE_INFO_GET_IFACE (file)->can_write (file);
}

void
athena_file_info_add_emblem (AthenaFileInfo *file,
			       const char *emblem_name)
{
	g_return_if_fail (ATHENA_IS_FILE_INFO (file));
	g_return_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->add_emblem != NULL);

	ATHENA_FILE_INFO_GET_IFACE (file)->add_emblem (file, emblem_name);
}

char *
athena_file_info_get_string_attribute (AthenaFileInfo *file,
					 const char *attribute_name)
{
	g_return_val_if_fail (ATHENA_IS_FILE_INFO (file), NULL);
	g_return_val_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->get_string_attribute != NULL, NULL);
	g_return_val_if_fail (attribute_name != NULL, NULL);

	return ATHENA_FILE_INFO_GET_IFACE (file)->get_string_attribute 
		(file, attribute_name);
}

void
athena_file_info_add_string_attribute (AthenaFileInfo *file,
					 const char *attribute_name,
					 const char *value)
{
	g_return_if_fail (ATHENA_IS_FILE_INFO (file));
	g_return_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->add_string_attribute != NULL);
	g_return_if_fail (attribute_name != NULL);
	g_return_if_fail (value != NULL);
	
	ATHENA_FILE_INFO_GET_IFACE (file)->add_string_attribute 
		(file, attribute_name, value);
}

void
athena_file_info_invalidate_extension_info (AthenaFileInfo *file)
{
	g_return_if_fail (ATHENA_IS_FILE_INFO (file));
	g_return_if_fail (ATHENA_FILE_INFO_GET_IFACE (file)->invalidate_extension_info != NULL);
	
	ATHENA_FILE_INFO_GET_IFACE (file)->invalidate_extension_info (file);
}

/**
 * athena_file_info_lookup:
 * @location: the location to lookup the file info for
 *
 * Returns: (transfer full): a #AthenaFileInfo
 */
AthenaFileInfo *
athena_file_info_lookup (GFile *location)
{
	return athena_file_info_getter (location, FALSE);
}

/**
 * athena_file_info_create:
 * @location: the location to create the file info for
 *
 * Returns: (transfer full): a #AthenaFileInfo
 */
AthenaFileInfo *
athena_file_info_create (GFile *location)
{
	return athena_file_info_getter (location, TRUE);
}

/**
 * athena_file_info_lookup_for_uri:
 * @uri: the URI to lookup the file info for
 *
 * Returns: (transfer full): a #AthenaFileInfo
 */
AthenaFileInfo *
athena_file_info_lookup_for_uri (const char *uri)
{
	GFile *location;
	AthenaFile *file;

	location = g_file_new_for_uri (uri);
	file = athena_file_info_lookup (location);
	g_object_unref (location);

	return file;
}

/**
 * athena_file_info_create_for_uri:
 * @uri: the URI to lookup the file info for
 *
 * Returns: (transfer full): a #AthenaFileInfo
 */
AthenaFileInfo *
athena_file_info_create_for_uri (const char *uri)
{
	GFile *location;
	AthenaFile *file;

	location = g_file_new_for_uri (uri);
	file = athena_file_info_create (location);
	g_object_unref (location);

	return file;
}
