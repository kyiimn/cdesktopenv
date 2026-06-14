MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = dticon

dticon_CFLAGS = -D__TOOLTALK

dticon_LDADD = $(DTCLIENTLIBS) $(XTOOLLIB)

dticon_SOURCES = dtIconShell.c event.c event.h fileIO.c fileIO.h	\
                 fileIODialog.c globals.c graphics.c graphics.h		\
                 help.c image.c image.h main.c newIconDialog.c		\
                 process.c queryDialog.c stdErrDialog.c utils.c		\
                 utils.h constants.h externals.h help.h main.h

EXTRA_DIST = 	bitmaps/circle.xbm bitmaps/ellipse.xbm bitmaps/eraser.xbm bitmaps/face.xbm bitmaps/flood.xbm bitmaps/line.xbm bitmaps/point.xbm bitmaps/polygon.xbm bitmaps/polyline.xbm bitmaps/q_mark.xbm bitmaps/rectangle.xbm bitmaps/select.xbm pixmaps/Circle.pm pixmaps/CircleSolid.pm pixmaps/ConnecClosedSolid.pm pixmaps/Connected.pm pixmaps/ConnectedClosed.pm pixmaps/Ellipse.pm pixmaps/EllipseSolid.pm pixmaps/Eraser.pm pixmaps/Line.pm pixmaps/PaintBucket.pm pixmaps/Pencil.pm pixmaps/Rectangle.pm pixmaps/RectangleSolid.pm pixmaps/SelectArea.pm \
	dticon.msg \
	Dticon
noinst_HEADERS = constants.h \
	event.h \
	externals.h \
	fileIO.h \
	graphics.h \
	help.h \
	image.h \
	main.h \
	process.h \
	utils.h
