

#ifndef __ATHENA_RECENT_H__
#define __ATHENA_RECENT_H__

#include <gtk/gtk.h>
#include <libathena-private/athena-file.h>
#include <gio/gio.h>

void athena_recent_add_file (AthenaFile *file,
			       GAppInfo *application);

#endif
