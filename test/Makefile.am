NULL=

INCLUDES =\
	-I$(top_srcdir) \
	$(BASE_CFLAGS) \
	$(COMMON_CFLAGS) \
	$(ATHENA_CFLAGS) \
	$(WARNING_CFLAGS) \
	-DVERSION="\"$(VERSION)\"" \
	-DATHENA_DATADIR=\""$(datadir)/athena"\" \
	-DGNOMELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\"	\
	$(NULL)

LDADD =\
	$(top_builddir)/libathena-private/libathena-private.la \
	$(BASE_LIBS) \
	$(COMMON_LIBS) \
	$(ATHENA_LIBS) \
	$(NULL)

noinst_PROGRAMS =\
	test-athena-search-engine \
	test-athena-directory-async \
	test-athena-copy \
	test-eel-editable-label	\
	$(NULL)

test_athena_copy_SOURCES = test-copy.c test.c

test_athena_search_engine_SOURCES = test-athena-search-engine.c 

test_athena_directory_async_SOURCES = test-athena-directory-async.c

EXTRA_DIST = \
	test.h \
	$(NULL)
