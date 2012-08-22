include $(top_srcdir)/Makefile.shared

bin_PROGRAMS=					\
	athena				\
	athena-autorun-software		\
	athena-connect-server			\
	$(NULL)

libexec_PROGRAMS=				\
	athena-convert-metadata		\
	$(NULL)

INCLUDES =							\
	-I$(top_srcdir) 					\
	-I$(top_srcdir)/cut-n-paste-code			\
	-I$(top_builddir)/libathena-private 			\
	$(BASE_CFLAGS) 						\
	$(COMMON_CFLAGS)					\
	$(ATHENA_CFLAGS)					\
	$(WARNING_CFLAGS)					\
	$(EXIF_CFLAGS)						\
	$(EXEMPI_CFLAGS)                                        \
	-DDATADIR=\""$(datadir)"\" 				\
	-DLIBDIR=\""$(libdir)"\" 				\
	-DLIBEXECDIR=\""$(libexecdir)"\" 			\
	-DATHENA_DATADIR=\""$(datadir)/athena"\" 		\
	-DUIDIR=\""$(datadir)/athena/ui"\" 		\
	-DATHENA_PIXMAPDIR=\""$(datadir)/pixmaps/athena"\" 	\
	-DPREFIX=\""$(prefix)"\" 				\
	-DSYSCONFDIR=\""$(sysconfdir)"\" 			\
	-DVERSION="\"$(VERSION)\"" 				\
	$(DISABLE_DEPRECATED_CFLAGS)                            \
	$(NULL)

LDADD =\
	$(top_builddir)/libathena-private/libathena-private.la  \
	$(BASE_LIBS) \
	$(COMMON_LIBS) \
	$(ATHENA_LIBS) \
	$(CORE_LIBS) \
	$(EXIF_LIBS) \
	$(EXEMPI_LIBS) \
	$(POPT_LIBS) \
	$(NULL)

dbus_built_sources = athena-freedesktop-generated.c athena-freedesktop-generated.h
$(dbus_built_sources) : Makefile.am $(top_srcdir)/data/freedesktop-dbus-interfaces.xml
	gdbus-codegen									\
		--interface-prefix org.freedesktop.					\
		--c-namespace AthenaFreedesktop					\
		--c-generate-object-manager						\
		--generate-c-code athena-freedesktop-generated			\
		$(top_srcdir)/data/freedesktop-dbus-interfaces.xml			\
		$(NULL)

UI_FILES = \
	athena-bookmarks-window.ui		\
	athena-file-management-properties.ui	\
	athena-icon-view-ui.xml		\
	athena-toolbar-ui.xml			\
	athena-directory-view-ui.xml		\
	athena-desktop-icon-view-ui.xml	\
	athena-list-view-ui.xml		\
	athena-shell-ui.xml			\
	$(NULL)

athena-resources.c: athena.gresource.xml $(UI_FILES)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-source --c-name athena $(srcdir)/athena.gresource.xml

athena-resources.h: athena.gresource.xml
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-header --c-name athena $(srcdir)/athena.gresource.xml


BUILT_SOURCES = \
	$(dbus_built_sources) \
	athena-resources.c \
	athena-resources.h \
	$(NULL)

athena_SOURCES = \
	athena-actions.h			\
	athena-application.c			\
	athena-application.h			\
	athena-bookmark-list.c		\
	athena-bookmark-list.h		\
	athena-bookmarks-window.c		\
	athena-bookmarks-window.h		\
	athena-connect-server-dialog.c	\
	athena-connect-server-dialog.h	\
	athena-connect-server-dialog-nonmain.c \
	athena-connect-server-operation.c	\
	athena-connect-server-operation.h	\
	athena-desktop-icon-view.c		\
	athena-desktop-icon-view.h		\
	athena-desktop-item-properties.c	\
	athena-desktop-item-properties.h	\
	athena-desktop-window.c		\
	athena-desktop-window.h		\
	athena-error-reporting.c		\
	athena-error-reporting.h		\
	athena-file-management-properties.c	\
	athena-file-management-properties.h	\
	athena-floating-bar.c			\
	athena-floating-bar.h			\
	athena-freedesktop-dbus.c		\
	athena-freedesktop-dbus.h		\
	athena-icon-view.c			\
	athena-icon-view.h			\
	athena-icon-view-container.c		\
	athena-icon-view-container.h		\
	athena-image-properties-page.c	\
	athena-image-properties-page.h	\
	athena-list-model.c			\
	athena-list-model.h			\
	athena-list-view-private.h 		\
	athena-list-view.c			\
	athena-list-view.h 			\
	athena-location-bar.c         	\
	athena-location-bar.h			\
	athena-location-entry.c               \
	athena-location-entry.h               \
	athena-main.c 			\
	athena-mime-actions.c 		\
	athena-mime-actions.h 		\
	athena-navigation-action.c		\
	athena-navigation-action.h		\
	athena-navigation-state.c		\
	athena-navigation-state.h		\
	athena-notebook.c			\
	athena-notebook.h			\
	athena-pathbar.c			\
	athena-pathbar.h			\
	athena-places-sidebar.c		\
	athena-places-sidebar.h		\
	athena-previewer.c			\
	athena-previewer.h			\
	athena-progress-info-widget.c		\
	athena-progress-info-widget.h		\
	athena-progress-ui-handler.c		\
	athena-progress-ui-handler.h		\
	athena-properties-window.c		\
	athena-properties-window.h		\
	athena-query-editor.c			\
	athena-query-editor.h			\
	athena-search-bar.c			\
	athena-search-bar.h			\
	athena-self-check-functions.c 	\
	athena-self-check-functions.h 	\
	athena-toolbar.c			\
	athena-toolbar.h			\
	athena-tree-sidebar.c			\
	athena-tree-sidebar.h			\
	athena-tree-sidebar-model.c		\
	athena-tree-sidebar-model.h		\
	athena-trash-bar.c			\
	athena-trash-bar.h			\
	athena-view.c				\
	athena-view.h				\
	athena-view-dnd.c			\
	athena-view-dnd.h			\
	athena-view-factory.c 		\
	athena-view-factory.h 		\
	athena-window-bookmarks.c		\
	athena-window-bookmarks.h		\
	athena-window-manage-views.c		\
	athena-window-manage-views.h		\
	athena-window-menus.c         	\
	athena-window-pane.c	\
	athena-window-pane.h	\
	athena-window-private.h		\
	athena-window-slot.c			\
	athena-window-slot.h			\
	athena-window-slot-dnd.c		\
	athena-window-slot-dnd.h		\
	athena-window.c			\
	athena-window.h			\
	athena-window-types.h			\
	athena-x-content-bar.c		\
	athena-x-content-bar.h		\
	$(NULL)

nodist_athena_SOURCES = \
	$(BUILT_SOURCES) \
	$(NULL)

EMPTY_VIEW_SOURCES = \
	athena-empty-view.c 			\
	athena-empty-view.h

if ENABLE_EMPTY_VIEW
athena_SOURCES += $(EMPTY_VIEW_SOURCES)
endif

athena_autorun_software_SOURCES= 			\
	athena-autorun-software.c			\
	$(NULL)

athena_connect_server_SOURCES= \
	athena-bookmark-list.c		\
	athena-bookmark-list.h		\
	athena-connect-server-dialog.c	\
	athena-connect-server-dialog.h	\
	athena-connect-server-dialog-main.c	\
	athena-connect-server-operation.c     \
	athena-connect-server-operation.h     \
	$(NULL)

athena_convert_metadata_SOURCES= \
	athena-convert-metadata.c	\
	$(NULL)

TESTS=check-athena

@INTLTOOL_SERVER_RULE@

CLEANFILES = \
	$(BUILT_SOURCES)			\
	$(server_DATA)				\
	$(NULL)

EXTRA_DIST =					\
	$(server_in_files)			\
	$(UI_FILES)				\
	athena.gresource.xml			\
	check-athena				\
	$(NULL)

dist-hook:
	cd $(distdir); rm -f $(CLEANFILES)
