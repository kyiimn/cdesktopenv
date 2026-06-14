MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libAgents.a

# get our env
include $(top_srcdir)/programs/dtinfo/dtinfo_env.mk

# class info
include Classlist.mk

libAgents_a_CXXFLAGS = $(DTINFO_DEFINES) $(DTINFO_INCLUDES)
libAgents_a_SOURCES = ../Prelude.h $(Agents_SRCS)

RANLIB = echo
libAgents_a_AR = $(LD) -r -o

BUILT_SOURCES = Agents.msg
CLEANFILES = $(BUILT_SOURCES)

Agents.msg:
	cat $(Agents_MSGS) > $@

noinst_HEADERS = Agents.h \
	AddLibraryAgent.hh \
	AddLibraryAgentMotif.hh \
	Agent.hh \
	AgentMotif.hh \
	AnchorCanvas.hh \
	BookTab.hh \
	BookTabMotif.hh \
	BookmarkEdit.hh \
	BookmarkEditMotif.hh \
	Dialog.hh \
	DialogMotif.hh \
	GraphicAgent.hh \
	GraphicAgentMotif.hh \
	HelpAgent.hh \
	IcccmAgent.hh \
	LibraryAgent.hh \
	LibraryAgentMotif.hh \
	ListView.hh \
	ListViewMotif.hh \
	MapAgent.hh \
	MapAgentMotif.hh \
	MarkCanvas.hh \
	MarkChooser.hh \
	MarkIcon.hh \
	MarkIconMotif.hh \
	MarkListView.hh \
	MessageAgent.hh \
	MessageAgentMotif.hh \
	NodeHistoryAgent.hh \
	NodeHistoryAgentMotif.hh \
	NodeListAgent.hh \
	NodeListAgentMotif.hh \
	NodePrintAgent.hh \
	NodeWindowAgent.hh \
	NodeWindowAgentMotif.hh \
	OutlineListView.hh \
	OutlineListViewMotif.hh \
	PrefAgent.hh \
	PrefAgentMotif.hh \
	PrintPanelAgent.hh \
	PrintPanelAgentMotif.hh \
	ScopeMenu.hh \
	SearchHistoryListView.hh \
	SearchHistoryListViewMotif.hh \
	SearchQueryAgent.hh \
	SearchQueryAgentMotif.hh \
	SearchResultsAgent.hh \
	SearchResultsAgentMotif.hh \
	SearchScopeAgent.hh \
	SearchScopeAgentMotif.hh \
	SearchScopeList.hh \
	UrlAgent.hh \
	UrlAgentMotif.hh \
	create_macros.hh

EXTRA_DIST = \
	AddLibraryAgent.d \
	Agent.d \
	Agents.d \
	AnchorCanvas.d \
	BookTab.d \
	BookmarkEdit.d \
	Dialog.d \
	GraphicAgent.d \
	HelpAgent.d \
	IcccmAgent.d \
	LibraryAgent.d \
	ListView.d \
	MapAgent.d \
	MarkCanvas.d \
	MarkChooser.d \
	MarkIcon.d \
	MarkListView.d \
	MessageAgent.d \
	NodeHistoryAgent.d \
	NodeListAgent.d \
	NodePrintAgent.d \
	NodeWindowAgent.d \
	OutlineListView.d \
	PrefAgent.d \
	PrintPanelAgent.d \
	ScopeMenu.d \
	SearchHistoryListView.d \
	SearchQueryAgent.d \
	SearchResultsAgent.d \
	SearchScopeAgent.d \
	SearchScopeList.d \
	UrlAgent.d
