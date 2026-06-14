MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libBasic.a

# get our env
include $(top_srcdir)/programs/dtinfo/dtinfo_env.mk

# class info
include Classlist.mk

libBasic_a_CXXFLAGS = $(DTINFO_DEFINES) $(DTINFO_INCLUDES)

libBasic_a_SOURCES = ../Prelude.h $(Basic_SRCS)

RANLIB = echo
libBasic_a_AR = $(LD) -r -o

noinst_HEADERS = Basic.h \
	AgentStack.hh \
	Atomizer.hh \
	BitField.hh \
	Buffer.hh \
	ClientEventHandler.hh \
	Database.hh \
	Error.hh \
	FString.hh \
	FolioObject.hh \
	HashTable.hh \
	HashTbl.hh \
	Hashable.hh \
	InfoBase.hh \
	Iterator.hh \
	LibraryElement.hh \
	List.hh \
	ListElement.hh \
	Long_Lived.hh \
	NamedData.hh \
	NodeDisplayData.hh \
	NodeDisplayDataTml.hh \
	NodeID.hh \
	NodeViewInfo.hh \
	OString.hh \
	OrderList.hh \
	OutlineElement.hh \
	OutlineList.hh \
	OutlineString.hh \
	ParseTree.hh \
	Preferences.hh \
	StringHash.hh \
	TOC_Element.hh \
	Tab.hh \
	VisualInfo.hh