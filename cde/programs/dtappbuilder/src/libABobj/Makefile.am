MAINTAINERCLEANFILES = Makefile.in

ABINCLUDES = ../include

EXTRA_DIST = \
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

EXTRA_DEFINES =  -D_POSIX_SOURCE=1

if SOLARIS
EXTRALIBS = -lgen
endif

noinst_LIBRARIES = libABobj.a

#noinst_PROGRAMS = test test_notify

libABobj_a_CFLAGS = $(EXTRA_DEFINES) -I$(ABINCLUDES)

libABobj_a_SOURCES = obj.c \
		     obj_action.c \
		     obj_alloc.c \
		     obj_fields.c \
		     obj_geometry.c \
		     obj_init.c \
		     obj_isa.c \
		     obj_names_list.c \
		     obj_list.c \
		     obj_lists.c \
		     obj_notify.c \
		     obj_release.c \
		     obj_scoped.c \
		     obj_select.c \
		     obj_tree.c \
		     obj_utils.c \
		     trav.c \
		     travP.c \
		     trav_public.c \
		     trav_safe.c

#test_LDADD = ../libAButil/libAButil.a libABobj.a $(EXTRALIBS)

#test_SOURCES = test.c

#test_notify_LDADD = ../libAButil/libAButil.a libABobj.a $(EXTRALIBS)

#test_notify_SOURCES = test_notify.c

noinst_HEADERS = gil.h \
	objP.h \
	obj_motifdefs.h \
	obj_names_listP.h \
	obj_notifyP.h \
	obj_utils.h \
	template.h \
	travP.h
