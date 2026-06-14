MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = TermLineData.data

CLEANFILES = TermLineData.c

noinst_LTLIBRARIES = libTerm.la

libTerm_la_CFLAGS = -I../TermPrim @DT_INCDIR@

TermLineData.c: TermLineData.data
	$(RM) $@
	../util/lineToData -p _DtTerm -f TermLineData.data > $@

includes:: TermLineData.c

libTerm_la_SOURCES = \
        Term.c \
        TermAction.c \
        TermBuffer.c \
        TermColor.c \
        TermEnhance.c \
        TermFunction.c \
        TermFunctionKey.c \
        TermLineData.c \
        TermParse.c \
        TermParseTable.c \
        TermVersion.c

noinst_HEADERS = TermAction.h \
	TermBuffer.h \
	TermBufferP.h \
	TermColor.h \
	TermData.h \
	TermEnhance.h \
	TermFunction.h \
	TermFunctionKey.h \
	TermFunctionKeyP.h \
	TermLineData.h \
	TermParseTable.h \
	TermParseTableP.h \
	TermSendEsc.h
