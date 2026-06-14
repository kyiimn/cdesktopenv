MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = wwl src install

# dtinfo uses generated source lists (Classlist.mk) that automake can't detect.
# Copy all source files to ensure complete distribution.
dist-hook:
	@echo "Copying dtinfo source files..."
	cp -R $(srcdir)/src "$(distdir)/"
	find "$(distdir)/src" -name '*.o' -delete
	find "$(distdir)/src" -name '*.lo' -delete
	find "$(distdir)/src" -name '*.la' -delete
	find "$(distdir)/src" -name '*.a' -delete
	find "$(distdir)/src" -name '.deps' -type d -exec rm -rf {} + 2>/dev/null || true
	find "$(distdir)/src" -name '.libs' -type d -exec rm -rf {} + 2>/dev/null || true
