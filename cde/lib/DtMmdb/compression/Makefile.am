MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = sgml.ll ps.ll

noinst_LTLIBRARIES = libcompression.la

libcompression_la_CXXFLAGS = -I..

libcompression_la_SOURCES = abs_agent.C zip.C huffman.C trie.C code.C lzss.C \
			    sgml.C ps.C

AM_LFLAGS = -8 -s

CLEANFILES = sgml.C ps.C

sgml.C: sgml.ll
	$(LEX) $(AM_LFLAGS) -P sgml -o $@ $<

ps.C: ps.ll
	$(LEX) $(AM_LFLAGS) -P ps -o $@ $<

noinst_HEADERS = abs_agent.h \
	code.h \
	huffman.h \
	lzss.h \
	trie.h \
	zip.h
