include $(top_srcdir)/Makefile.shared

noinst_LTLIBRARIES=libathena-private.la

INCLUDES = 						\
	-I$(top_srcdir) 				\
	-I$(top_builddir) 				\
	-I$(top_srcdir)/cut-n-paste-code		\
	$(BASE_CFLAGS)					\
	$(COMMON_CFLAGS)				\
	$(ATHENA_CFLAGS)				\
	$(WARNING_CFLAGS)				\
	$(DISABLE_DEPRECATED_CFLAGS)			\
	$(TRACKER_CFLAGS)				\
	-DDATADIR=\""$(datadir)"\" 			\
	-DSYSCONFDIR=\""$(sysconfdir)"\" 			\
	-DATHENA_DATADIR=\""$(datadir)/athena"\" 	\
	-DATHENA_EXTENSIONDIR=\""$(libdir)/athena/extensions-3.0"\" \
	$(NULL)

dbus_built_sources = athena-generated.c athena-generated.h
$(dbus_built_sources) : Makefile.am $(top_srcdir)/data/dbus-interfaces.xml
	gdbus-codegen									\
		--interface-prefix com.solusos.Athena.					\
		--c-namespace AthenaDBus						\
		--c-generate-object-manager						\
		--generate-c-code athena-generated					\
		$(top_srcdir)/data/dbus-interfaces.xml					\
		$(NULL)

BUILT_SOURCES = \
	$(dbus_built_sources) \
	$(NULL)

if ENABLE_TRACKER
athena_tracker_engine_sources = \
	athena-search-engine-tracker.c \
	athena-search-engine-tracker.h
endif

dependency_static_libs = \
	$(top_builddir)/cut-n-paste-code/libegg/libegg.la \
	$(NULL)

libathena_private_la_LDFLAGS =	\
	-no-undefined			\
	$(NULL)

libathena_private_la_LIBADD =		\
	$(dependency_static_libs)	\
	$(SELINUX_LIBS)			\
	$(TRACKER_LIBS)			\
	$(top_builddir)/eel/libeel-2.la \
	$(top_builddir)/libathena-extension/libathena-extension.la \
	$(BASE_LIBS)			\
	$(COMMON_LIBS)			\
	$(ATHENA_LIBS)		\
	-lm				\
	$(NULL)

libathena_private_la_SOURCES = \
	athena-bookmark.c \
	athena-bookmark.h \
	athena-cell-renderer-text-ellipsized.c \
	athena-cell-renderer-text-ellipsized.h \
	athena-clipboard-monitor.c \
	athena-clipboard-monitor.h \
	athena-clipboard.c \
	athena-clipboard.h \
	athena-column-chooser.c \
	athena-column-chooser.h \
	athena-column-utilities.c \
	athena-column-utilities.h \
	athena-dbus-manager.c \
	athena-dbus-manager.h \
	athena-debug.c \
	athena-debug.h \
	athena-default-file-icon.c \
	athena-default-file-icon.h \
	athena-desktop-background.c \
	athena-desktop-background.h \
	athena-desktop-directory-file.c \
	athena-desktop-directory-file.h \
	athena-desktop-directory.c \
	athena-desktop-directory.h \
	athena-desktop-icon-file.c \
	athena-desktop-icon-file.h \
	athena-desktop-link-monitor.c \
	athena-desktop-link-monitor.h \
	athena-desktop-link.c \
	athena-desktop-link.h \
	athena-desktop-metadata.c \
	athena-desktop-metadata.h \
	athena-directory-async.c \
	athena-directory-notify.h \
	athena-directory-private.h \
	athena-directory.c \
	athena-directory.h \
	athena-dnd.c \
	athena-dnd.h \
	athena-entry.c \
	athena-entry.h \
	athena-file-attributes.h \
	athena-file-changes-queue.c \
	athena-file-changes-queue.h \
	athena-file-conflict-dialog.c \
	athena-file-conflict-dialog.h \
	athena-file-dnd.c \
	athena-file-dnd.h \
	athena-file-operations.c \
	athena-file-operations.h \
	athena-file-private.h \
	athena-file-queue.c \
	athena-file-queue.h \
	athena-file-utilities.c \
	athena-file-utilities.h \
	athena-file.c \
	athena-file.h \
	athena-generated.c \
	athena-generated.h \
	athena-global-preferences.c \
	athena-global-preferences.h \
	athena-icon-canvas-item.c \
	athena-icon-canvas-item.h \
	athena-icon-container.c \
	athena-icon-container.h \
	athena-icon-dnd.c \
	athena-icon-dnd.h \
	athena-icon-private.h \
	athena-icon-info.c \
	athena-icon-info.h \
	athena-icon-names.h \
	athena-lib-self-check-functions.c \
	athena-lib-self-check-functions.h \
	athena-link.c \
	athena-link.h \
	athena-merged-directory.c \
	athena-merged-directory.h \
	athena-metadata.h \
	athena-metadata.c \
	athena-mime-application-chooser.c \
	athena-mime-application-chooser.h \
	athena-module.c \
	athena-module.h \
	athena-monitor.c \
	athena-monitor.h \
	athena-progress-info.c \
	athena-progress-info.h \
	athena-progress-info-manager.c \
	athena-progress-info-manager.h \
	athena-program-choosing.c \
	athena-program-choosing.h \
	athena-recent.c \
	athena-recent.h \
	athena-saved-search-file.c \
	athena-saved-search-file.h \
	athena-search-directory.c \
	athena-search-directory.h \
	athena-search-directory-file.c \
	athena-search-directory-file.h \
	athena-search-engine.c \
	athena-search-engine.h \
	athena-search-engine-simple.c \
	athena-search-engine-simple.h \
	athena-selection-canvas-item.c \
	athena-selection-canvas-item.h \
	athena-signaller.h \
	athena-signaller.c \
	athena-query.c \
	athena-query.h \
	athena-thumbnails.c \
	athena-thumbnails.h \
	athena-trash-monitor.c \
	athena-trash-monitor.h \
	athena-tree-view-drag-dest.c \
	athena-tree-view-drag-dest.h \
	athena-ui-utilities.c \
	athena-ui-utilities.h \
	athena-undo-manager.c \
	athena-undo-manager.h \
	athena-undo-private.h \
	athena-undo-signal-handlers.c \
	athena-undo-signal-handlers.h \
	athena-undo-transaction.c \
	athena-undo-transaction.h \
	athena-undo.c \
	athena-undo.h \
	athena-vfs-directory.c \
	athena-vfs-directory.h \
	athena-vfs-file.c \
	athena-vfs-file.h \
	athena-file-undo-operations.c \
	athena-file-undo-operations.h \
	athena-file-undo-manager.c \
	athena-file-undo-manager.h \
	$(athena_tracker_engine_sources) \
	$(NULL)

nodist_libathena_private_la_SOURCES =\
	$(BUILT_SOURCES) \
	$(NULL)

$(lib_LTLIBRARIES): $(dependency_static_libs)

gsettingsschema_in_files = com.solusos.athena.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
.PRECIOUS: $(gsettings_SCHEMAS)

convertdir = $(datadir)/GConf/gsettings
convert_DATA = athena.convert

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

EXTRA_DIST =				\
	athena.convert		\
	$(gsettingsschema_in_files)     \
	$(TRACKER_SOURCES)		\
	$(NULL)

CLEANFILES = \
	$(BUILT_SOURCES)				\
	$(gsettings_SCHEMAS)				\
	$(NULL)

dist-hook:
	cd $(distdir); rm -f $(CLEANFILES)
