MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libMMDB.a

# get our env
include $(top_srcdir)/programs/dtinfo/dtinfo_env.mk

# class info
include Classlist.mk

libMMDB_a_CXXFLAGS = -I../Base -I../.. $(DTINFO_DEFINES)	\
	$(DTINFO_INCLUDES)
libMMDB_a_SOURCES = $(MMDB_SRCS)

RANLIB = echo
libMMDB_a_AR = $(LD) -r -o


BUILT_SOURCES = MMDB.msg
CLEANFILES = $(BUILT_SOURCES)

MMDB.msg:
	cat $(MMDB_MSGS) > $@

noinst_HEADERS = MMDB.hh \
	MMDB_BookCase.hh \
	MMDB_Common.hh \
	MMDB_EmbeddedObject.hh \
	MMDB_Factory.hh \
	MMDB_Library.hh \
	MMDB_Section.hh \
	MMDB_StyleSheet.hh
