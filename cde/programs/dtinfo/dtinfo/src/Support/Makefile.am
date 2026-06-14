MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libSupport.a

# get our env
include $(top_srcdir)/programs/dtinfo/dtinfo_env.mk

# class info
include Classlist.mk

libSupport_a_CXXFLAGS = $(DTINFO_DEFINES) $(DTINFO_INCLUDES)

libSupport_a_SOURCES = ../Prelude.h $(Support_SRCS)

RANLIB = echo
libSupport_a_AR = $(LD) -r -o

noinst_HEADERS = Support.h \
	InputNotifier.hh \
	List_base.hh \
	Pointer.hh \
	Receiver.hh \
	Sender.hh \
	Stack.hh \
	UtilityFunctions.hh \
	xList.hh

EXTRA_DIST = \
	InputNotifier.d \
	Pointer.d \
	Receiver.d \
	Sender.d \
	Stack.d \
	Support.d \
	xList.d
