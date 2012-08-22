/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
   
   athena-lib-self-check-functions.c: Wrapper for all self check functions
   in Athena proper.
 
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

#include <config.h>

#if ! defined (ATHENA_OMIT_SELF_CHECK)

#include "athena-lib-self-check-functions.h"

void
athena_run_lib_self_checks (void)
{
	ATHENA_LIB_FOR_EACH_SELF_CHECK_FUNCTION (EEL_CALL_SELF_CHECK_FUNCTION)
}

#endif /* ! ATHENA_OMIT_SELF_CHECK */
