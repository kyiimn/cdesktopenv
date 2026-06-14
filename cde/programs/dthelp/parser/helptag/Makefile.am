MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = helptag.cpp

bin_SCRIPTS = dthelptag

dthelptag: helptag.cpp
	$(RM) $@
	$(GENCPP) -P -DXCOMM=# $(LOCAL_CPP_DEFINES) $< > $@
	chmod +x $@
