MAINTAINERCLEANFILES = Makefile.in

APPBUILDER = AppBuilder.sdl

# this sets the LANG variables
include $(top_srcdir)/programs/localized/templates/Japanese.am
# this does all the work
include $(top_srcdir)/doc/common/help/sdl-docs.am
# install all of the extra assets
include $(top_srcdir)/doc/common/help/sdl-data.am

# The common/ directory contains shared SGML entities
# needed by help system processing. These must be distributed.
EXTRA_DIST += common/ProdEnt.sgm common/glossary.sgm
