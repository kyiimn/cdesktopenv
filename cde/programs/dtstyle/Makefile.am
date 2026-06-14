MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = 	Backdrops.src Dtstyle.src Palettes.src \
	dtstyle.msg \
	Backdrops \
	Dtstyle \
	Palettes
bin_PROGRAMS = dtstyle

dtstyle_CPPFLAGS = -DCDE_CONFIGURATION_TOP=\"$(CDE_CONFIGURATION_TOP)\"	\
	-DCDE_INSTALLATION_TOP=\"$(CDE_INSTALLATION_TOP)\" -I../dthelp	\
	$(DT_INCDIR)

if HAVE_XFT
dtstyle_CPPFLAGS += $(FREETYPE2_INCS)
endif

dtstyle_LDADD = $(DTCLIENTLIBS) $(XTOOLLIB)

if HAVE_XFT
dtstyle_LDADD += $(XFTLIB) $(FONTCONFIGLIB)
endif

dtstyle_SOURCES = Main.c MainWin.c Font.c Audio.c Startup.c Mouse.c	\
		  SaveRestore.c Backdrop.c Screen.c Keyboard.c		\
		  ColorMain.c ColorPalette.c ColorFile.c ColorEdit.c	\
		  Resource.c Protocol.c Dtwm.c Help.c HelpCB.c		\
		  I18nMain.c I18nEnv.c I18nUtil.c Audio.h ColorMain.h	\
		  Help.h Keyboard.h OWsync.h Screen.h Backdrop.h	\
		  ColorPalette.h I18nEnv.h Main.h Protocol.h		\
		  Startup.h ColorEdit.h Dtwm.h I18nMain.h MainWin.h	\
		  Resource.h ColorFile.h Font.h I18nUtil.h Mouse.h	\
		  SaveRestore.h

if SOLARIS
dtstyle_SOURCES += OWsync.c
endif

LOCAL_CPP_DEFINES = -DCDE_INSTALLATION_TOP=${prefix}

CPP_TARGETS = Dtstyle Backdrops Palettes

BUILT_SOURCES = $(CPP_TARGETS)
CLEANFILES = $(CPP_TARGETS)

Dtstyle: Dtstyle.src
	$(RM) $@
	$(GENCPP) -P -DXCOMM=# $(LOCAL_CPP_DEFINES) $< > $@

Backdrops: Backdrops.src
	$(RM) $@
	$(GENCPP) -P -DXCOMM=# $(LOCAL_CPP_DEFINES) $< > $@

Palettes: Palettes.src
	$(RM) $@
	$(GENCPP) -P -DXCOMM=# $(LOCAL_CPP_DEFINES) $< > $@

noinst_HEADERS = Audio.h \
	Backdrop.h \
	ColorEdit.h \
	ColorFile.h \
	ColorMain.h \
	ColorPalette.h \
	Dtwm.h \
	Font.h \
	Help.h \
	I18nEnv.h \
	I18nMain.h \
	I18nUtil.h \
	Keyboard.h \
	Main.h \
	MainWin.h \
	Mouse.h \
	OWsync.h \
	Protocol.h \
	Resource.h \
	SaveRestore.h \
	Screen.h \
	Startup.h
