MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = libDtFont.la

libDtFont_la_SOURCES = DtFont.c XftWrapper.c FontEnum.c

libDtFont_la_CPPFLAGS = -I./include -DMULTIBYTE -DNLS16 -I../include \
	@DT_INCDIR@

if HAVE_XFT
libDtFont_la_CPPFLAGS += $(FREETYPE2_INCS)
endif

# in order to try to keep lib versions the same across platforms, (2.1.0)
if BSD
libDtFont_la_LDFLAGS = -version-info 2:1:0
else
libDtFont_la_LDFLAGS = -version-info 3:0:1
endif

libDtFont_la_LIBADD = $(XFTLIB) $(FONTCONFIGLIB) $(XTOOLLIB)