MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = \
	dtcm.msg \
	drag_xbm \
	drag_mask_xbm \
	icon_pixmap_xbm \
	contract_xbm \
	expand_xbm \
	postup_xbm \
	Dtcm.ad


bin_PROGRAMS = dtcm dtcm_delete dtcm_insert dtcm_lookup dtcm_editor

AM_CFLAGS = $(DT_INCDIR) $(CSA_INCDIR) -I../../../lib/csa	\
-I../libDtCmP

if HAVE_XFT
AM_CFLAGS += $(FREETYPE2_INCS)
endif

LDADD = ../libDtCmP/libDtCmP.a $(LIBCSA) $(DTCLIENTLIBS) $(XTOOLLIB)

if FREEBSD
LDADD += -lcompat -lutil
endif

if NETBSD
LDADD += -lcompat
endif

if HAVE_XFT
LDADD += $(XFTLIB) $(FONTCONFIGLIB)
endif

dtcm_SOURCES = MonthPanel.c    RFCMIME.c       about.c \
               alarm.c         blist.c         browser.c \
               calendarA.c     cm_i18n.c       cmtt.c \
               datefield.c     dayglance.c     deskset.c \
               dnd.c           dssw.c          editor.c \
               find.c          fns.c           font.c \
               format.c        goto.c          graphics.c \
               group_editor.c  help.c          i18nxlate.c \
               icon.c          md5.c           misc.c \
               monthglance.c   namesvc.c       print.c \
               props.c         props_pu.c      reminders.c \
               rfp.c           select.c        session.c \
	       submit.c        tempbr.c        timezone.c \
               todo.c          weekglance.c    x_graphics.c \
               yearglance.c

dtcm_delete_SOURCES = cm_delete.c

dtcm_insert_SOURCES = cm_insert.c

dtcm_lookup_SOURCES = cm_lookup.c

dtcm_editor_SOURCES = datefield.c     deskset.c       dssw.c \
		      dtcm_editor.c   help.c          misc.c \
		      reminders.c     rfp.c

install-exec-hook:
	$(INSTALL) -m 0755 -d $(DESTDIR)$(localstatedir)/spool/calendar

noinst_HEADERS = MonthPanel.h \
	MonthPanelP.h \
	RFCMIME.h \
	alarm.h \
	blist.h \
	browser.h \
	calendar.h \
	cm_i18n.h \
	cmtt.h \
	common.h \
	datefield.h \
	dayglance.h \
	deskset.h \
	dnd.h \
	dssw.h \
	editor.h \
	fallback.h \
	find.h \
	format.h \
	gettext.h \
	goto.h \
	group_editor.h \
	help.h \
	md5.h \
	md5global.h \
	misc.h \
	monthglance.h \
	print.h \
	props_pu.h \
	reminders.h \
	revision.h \
	rfp.h \
	select.h \
	session.h \
	tempbr.h \
	timezone.h \
	todo.h \
	weekglance.h \
	x_graphics.h \
	yearglance.h
