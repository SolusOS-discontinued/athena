/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Athena
 *
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * Athena is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Athena is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors: Cosimo Cecchi <cosimoc@redhat.com>
 */

#ifndef __ATHENA_DESKTOP_METADATA_H__
#define __ATHENA_DESKTOP_METADATA_H__

#include <glib.h>

#include <libathena-private/athena-file.h>

void athena_desktop_set_metadata_string (AthenaFile *file,
                                           const gchar *name,
                                           const gchar *key,
                                           const gchar *string);

void athena_desktop_set_metadata_stringv (AthenaFile *file,
                                            const char *name,
                                            const char *key,
                                            const char * const *stringv);

gboolean athena_desktop_update_metadata_from_keyfile (AthenaFile *file,
                                                        const gchar *name);

#endif /* __ATHENA_DESKTOP_METADATA_H__ */
