MAINTAINERCLEANFILES = Makefile.in

APPBUILDER = AppBuilder.sdl

# this sets the LANG variables
include $(top_srcdir)/programs/localized/templates/Japanese.am
# this does all the work
include $(top_srcdir)/doc/common/help/sdl-docs.am
# install all of the extra assets
include $(top_srcdir)/doc/common/help/sdl-data.am

# AppBuilder help sources only exist for en_US.UTF-8 and ja_JP.UTF-8,
# so they are distributed only from those locales.
EXTRA_DIST += $(APPBUILDER_SRCS)

# The common/ directory contains shared SGML entities
# needed by help system processing. These must be distributed.
EXTRA_DIST += common/ProdEnt.sgm common/glossary.sgm
