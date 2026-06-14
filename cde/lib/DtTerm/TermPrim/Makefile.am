MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libTermPrim.la

AM_CPPFLAGS = $(DT_INCDIR)

if HAVE_XFT
AM_CPPFLAGS += $(FREETYPE2_INCS)
endif

libTermPrim_la_SOURCES = TermPrim.c \
        TermPrimAction.c \
        TermPrimBuffer.c \
        TermPrimBufferWc.c \
        TermPrimCursor.c \
        TermPrimDebug.c \
        TermPrimFunction.c \
        TermPrimLineDraw.c \
        TermPrimParse.c \
        TermPrimParseTable.c \
        TermPrimParser.c \
        TermPrimPendingText.c \
        TermPrimRender.c \
        TermPrimRenderMb.c \
        TermPrimRenderFont.c \
        TermPrimRenderFontSet.c \
        TermPrimRenderXft.c \
        TermPrimRenderLineDraw.c \
        TermPrimRepType.c \
	TermPrimScroll.c \
        TermPrimSelect.c \
        TermPrimSetPty.c \
        TermPrimSetUtmp.c \
        TermPrimSubproc.c \
        TermPrimUtil.c    \
        TermPrimVersion.c \
        TermPrimWMProtocols.c

# JET there may be some more missing here...

if SOLARIS
libTermPrim_la_SOURCES += TermPrimGetPty-svr4.c
else
libTermPrim_la_SOURCES += TermPrimGetPty-pts.c
endif

if HAVE_XFT
libTermPrim_la_LIBADD = $(XFTLIB) $(FONTCONFIGLIB)
endif

noinst_HEADERS = TermHeader.h \
	TermPrimAction.h \
	TermPrimBuffer.h \
	TermPrimBufferP.h \
	TermPrimCursor.h \
	TermPrimData.h \
	TermPrimDebug.h \
	TermPrimFunction.h \
	TermPrimGetPty.h \
	TermPrimI.h \
	TermPrimLineDraw.h \
	TermPrimLineDrawP.h \
	TermPrimLineFont.h \
	TermPrimLineFontP.h \
	TermPrimMessageCatI.h \
	TermPrimOSDepI.h \
	TermPrimParseTable.h \
	TermPrimParser.h \
	TermPrimParserP.h \
	TermPrimPendingText.h \
	TermPrimPendingTextP.h \
	TermPrimRender.h \
	TermPrimRenderFont.h \
	TermPrimRenderFontSet.h \
	TermPrimRenderLineDraw.h \
	TermPrimRenderP.h \
	TermPrimRenderXft.h \
	TermPrimRepType.h \
	TermPrimScroll.h \
	TermPrimSelect.h \
	TermPrimSelectP.h \
	TermPrimSetPty.h \
	TermPrimSetUtmp.h \
	TermPrimSubproc.h \
	TermPrimUtil.h \
	TermPrimWMProtocols.h
