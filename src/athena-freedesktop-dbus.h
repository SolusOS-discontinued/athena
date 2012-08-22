/*
 * athena-freedesktop-dbus: Implementation for the org.freedesktop DBus file-management interfaces
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Authors: Akshay Gupta <kitallis@gmail.com>
 *          Federico Mena Quintero <federico@gnome.org>
 */


#ifndef __ATHENA_FREEDESKTOP_DBUS_H__
#define __ATHENA_FREEDESKTOP_DBUS_H__

#include "athena-application.h"

void athena_freedesktop_dbus_start (AthenaApplication *app);
void athena_freedesktop_dbus_stop (void);

#endif /* __ATHENA_FREEDESKTOP_DBUS_H__ */
