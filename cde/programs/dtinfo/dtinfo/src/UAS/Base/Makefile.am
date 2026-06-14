MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libBase.a

# get our env
include $(top_srcdir)/programs/dtinfo/dtinfo_env.mk

# class info
include Classlist.mk

libBase_a_CXXFLAGS = -I.. -I ../.. -I../Support $(DTINFO_DEFINES)	\
	$(DTINFO_INCLUDES)
libBase_a_SOURCES = $(Base_SRCS)

RANLIB = echo
libBase_a_AR = $(LD) -r -o

BUILT_SOURCES = Base.msg
CLEANFILES = $(BUILT_SOURCES)

Base.msg:
	cat $(Base_MSGS) > $@

noinst_HEADERS = UAS.hh \
	UAS_Base.hh \
	UAS_BookcaseEntry.hh \
	UAS_Buffer.hh \
	UAS_Collection.hh \
	UAS_Common.hh \
	UAS_EmbeddedObject.hh \
	UAS_Exceptions.hh \
	UAS_Factory.hh \
	UAS_InputMgr.hh \
	UAS_Iterator.hh \
	UAS_LinkedObject.hh \
	UAS_List.hh \
	UAS_Msgs.hh \
	UAS_OQLParser.hh \
	UAS_ObjList.hh \
	UAS_Path.hh \
	UAS_Pointer.hh \
	UAS_PtrList.hh \
	UAS_Receiver.hh \
	UAS_SearchEngine.hh \
	UAS_SearchResults.hh \
	UAS_SearchResultsEntry.hh \
	UAS_SearchScope.hh \
	UAS_SearchZones.hh \
	UAS_Sender.hh \
	UAS_String.hh \
	UAS_StyleSheet.hh \
	UAS_TextRun.hh
