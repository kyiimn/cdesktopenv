MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = 	Dthello.src \
	dthello.msg \
	Dthello
bin_PROGRAMS = dthello

dthello_LDADD = $(DTCLIENTLIBS) $(XTOOLLIB)

if HAVE_XFT
dthello_LDADD += $(XFTLIB) $(FONTCONFIGLIB)
endif

if SOLARIS
dthello_LDADD += -lgen
endif

if LINUX
dthello_LDADD += $(TIRPCLIB)
endif

dthello_SOURCES = dthello.c

if HAVE_XFT
dthello_CPPFLAGS = $(FREETYPE2_INCS)
endif

CPP_TARGETS = Dthello
BUILT_SOURCES = $(CPP_TARGETS)
CLEANFILES = $(CPP_TARGETS)

Dthello: Dthello.src
	$(RM) $@
	$(GENCPP) -P -DXCOMM=#  $< > $@

noinst_HEADERS = dthello.h
