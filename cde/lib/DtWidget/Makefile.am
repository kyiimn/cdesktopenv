unixdir = $(CDE_INSTALLATION_TOP)/app-defaults/C

MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = libDtWidget.la

AM_CPPFLAGS = -DCDE_INSTALLATION_TOP=$(CDE_INSTALLATION_TOP) $(DT_INCDIR)

if HAVE_XFT
AM_CPPFLAGS += $(FREETYPE2_INCS)
endif

libDtWidget_la_CFLAGS = -DI18N_MSG -DMULTIBYTE

# in order to try to keep lib versions the same across platforms, (2.1.0)
if BSD
libDtWidget_la_LDFLAGS = -version-info 2:1:0
else
libDtWidget_la_LDFLAGS = -version-info 3:0:1
endif

if SOLARIS
libDtWidget_la_CFLAGS += -DNO_REGCOMP
libDtWidget_la_LIBADD = -lw -lintl
else
libDtWidget_la_LIBADD =
endif

if HAVE_XFT
libDtWidget_la_LIBADD += $(XFTLIB) $(FONTCONFIGLIB)
endif


libDtWidget_la_SOURCES = ComboBox.c      Control.c       DialogBox.c \
			 DtMsgs.c        DtStrDefs.c     EditAreaData.c \
			 EditCalls.c     Editor.c        Icon.c \
			 MenuButton.c    NewCvt.c        SearchCalls.c \
			 SearchDlg.c     SpinBox.c       TitleBox.c \
			 View.c

# cpp this...
BUILT_SOURCES = UNIXbindings
CLEANFILES = UNIXbindings

UNIXbindings: UNIXbindings.cpp
	$(RM) $@
	$(GENCPP) -P -DXCOMM=# $(AM_CPPFLAGS) $< > $@

EXTRA_DIST = UNIXbindings.cpp

noinst_HEADERS = DtSvcInternal.h \
	DtWidgetI.h
