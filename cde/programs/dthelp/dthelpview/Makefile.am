MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = dthelpview

dthelpview_CFLAGS = -D_BMS

dthelpview_LDADD = $(DTCLIENTLIBS) $(XTOOLLIB)

dthelpview_SOURCES = Main.c ManPage.c Util.c

noinst_HEADERS = Main.h \
	ManPageI.h \
	UtilI.h
