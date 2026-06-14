MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = resource/Dtudcfonted dtudcfonted.msg \
	code.c comsub.c cpyw.c cpyx.c mtfgui.c selectx.c xutil.c \
	ufont.c util.c \
	fssxeg.h selectxlfd.h ufontrsrc.h xoakufont.h util.h

# dtudcfonted depends on internal libraries (libfuty, libfal) that are not built
# by the autotools system. Distribute source files only via dist-hook.

dist-hook:
	@for dir in include libfal libfuty dtaddcpf dtbdftocpf dtcpftobdf dtcpftogpf dtcpgpf dtgpftobdf dtgpftocpf dtlsgpf; do \
		if test -d "$(srcdir)/$$dir"; then \
			$(MKDIR_P) "$(distdir)/$$dir"; \
			for f in "$(srcdir)/$$dir"/*; do \
				if test -f "$$f"; then \
					base=`basename "$$f"`; \
					case "$$base" in \
						Makefile|Makefile.am|Makefile.in|*.o|*.lo|*.la|*.a|.libs|.deps) ;; \
						*) cp -p "$$f" "$(distdir)/$$dir/" ;; \
					esac; \
				fi; \
			done; \
		fi; \
	done