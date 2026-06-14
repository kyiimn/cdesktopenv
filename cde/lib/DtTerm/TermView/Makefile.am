MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libTermView.la

libTermView_la_CFLAGS = -I../TermPrim -I../Term @DT_INCDIR@

if HAVE_XFT
libTermView_la_CFLAGS += $(FREETYPE2_INCS)
libTermView_la_LIBADD = $(XFTLIB) $(FONTCONFIGLIB)
endif

libTermView_la_SOURCES = TermView.c \
			 TermViewMenu.c \
			 TermViewGlobalDialog.c \
			 TermViewTerminalDialog.c

noinst_HEADERS = TermView.h \
	TermViewGlobalDialog.h \
	TermViewHelp.h \
	TermViewMenu.h \
	TermViewP.h \
	TermViewTerminalDialog.h
