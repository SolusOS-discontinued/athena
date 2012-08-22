CLEANFILES = $(NULL)

include $(top_srcdir)/Makefile.shared

lib_LTLIBRARIES=libathena-extension.la

INCLUDES=\
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(BASE_CFLAGS) \
	$(WARNING_CFLAGS) \
	$(DISABLE_DEPRECATED_CFLAGS) \
	-DDATADIR=\""$(datadir)"\" \
	$(NULL)

libathena_extension_la_LDFLAGS=\
	-version-info @ATHENA_EXTENSION_VERSION_INFO@ \
	-no-undefined \
	$(NULL)

libathena_extension_la_LIBADD = $(BASE_LIBS)

libathena_extension_includedir=$(includedir)/athena/libathena-extension

libathena_extension_include_HEADERS=		\
	athena-column-provider.h              \
	athena-column.h                       \
	athena-extension-types.h              \
	athena-file-info.h                    \
	athena-info-provider.h                \
	athena-location-widget-provider.h     \
	athena-menu-item.h                    \
	athena-menu-provider.h                \
	athena-property-page-provider.h       \
	athena-property-page.h                \
	athena-menu.h                         \
	$(NULL)

libathena_extension_la_SOURCES=			\
	athena-column-provider.c                      \
	athena-column.c                               \
	athena-extension-i18n.h                       \
	athena-extension-types.c                      \
	athena-extension-private.h			\
	athena-file-info.c                            \
	athena-info-provider.c                        \
	athena-location-widget-provider.c             \
	athena-menu-item.c                            \
	athena-menu-provider.c                        \
	athena-property-page-provider.c               \
	athena-property-page.c                        \
	athena-menu.c                                 \
	$(NULL)

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=libathena-extension.pc

EXTRA_DIST = \
	libathena-extension.pc.in \
	$(NULL)

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_files =					\
	$(libathena_extension_include_HEADERS)	\
	$(libathena_extension_la_SOURCES)
Athena-3.0.gir: libathena-extension.la Makefile
Athena_3_0_gir_INCLUDES = Gtk-3.0 Gio-2.0 GLib-2.0
Athena_3_0_gir_CFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(BASE_CFLAGS)
Athena_3_0_gir_LIBS = libathena-extension.la
Athena_3_0_gir_FILES = $(addprefix $(srcdir)/, $(introspection_files))
INTROSPECTION_GIRS += Athena-3.0.gir

girdir = $(datadir)/gir-1.0/
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)
endif
