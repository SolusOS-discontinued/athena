/*
 *  athena-extension-private.h - Type definitions for Athena extensions
 * 
 *  Copyright (C) 2009 Red Hat, Inc.
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
 *  Author: Alexander Larsson <alexl@redhat.com>
 * 
 */

#ifndef ATHENA_EXTENSION_PRIVATE_H
#define ATHENA_EXTENSION_PRIVATE_H

#include <libathena-extension/athena-file-info.h>

G_BEGIN_DECLS

extern AthenaFileInfo *(*athena_file_info_getter) (GFile *location, gboolean create);

G_END_DECLS

#endif
