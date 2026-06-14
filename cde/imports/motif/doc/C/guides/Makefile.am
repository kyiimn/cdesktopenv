MAINTAINERCLEANFILES = Makefile.in

# The motif documentation is large and deeply nested. Use a dist-hook
# to copy all source files rather than listing each one in EXTRA_DIST.
dist-hook:
	@$(MKDIR_P) "$(distdir)/common"
	cp -p $(srcdir)/common/BOOKDEFS.sgm "$(distdir)/common/"
	cp -p $(srcdir)/common/ManLinks.sgm "$(distdir)/common/"
	@for dir in PR1X PR3X PR3X_Mrm PR3X_Xm PR4X PR5X; do \
		if test -d "$(srcdir)/man/$$dir"; then \
			$(MKDIR_P) "$(distdir)/man/$$dir"; \
			cp -p $(srcdir)/man/$$dir/*.sgm "$(distdir)/man/$$dir/"; \
		fi; \
	done