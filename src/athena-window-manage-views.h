/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

/*
 *  Athena
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *
 *  Athena is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Athena is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Author: Darin Adler <darin@bentspoon.com>
 *
 */

#ifndef ATHENA_WINDOW_MANAGE_VIEWS_H
#define ATHENA_WINDOW_MANAGE_VIEWS_H

#include "athena-window.h"
#include "athena-window-pane.h"

void athena_window_manage_views_close_slot (AthenaWindowSlot *slot);


/* AthenaWindowInfo implementation: */
void athena_window_report_location_change   (AthenaWindow     *window);

#endif /* ATHENA_WINDOW_MANAGE_VIEWS_H */
