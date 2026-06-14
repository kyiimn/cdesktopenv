MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = dtcodegen.msg \
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

if BSD
UTILLIB = -lutil
endif

SOURCES.msg = \
        dtcodegen.msg   \
        ../libAButil/libAButil.msg \
        ../libABobjXm/libABobjXm.msg \
        ../libABil/libABil.msg

bin_PROGRAMS = dtcodegen

#noinst_PROGRAMS = ab_func_strings test_parse test_merge

noinst_DATA = dtcodegen.cat

dtcodegen_CFLAGS = $(EXTRA_DEFINES) -I$(ABINCLUDES) -I../libABil

dtcodegen_LDADD = ../libABil/libABil.a  \
		  ../libABobjXm/libABobjXm.a  \
                  ../libABobj/libABobj.a ../libAButil/libAButil.a \
		  $(LIBTERM) $(LIBSVC) $(LIBTT) $(LIBHELP) $(LIBXIN) \
		  $(LIBWIDGET) $(UTILLIB) $(XTOOLLIB)


dtcodegen_SOURCES = abmf.c                  args.c \
		    cdefs.c                 connect.c \
		    create_decls.c          dtb_utils.c \
		    generate_code.c         instances.c \
		    lib_func_strings.c      make_file.c \
		    merge_c.c               motifdefs.c \
		    msg_cvt.c               msg_file.c \
		    obj_names.c             parse_c.c \
		    proj_c_file.c           proj_header_file.c \
		    resource_file.c         stubs_c_file.c \
		    ui_c_file.c             ui_header_file.c \
		    utils.c                 utils_c_file.c \
		    utils_header_file.c     write_c.c \
		    write_code.c

#ab_func_strings_LDADD = ../libAButil/libAButil.a

#ab_func_strings_SOURCES = ab_func_strings.c

#test_parse_CFLAGS = -DMAIN -D_POSIX_SOURCE=1 -I$(ABINCLUDES)

#test_parse_LDADD = ../libAButil/libAButil.a

#test_parse_SOURCES = parse_c.c

#test_merge_CFLAGS = -DMAIN -D_POSIX_SOURCE=1 -I$(ABINCLUDES)

#test_merge_LDADD = ../libAButil/libAButil.a parse_c.o

#test_merge_SOURCES = merge_c.c

dtcodegen.cat:: $(SOURCES.msg)
	$(GENCAT) dtcodegen.cat $(SOURCES.msg)

noinst_HEADERS = abmfP.h \
	argsP.h \
	cdefsP.h \
	connectP.h \
	create_declsP.h \
	instancesP.h \
	lib_func_stringsP.h \
	make_fileP.h \
	merge_cP.h \
	motifdefsP.h \
	msg_cvt.h \
	msg_file.h \
	msg_fileP.h \
	obj_namesP.h \
	parse_cP.h \
	proj_c_fileP.h \
	proj_header_fileP.h \
	resource_fileP.h \
	stubs_c_fileP.h \
	template.h \
	ui_c_fileP.h \
	ui_header_fileP.h \
	utilsP.h \
	utils_c_fileP.h \
	utils_header_fileP.h \
	write_cP.h \
	write_codeP.h
