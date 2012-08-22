/*
 * athena-previewer: athena previewer DBus wrapper
 *
 * Copyright (C) 2011, Red Hat, Inc.
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
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __ATHENA_PREVIEWER_H__
#define __ATHENA_PREVIEWER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define ATHENA_TYPE_PREVIEWER athena_previewer_get_type()
#define ATHENA_PREVIEWER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_PREVIEWER, AthenaPreviewer))
#define ATHENA_PREVIEWER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_PREVIEWER, AthenaPreviewerClass))
#define ATHENA_IS_PREVIEWER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_PREVIEWER))
#define ATHENA_IS_PREVIEWER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_PREVIEWER))
#define ATHENA_PREVIEWER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_PREVIEWER, AthenaPreviewerClass))

typedef struct _AthenaPreviewerPriv AthenaPreviewerPriv;

typedef struct {
  GObject parent;

  /* private */
  AthenaPreviewerPriv *priv;
} AthenaPreviewer;

typedef struct {
  GObjectClass parent_class;
} AthenaPreviewerClass;

GType athena_previewer_get_type (void);

AthenaPreviewer *athena_previewer_get_singleton (void);
void athena_previewer_call_show_file (AthenaPreviewer *previewer,
                                        const gchar *uri,
                                        guint xid,
					gboolean close_if_already_visible);
void athena_previewer_call_close (AthenaPreviewer *previewer);

G_END_DECLS

#endif /* __ATHENA_PREVIEWER_H__ */
