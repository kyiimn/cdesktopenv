MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = include/build/build.h \
	include/build/proto.h \
	include/eltdef/eltdef.h \
	include/eltdef/proto.h \
	include/helptag/fproto.h \
	include/helptag/global.h \
	include/helptag/roman8.h \
	include/helptag/special.h \
	include/parser/charconv.h \
	include/parser/parser.h \
	include/parser/proto.h \
	include/parser/tex.h \
	include/parser/userinc.h \
	include/util/basic.h \
	include/util/chartype.h \
	include/util/common.h \
	include/util/cont.h \
	include/util/dtddef.h \
	include/util/dtdext.h \
	include/util/entdef.h \
	include/util/entext.h \
	include/util/sparse.h \
	include/util/sref.h \
	include/util/trie.h \
	include/util/version.h

# The dthelp parser #includes .c files that automake can't detect.
# Copy source files that are not in _SOURCES to ensure complete distribution.
dist-hook:
	@for dir in canon1/build canon1/eltdef canon1/parser canon1/util \
		pass1/build pass1/eltdef pass1/parser pass1/util \
		pass2/build pass2/eltdef pass2/parser pass2/util; do \
		if test -d "$(srcdir)/$$dir"; then \
			$(MKDIR_P) "$(distdir)/$$dir"; \
			for f in $(srcdir)/$$dir/*.c $(srcdir)/$$dir/*.h; do \
				if test -f "$$f"; then \
					cp -p "$$f" "$(distdir)/$$dir/"; \
				fi; \
			done; \
		fi; \
	done

.NOTPARALLEL:

SUBDIRS = pass1 canon1 pass2 helptag
