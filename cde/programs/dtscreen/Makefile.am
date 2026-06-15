
EXTRA_DIST = Dtscreen.src \
	lifeicon.bit \
	xlogo.bit \
	dtscreen.msg
MAITAINERCLEANFILES = Makefile.in

bin_PROGRAMS = dtscreen

dtscreen_LDADD = $(DTCLIENTLIBS) $(XTOOLLIB)

if SOLARIS
dtscreen_LDADD += -lgen
endif

dtscreen_SOURCES = blank.c dtscreen.c flame.c hopalong.c hsbramp.c	\
		   image.c life.c pyro.c qix.c resource.c rotor.c	\
		   swarm.c usleep.c worm.c dtscreen.h

LOCAL_CPP_DEFINES = -DCDE_INSTALLATION_TOP=${prefix}

CPP_TARGETS = Dtscreen
BUILT_SOURCES = $(CPP_TARGETS)
CLEANFILES = $(CPP_TARGETS)

Dtscreen: Dtscreen.src
	$(RM) $@
	$(GENCPP) -P -DXCOMM=# $(LOCAL_CPP_DEFINES) $< > $@
