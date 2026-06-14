MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = 	Dtwm.defs.src sys.dtwmrc.src \
	dtwm.msg
bin_PROGRAMS = dtwm dtfplist

configdir = $(CDE_INSTALLATION_TOP)/config/C
dist_config_DATA = sys.dtwmrc

BUILT_SOURCES = sys.dtwmrc Dtwm.defs
CLEANFILES = sys.dtwmrc Dtwm.defs

AM_CPPFLAGS = -DLARGECURSORS -DR2_COMPAT -DOPAQUE -DSHAPE -DUNMAP_ON_RESTART \
	      -DBATCH_DRAG_REQUESTS -DCDE_INSTALLATION_TOP=\"$(CDE_INSTALLATION_TOP)\" \
	      -DCDE_CONFIGURATION_TOP=\"$(CDE_CONFIGURATION_TOP)\"

if HAVE_XFT
AM_CPPFLAGS += $(FREETYPE2_INCS)
endif

dtwm_LDADD = $(DTCLIENTLIBS) $(XTOOLLIB) $(top_builddir)/lib/DtFont/libDtFont.la
dtfplist_LDADD = $(DTCLIENTLIBS) $(TIRPCLIB) $(XTOOLLIB)

if HAVE_XFT
dtwm_LDADD += $(XFTLIB) $(FONTCONFIGLIB)
endif

if SOLARIS
dtwm_LDADD += -lintl -lresolv
dtfplist_LDADD += -lintl -lresolv
endif

dtwm_SOURCES = WmCDInfo.c      WmCDecor.c      WmCEvent.c \
               WmCPlace.c      WmColormap.c    WmError.c \
               WmEvent.c       WmEwmh.c        WmFeedback.c \
               WmFunction.c    WmGraphics.c    WmIDecor.c \
               WmIPlace.c      WmIconBox.c     WmImage.c \
               WmInitWs.c      WmKeyFocus.c    WmMain.c \
               WmManage.c      WmMenu.c        WmProperty.c \
               WmProtocol.c    WmResCvt.c      WmResParse.c \
               WmResource.c    WmSignal.c      WmWinConf.c \
               WmWinInfo.c     WmWinList.c     WmWinState.c \
               WmMultiHead.c   Button.c        Callback.c \
               Clock.c         DataBaseLoad.c  PanelS.c \
               Parse.c         PopupMenu.c     Session.c \
               UI.c            WmBackdrop.c    WmFP.c \
               WmHelp.c        WmIPC.c         WmInstantTitle.c \
               WmOL.c          WmParse.c       WmParseP.c \
               WmPresence.c    WmWrkspace.c    WmXSMP.c

dtfplist_SOURCES = Print.c Parse.c DataBaseLoad.c WmParse.c Session.c

SCRIPTFLAGS = -DCDE_INSTALLATION_TOP=$(CDE_INSTALLATION_TOP) \
              -DCDE_CONFIGURATION_TOP=$(CDE_CONFIGURATION_TOP)

Dtwm.defs: Dtwm.defs.src
	$(RM) $@
	$(GENCPP) -P -DXCOMM=# $(SCRIPTFLAGS) $< > $@

sys.dtwmrc: sys.dtwmrc.src
	$(RM) $@
	$(GENCPP) -P -DXCOMM=# $(SCRIPTFLAGS) $< > $@

noinst_HEADERS = Button.h \
	ButtonP.h \
	Clock.h \
	ClockP.h \
	DataBaseLoad.h \
	DtSvcInternal.h \
	PanelS.h \
	PanelSP.h \
	Parse.h \
	PopupMenu.h \
	UI.h \
	WmBackdrop.h \
	WmCDInfo.h \
	WmCDecor.h \
	WmCEvent.h \
	WmCPlace.h \
	WmColormap.h \
	WmError.h \
	WmEvent.h \
	WmEwmh.h \
	WmFeedback.h \
	WmFunction.h \
	WmGlobal.h \
	WmGraphics.h \
	WmHelp.h \
	WmIBitmap.h \
	WmICCC.h \
	WmIDecor.h \
	WmIPC.h \
	WmIPlace.h \
	WmIconBox.h \
	WmImage.h \
	WmInitWs.h \
	WmInstantTitle.h \
	WmKeyFocus.h \
	WmManage.h \
	WmMenu.h \
	WmMultiHead.h \
	WmOL.h \
	WmPanelP.h \
	WmParse.h \
	WmParseP.h \
	WmPresence.h \
	WmProperty.h \
	WmProtocol.h \
	WmResCvt.h \
	WmResNames.h \
	WmResParse.h \
	WmResource.h \
	WmSignal.h \
	WmWinConf.h \
	WmWinInfo.h \
	WmWinList.h \
	WmWinState.h \
	WmWrkspace.h \
	WmXSMP.h
