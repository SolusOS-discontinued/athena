#ifndef ATHENA_ICON_INFO_H
#define ATHENA_ICON_INFO_H

#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdk.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Names for Athena's different zoom levels, from tiniest items to largest items */
typedef enum {
	ATHENA_ZOOM_LEVEL_SMALLEST,
	ATHENA_ZOOM_LEVEL_SMALLER,
	ATHENA_ZOOM_LEVEL_SMALL,
	ATHENA_ZOOM_LEVEL_STANDARD,
	ATHENA_ZOOM_LEVEL_LARGE,
	ATHENA_ZOOM_LEVEL_LARGER,
	ATHENA_ZOOM_LEVEL_LARGEST
} AthenaZoomLevel;

#define ATHENA_ZOOM_LEVEL_N_ENTRIES (ATHENA_ZOOM_LEVEL_LARGEST + 1)

/* Nominal icon sizes for each Athena zoom level.
 * This scheme assumes that icons are designed to
 * fit in a square space, though each image needn't
 * be square. Since individual icons can be stretched,
 * each icon is not constrained to this nominal size.
 */
#define ATHENA_ICON_SIZE_SMALLEST	16
#define ATHENA_ICON_SIZE_SMALLER	24
#define ATHENA_ICON_SIZE_SMALL	32
#define ATHENA_ICON_SIZE_STANDARD	48
#define ATHENA_ICON_SIZE_LARGE	72
#define ATHENA_ICON_SIZE_LARGER	96
#define ATHENA_ICON_SIZE_LARGEST     192

/* Maximum size of an icon that the icon factory will ever produce */
#define ATHENA_ICON_MAXIMUM_SIZE     320

typedef struct _AthenaIconInfo      AthenaIconInfo;
typedef struct _AthenaIconInfoClass AthenaIconInfoClass;


#define ATHENA_TYPE_ICON_INFO                 (athena_icon_info_get_type ())
#define ATHENA_ICON_INFO(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATHENA_TYPE_ICON_INFO, AthenaIconInfo))
#define ATHENA_ICON_INFO_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ATHENA_TYPE_ICON_INFO, AthenaIconInfoClass))
#define ATHENA_IS_ICON_INFO(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATHENA_TYPE_ICON_INFO))
#define ATHENA_IS_ICON_INFO_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ATHENA_TYPE_ICON_INFO))
#define ATHENA_ICON_INFO_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ATHENA_TYPE_ICON_INFO, AthenaIconInfoClass))


GType    athena_icon_info_get_type (void) G_GNUC_CONST;

AthenaIconInfo *    athena_icon_info_new_for_pixbuf               (GdkPixbuf         *pixbuf);
AthenaIconInfo *    athena_icon_info_lookup                       (GIcon             *icon,
								       int                size);
AthenaIconInfo *    athena_icon_info_lookup_from_name             (const char        *name,
								       int                size);
AthenaIconInfo *    athena_icon_info_lookup_from_path             (const char        *path,
								       int                size);
gboolean              athena_icon_info_is_fallback                  (AthenaIconInfo  *icon);
GdkPixbuf *           athena_icon_info_get_pixbuf                   (AthenaIconInfo  *icon);
GdkPixbuf *           athena_icon_info_get_pixbuf_nodefault         (AthenaIconInfo  *icon);
GdkPixbuf *           athena_icon_info_get_pixbuf_nodefault_at_size (AthenaIconInfo  *icon,
								       gsize              forced_size);
GdkPixbuf *           athena_icon_info_get_pixbuf_at_size           (AthenaIconInfo  *icon,
								       gsize              forced_size);
gboolean              athena_icon_info_get_embedded_rect            (AthenaIconInfo  *icon,
								       GdkRectangle      *rectangle);
gboolean              athena_icon_info_get_attach_points            (AthenaIconInfo  *icon,
								       GdkPoint         **points,
								       gint              *n_points);
const char *          athena_icon_info_get_display_name             (AthenaIconInfo  *icon);
const char *          athena_icon_info_get_used_name                (AthenaIconInfo  *icon);

void                  athena_icon_info_clear_caches                 (void);

/* Relationship between zoom levels and icons sizes. */
guint athena_get_icon_size_for_zoom_level          (AthenaZoomLevel  zoom_level);

gint  athena_get_icon_size_for_stock_size          (GtkIconSize        size);
guint athena_icon_get_emblem_size_for_icon_size    (guint              size);

gboolean athena_icon_theme_can_render              (GThemedIcon *icon);
GIcon * athena_user_special_directory_get_gicon (GUserDirectory directory);


G_END_DECLS

#endif /* ATHENA_ICON_INFO_H */

