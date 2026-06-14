MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libWWL.a

libWWL_a_CXXFLAGS = -I../include

if SOLARIS
libWWL_a_CXXFLAGS += -DSYSV -DSVR4
endif

libWWL_a_SOURCES = WArgList.C      WCallback.C     WComposite.C \
		   WObject.C       WXmList.C       WXmMenu.C \
		   WXmRadioBox.C   WXmString.C

EXTRA_DIST = \
	../include/WWL/WApplicationShell.h \
	../include/WWL/WArgList.h \
	../include/WWL/WCallback.h \
	../include/WWL/WComposite.h \
	../include/WWL/WConstraint.h \
	../include/WWL/WCore.h \
	../include/WWL/WObject.h \
	../include/WWL/WOverrideShell.h \
	../include/WWL/WRect.h \
	../include/WWL/WShell.h \
	../include/WWL/WTimeOut.h \
	../include/WWL/WTopLevelShell.h \
	../include/WWL/WTransientShell.h \
	../include/WWL/WVendorShell.h \
	../include/WWL/WWMShell.h \
	../include/WWL/WXmArrowButton.h \
	../include/WWL/WXmArrowButtonGadget.h \
	../include/WWL/WXmBulletinBoard.h \
	../include/WWL/WXmCascadeButton.h \
	../include/WWL/WXmCascadeButtonGadget.h \
	../include/WWL/WXmCommand.h \
	../include/WWL/WXmDialogShell.h \
	../include/WWL/WXmDrawingArea.h \
	../include/WWL/WXmDrawnButton.h \
	../include/WWL/WXmFileSelectionBox.h \
	../include/WWL/WXmForm.h \
	../include/WWL/WXmFrame.h \
	../include/WWL/WXmGadget.h \
	../include/WWL/WXmLabel.h \
	../include/WWL/WXmLabelGadget.h \
	../include/WWL/WXmList.h \
	../include/WWL/WXmMainWindow.h \
	../include/WWL/WXmManager.h \
	../include/WWL/WXmMenu.h \
	../include/WWL/WXmMenuShell.h \
	../include/WWL/WXmMessageBox.h \
	../include/WWL/WXmPanedWindow.h \
	../include/WWL/WXmPrimitive.h \
	../include/WWL/WXmPushButton.h \
	../include/WWL/WXmPushButtonGadget.h \
	../include/WWL/WXmRadioBox.h \
	../include/WWL/WXmRowColumn.h \
	../include/WWL/WXmScale.h \
	../include/WWL/WXmScrollBar.h \
	../include/WWL/WXmScrolledWindow.h \
	../include/WWL/WXmSelectionBox.h \
	../include/WWL/WXmSeparator.h \
	../include/WWL/WXmSeparatorGadget.h \
	../include/WWL/WXmString.h \
	../include/WWL/WXmText.h \
	../include/WWL/WXmTextField.h \
	../include/WWL/WXmToggleButton.h \
	../include/WWL/WXmToggleButtonGadget.h \
	../include/WWL/wwl.h
