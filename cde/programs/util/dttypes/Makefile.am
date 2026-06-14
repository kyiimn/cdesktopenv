MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = \
	dttypes.msg


bin_PROGRAMS = dttypes

dttypes_LDADD = $(DTCLIENTLIBS) $(XTOOLLIB)

if SOLARIS
dttypes_LDADD += -lgen
endif

