/*
 *  Athena SendTo extension
 *
 *  Copyright (C) 2005 Roberto Majadas
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Author: Roberto Majadas <roberto.majadas@openshine.com>
 *
 */

#ifndef ATHENA_NSTE_H
#define ATHENA_NSTE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define ATHENA_TYPE_NSTE  (athena_nste_get_type ())
#define ATHENA_NSTE(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), ATHENA_TYPE_NSTE, AthenaNste))
#define ATHENA_IS_NSTE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), ATHENA_TYPE_NSTE))

typedef struct _AthenaNste      AthenaNste;
typedef struct _AthenaNsteClass AthenaNsteClass;

struct _AthenaNste {
	GObject  __parent;
	gboolean nst_present;
};

struct _AthenaNsteClass {
	GObjectClass __parent;
};

GType athena_nste_get_type      (void);
void  athena_nste_register_type (GTypeModule *module);

G_END_DECLS

#endif /* ATHENA_NSTE_H */
