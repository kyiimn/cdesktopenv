MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libutility.la

libutility_la_CXXFLAGS = -DPORTABLE_DB -I..

libutility_la_SOURCES = funcs.C         pm_random.C \
			atoi_pearson.C 	xtime.C         buffer.C \
		        atoi_larson.C   atomic_lock.C 	rw_lock.C \
		        atoi_fast.C     filter.C        mmdb_exception.C \
			randomize.C

noinst_HEADERS = atoi_fast.h \
	atoi_larson.h \
	atoi_pearson.h \
	atomic_lock.h \
	buffer.h \
	c_charbuf.h \
	c_filebuf.h \
	c_fstream.h \
	c_ios.h \
	c_iostream.h \
	c_stream.h \
	c_streambuf.h \
	c_string.h \
	c_stringstream.h \
	config.h \
	const.h \
	db_version.h \
	debug.h \
	filter.h \
	funcs.h \
	key.h \
	macro.h \
	mmdb_exception.h \
	pm_random.h \
	randomize.h \
	rw_lock.h \
	types.h \
	xclock.h \
	xtime.h
