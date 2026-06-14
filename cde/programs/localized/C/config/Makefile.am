MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = Xresources.tmsg \
	Xresources.ow.tmsg \
	_common.session.tmsg \
	dtfile.config.tmsg \
	sys.dtwmrc.tmsg \
	sys.font.tmsg \
	sys.resources.tmsg \
	sys.session.tmsg

include ../../templates/C.am
include ../../templates/config.am

# Setup a symlink from C -> en_US.UTF-8
install-data-hook:
	$(RM) $(DESTDIR)$(CDE_INSTALLATION_TOP)/config/en_US.UTF-8
	cd $(DESTDIR)$(CDE_INSTALLATION_TOP)/config/ && $(LN_S) C en_US.UTF-8

