MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libManagers.a

# get our env
include $(top_srcdir)/programs/dtinfo/dtinfo_env.mk

# class info
include Classlist.mk

libManagers_a_CXXFLAGS = $(DTINFO_DEFINES) $(DTINFO_INCLUDES)
libManagers_a_SOURCES = ../Prelude.h $(Managers_SRCS)

RANLIB = echo
libManagers_a_AR = $(LD) -r -o

BUILT_SOURCES = Managers.msg
CLEANFILES = $(BUILT_SOURCES)

Managers.msg:
	cat $(Managers_MSGS) > $@

noinst_HEADERS = Managers.h \
	CatMgr.hh \
	ClientEventMgr.hh \
	ClientEventMgrX.hh \
	DatabaseMgr.hh \
	DatabaseMgrLEND.hh \
	EnvMgr.hh \
	ErrorMgr.hh \
	GlobalHistoryMgr.hh \
	GraphicsMgr.hh \
	HistoryList.hh \
	InputMgrX.hh \
	LibraryMgr.hh \
	LocalHistoryMgr.hh \
	MapMgr.hh \
	MarkListMgr.hh \
	MarkMgr.hh \
	MessageMgr.hh \
	NodeDict.hh \
	NodeListMgr.hh \
	NodeMgr.hh \
	PrefMgr.hh \
	PrintMgr.hh \
	SearchMgr.hh \
	SearchResultsMgr.hh \
	SearchScopeMgr.hh \
	ServiceMgr.hh \
	ServiceMgrX.hh \
	SessionMgr.hh \
	StyleSheetMgr.hh \
	TtIpcMgr.hh \
	UrlMgr.hh \
	WString.hh \
	WorkspaceMgr.hh