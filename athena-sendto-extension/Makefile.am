include $(top_srcdir)/Makefile.shared

INCLUDES=\
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(BASE_CFLAGS) \
	$(WARNING_CFLAGS) \
	$(DISABLE_DEPRECATED_CFLAGS) \
	-DDATADIR=\""$(datadir)"\" \
	$(NULL)

athena_extensiondir = $(libdir)/athena/extensions-3.0
athena_extension_LTLIBRARIES = libathena-sendto.la
libathena_sendto_la_SOURCES =		\
	athena-nste.c			\
	athena-nste.h			\
	athena-sendto-module.c

libathena_sendto_la_LDFLAGS = -module -avoid-version -no-undefined
libathena_sendto_la_LIBADD  = \
  $(top_builddir)/libathena-extension/libathena-extension.la \
  $(BASE_LIBS)
