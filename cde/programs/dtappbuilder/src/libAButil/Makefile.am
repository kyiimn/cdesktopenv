MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = libAButil.msg \
	../include/ab/util_types.h \
	../include/ab_private/AB.h \
	../include/ab_private/MwmUtil.h \
	../include/ab_private/UilDef.h \
	../include/ab_private/XmAll.h \
	../include/ab_private/ab.h \
	../include/ab_private/ab_bil.h \
	../include/ab_private/ab_dnd.h \
	../include/ab_private/ab_resources.h \
	../include/ab_private/ab_utils.h \
	../include/ab_private/abio.h \
	../include/ab_private/abmf.h \
	../include/ab_private/abobj.h \
	../include/ab_private/abobjP.h \
	../include/ab_private/abobj_edit.h \
	../include/ab_private/abobj_list.h \
	../include/ab_private/abobj_set.h \
	../include/ab_private/abuil_load.h \
	../include/ab_private/abuil_print.h \
	../include/ab_private/appfw.h \
	../include/ab_private/attch_ed.h \
	../include/ab_private/bil.h \
	../include/ab_private/brws.h \
	../include/ab_private/brwsP.h \
	../include/ab_private/cgen.h \
	../include/ab_private/color_chooser.h \
	../include/ab_private/conn.h \
	../include/ab_private/connP.h \
	../include/ab_private/dtb_utils.h \
	../include/ab_private/gil.h \
	../include/ab_private/help.h \
	../include/ab_private/istr.h \
	../include/ab_private/message.h \
	../include/ab_private/obj.h \
	../include/ab_private/obj_list.h \
	../include/ab_private/obj_notify.h \
	../include/ab_private/objxm.h \
	../include/ab_private/pal.h \
	../include/ab_private/proj.h \
	../include/ab_private/projP.h \
	../include/ab_private/prop.h \
	../include/ab_private/propP.h \
	../include/ab_private/strlist.h \
	../include/ab_private/tmode.h \
	../include/ab_private/tmodeP.h \
	../include/ab_private/trav.h \
	../include/ab_private/ui_util.h \
	../include/ab_private/util.h \
	../include/ab_private/util_ds.h \
	../include/ab_private/util_err.h \
	../include/ab_private/vwr.h \
	../include/ab_private/x_util.h

ABINCLUDES = ../include

EXTRA_DEFINES =  -D_POSIX_SOURCE=1

noinst_LIBRARIES = libAButil.a

#noinst_PROGRAMS = util_test istr_test

libAButil_a_CFLAGS = $(EXTRA_DEFINES) -I$(ABINCLUDES)

libAButil_a_SOURCES = abio.c          \
		      abio_path.c     \
		      istr.c          \
		      strlist.c       \
		      util.c          \
		      util_convert.c  \
		      util_ds.c       \
		      util_err.c      \
		      util_file.c     \
		      util_string.c

#util_test_LDADD = libAButil.a

#util_test_CFLAGS = $(EXTRA_DEFINES) -I$(ABINCLUDES)

#util_test_SOURCES = util_test.c

#istr_test_LDADD = libAButil.a

#istr_test_CFLAGS = $(EXTRA_DEFINES) -I$(ABINCLUDES)

#istr_test_SOURCES = istr_test.c

noinst_HEADERS = istr_hash.h \
	template.h \
	utilP.h
