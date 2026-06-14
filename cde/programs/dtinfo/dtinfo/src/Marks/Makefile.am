MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libMarks.a

# get our env
include $(top_srcdir)/programs/dtinfo/dtinfo_env.mk

# class info
include Classlist.mk

libMarks_a_CXXFLAGS = $(DTINFO_DEFINES) $(DTINFO_INCLUDES)

libMarks_a_SOURCES = ../Prelude.h $(Marks_SRCS)

RANLIB = echo
libMarks_a_AR = $(LD) -r -o

noinst_HEADERS = Marks.h \
	Anchor.hh \
	Mark.hh \
	MarkBase.hh \
	MarkBase_mmdb.hh \
	MarkInfo.hh \
	Mark_mmdb.hh