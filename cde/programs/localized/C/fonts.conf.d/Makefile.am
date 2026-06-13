MAINTAINERCLEANFILES = Makefile.in

include ../../templates/C.am

cfgfontconfdir = $(CDE_INSTALLATION_TOP)/config/C/fonts.conf.d

dist_cfgfontconf_DATA = \
	09-cde-aliases.conf \
	09-cde-cjk.conf

install-data-hook:
	-if test -x /usr/bin/fc-cache; then \
		echo "Running fc-cache to generate fontconfig cache..."; \
		fc-cache -f $(DESTDIR)$(CDE_INSTALLATION_TOP)/config/C/fonts.conf.d 2>/dev/null || true; \
	fi