/*
 * athena-debug: debug loggers for athena
 *
 * Copyright (C) 2007 Collabora Ltd.
 * Copyright (C) 2007 Nokia Corporation
 * Copyright (C) 2010 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Based on Empathy's empathy-debug.
 */

#ifndef __ATHENA_DEBUG_H__
#define __ATHENA_DEBUG_H__

#include <config.h>
#include <glib.h>

G_BEGIN_DECLS

#ifdef ENABLE_DEBUG

typedef enum {
  ATHENA_DEBUG_APPLICATION = 1 << 1,
  ATHENA_DEBUG_BOOKMARKS = 1 << 2,
  ATHENA_DEBUG_DBUS = 1 << 3,
  ATHENA_DEBUG_DIRECTORY_VIEW = 1 << 4,
  ATHENA_DEBUG_FILE = 1 << 5,
  ATHENA_DEBUG_ICON_CONTAINER = 1 << 6,
  ATHENA_DEBUG_ICON_VIEW = 1 << 7,
  ATHENA_DEBUG_LIST_VIEW = 1 << 8,
  ATHENA_DEBUG_MIME = 1 << 9,
  ATHENA_DEBUG_PLACES = 1 << 10,
  ATHENA_DEBUG_PREVIEWER = 1 << 11,
  ATHENA_DEBUG_SMCLIENT = 1 << 12,
  ATHENA_DEBUG_WINDOW = 1 << 13,
  ATHENA_DEBUG_UNDO = 1 << 14,
} DebugFlags;

void athena_debug_set_flags (DebugFlags flags);
gboolean athena_debug_flag_is_set (DebugFlags flag);

void athena_debug_valist (DebugFlags flag,
                            const gchar *format, va_list args);

void athena_debug (DebugFlags flag, const gchar *format, ...)
  G_GNUC_PRINTF (2, 3);

void athena_debug_files (DebugFlags flag, GList *files,
                           const gchar *format, ...) G_GNUC_PRINTF (3, 4);

#ifdef DEBUG_FLAG

#define DEBUG(format, ...) \
  athena_debug (DEBUG_FLAG, "%s: %s: " format, G_STRFUNC, G_STRLOC, \
                  ##__VA_ARGS__)

#define DEBUG_FILES(files, format, ...) \
  athena_debug_files (DEBUG_FLAG, files, "%s:" format, G_STRFUNC, \
                        ##__VA_ARGS__)

#define DEBUGGING athena_debug_flag_is_set(DEBUG_FLAG)

#endif /* DEBUG_FLAG */

#else /* ENABLE_DEBUG */

#ifdef DEBUG_FLAG

#define DEBUG(format, ...) \
  G_STMT_START { } G_STMT_END

#define DEBUG_FILES(files, format, ...) \
  G_STMT_START { } G_STMT_END

#define DEBUGGING 0

#endif /* DEBUG_FLAG */

#endif /* ENABLE_DEBUG */

G_END_DECLS

#endif /* __ATHENA_DEBUG_H__ */
