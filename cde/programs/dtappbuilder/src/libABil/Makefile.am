MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = libABil.msg \
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

BUILT_SOURCES = bil_parse.h bil_parse.c
CLEANFILES = bil_parse.h bil_parse.c
AM_YFLAGS = -d

noinst_LIBRARIES = libABil.a

#noinst_PROGRAMS = bil_test

libABil_a_CFLAGS = $(EXTRA_DEFINES) -I$(ABINCLUDES)

libABil_a_LIBADD =

libABil_a_SOURCES = bil_parse.y abuil_load.c	  	  abuil_print.c \
		  abuil_resource_attr_map.c       abuil_trav.c \
		  abuil_widget_obj_map.c          bil.c \
		  bil_lexer.c                     bil_loadact.c \
		  bil_loadatt.c                   bil_loadfile.c \
		  bil_loadobj.c 		  bil_parse.y \
		  bil_store.c                     gil.c \
		  gil_loadact.c                   gil_loadatt.c \
		  gil_loadfile.c                  gil_loadobj.c \
		  load.c

#bil_test_LDADD = ./libABil.a \
#        ../libABobjXm/libABobjXm.a \
#        ../libABobj/libABobj.a \
#        ../libAButil/libAButil.a

#bil_test_SOURCES = bil_test.c

noinst_HEADERS = abuil_loadP.h \
	bilP.h \
	bil_loadattP.h \
	bil_parse.h \
	gilP.h \
	gil_loadattP.h \
	load.h \
	loadP.h \
	template.h
