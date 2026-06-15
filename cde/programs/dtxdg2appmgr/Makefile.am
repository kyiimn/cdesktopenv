MAINTAINERCLEANFILES = Makefile.in

if HAVE_GLIB2
bin_PROGRAMS = dtxdg2appmgr
endif

dtxdg2appmgr_SOURCES = options.c desktop_parser.c exec_parser.c icon_resolver.c \
	xpm_converter.c dt_writer.c cache.c path_scanner.c \
	category_mapper.c dtxdg2appmgr.c

dtxdg2appmgr_CPPFLAGS = $(GLIB_CFLAGS) $(EXTRA_INCS) \
	-DCDE_INSTALLATION_TOP=\"$(CDE_INSTALLATION_TOP)\" \
	-DCDE_CONFIGURATION_TOP=\"$(CDE_CONFIGURATION_TOP)\" \
	-DCDE_LOGFILES_TOP=\"$(CDE_LOGFILES_TOP)\"

dtxdg2appmgr_LDADD = $(GLIB_LIBS) $(EXTRA_LIBS)

noinst_HEADERS = options.h desktop_parser.h exec_parser.h icon_resolver.h \
	xpm_converter.h dt_writer.h cache.h path_scanner.h \
	category_mapper.h