MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libOther.a

# get our env
include $(top_srcdir)/programs/dtinfo/dtinfo_env.mk

# class info
include Classlist.mk

libOther_a_CXXFLAGS = $(DTINFO_DEFINES) $(DTINFO_INCLUDES)
libOther_a_SOURCES = ../Prelude.h $(Other_SRCS)

RANLIB = echo
libOther_a_AR = $(LD) -r -o

BUILT_SOURCES = Other.msg
CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = detached_bw.xbm \
	detached_bw.xpm \
	graphic_unavailable.xbm \
	graphic_unavailable.xpm

Other.msg:
	cat $(Other_MSGS) > $@

noinst_HEADERS = Other.h \
	vroot.h \
	AppPrintData.hh \
	WindowSystem.hh \
	WindowSystemMotif.hh \
	XmStringLocalized.hh \
	XpmLib.hh \
	oid_t.hh