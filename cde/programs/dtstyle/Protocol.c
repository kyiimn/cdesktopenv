/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*$TOG: Protocol.c /main/6 1997/06/18 17:30:30 samborn $*/
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Protocol.c
 **
 **   Project:     DT 3.0
 **
 **   Description: inter-client communication for Dtstyle
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1993, 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>

#include <Xm/Protocols.h>


#include <Dt/Connect.h>
#include <Dt/SessionM.h>
#include <Dt/SessionP.h>
#include <Dt/Wsm.h>
#include <Dt/WsmP.h>

#include "MainWin.h"
#include "SaveRestore.h"
#include "Main.h"
#include "Backdrop.h"
#include "Font.h"

#include <Dt/Message.h>
#include <Dt/UserMsg.h>

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Protocol.h"

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/opt/dt"
#endif

#ifndef CDE_CONFIGURATION_TOP
#define CDE_CONFIGURATION_TOP "/etc/dt"
#endif

#ifndef PKEXECPATH
#define PKEXECPATH "/usr/bin/pkexec"
#endif

#define WS_STARTUP_RETRY_COUNT	12
#define WS_STARTUP_TIME		5000
#define ERR1   ((char *)GETMESSAGE(2, 4, "Cannot communicate with the session\nmanager... Exiting."))
#define ERR2   ((char *)GETMESSAGE(2, 9, "Could not obtain screen saver information\nfrom the session manager. Start up settings\nmay be incorrect."))

/*
 * Global variable definitions
 */
Window   smWindow;

static Atom     xaWmSaveYourself;
static Atom     xaWmDeleteWindow;

/* Atoms for client messages */
static Atom     xaSmStmProtocol;
static Atom     xaSmStateChange;
static Atom     xaSmRestoreDefault;
static Atom     xaSmLockChange;
static Atom     xaSmSaveToHome;

/* Atoms for window properties */
static Atom     xaDtSaveMode;
static Atom     xaDtSmAudioInfo;
static Atom     xaDtSmKeyboardInfo;
static Atom     xaDtSmPointerInfo;
static Atom     xaDtSmScreenInfo;
static Atom     xaDtSmFontInfo;
static Atom     xaDtSmPreeditInfo;

/* local function definitions */
static void SmRestoreDefault(Atom);
static int  SystemApplyFontDirect(char *fontResourceString);


/*************************************<->*************************************
 *
 *  InitProtocol ()
 *
 *
 *  Description:
 *  -----------
 *  Handles interning of atoms for inter-client communication.
 *  Color Server
 *  Dtwm
 *  Dtsession
 *
 *************************************<->***********************************/
void 
InitDtstyleProtocol( void )
  
{
  enum { XA_DT_SAVE_MODE, XA_WM_SAVE_YOURSELF, XA_WM_DELETE_WINDOW,
	 XA_DT_SM_STM_PROTOCOL, XA_DT_SM_SAVE_TO_HOME,
	 XA_DT_SM_STATE_CHANGE, XA_DT_SM_RESTORE_DEFAULT,
	 XA_DT_SM_LOCK_CHANGE, XA_DT_SM_AUDIO_INFO,
	 XA_DT_SM_SCREEN_INFO, XA_DT_SM_KEYBOARD_INFO,
	 XA_DT_SM_POINTER_INFO, XA_DT_SM_FONT_INFO,
	 XA_DT_SM_PREEDIT_INFO, NUM_ATOMS };
  static char *atom_names[] = {
	_XA_DT_SAVE_MODE, "WM_SAVE_YOURSELF", "WM_DELETE_WINDOW",
	_XA_DT_SM_STM_PROTOCOL, _XA_DT_SM_SAVE_TO_HOME,
	_XA_DT_SM_STATE_CHANGE, _XA_DT_SM_RESTORE_DEFAULT,
	_XA_DT_SM_LOCK_CHANGE, _XA_DT_SM_AUDIO_INFO,
	_XA_DT_SM_SCREEN_INFO, _XA_DT_SM_KEYBOARD_INFO,
	_XA_DT_SM_POINTER_INFO, _XA_DT_SM_FONT_INFO,
	_XA_DT_SM_PREEDIT_INFO };
  Atom atoms[XtNumber(atom_names)];

  /* Get Session Manager Window ID for communication */
  
  if (_DtGetSmWindow (style.display, 
		      XRootWindow(style.display,0),
		      &smWindow) == Success)
    {
      if (_DtGetSmState (style.display, smWindow, &style.smState) != Success)
	{
	  _DtSimpleError (progName, DtWarning, NULL, ERR1, NULL);
	  exit(1);
	}
      if (_DtGetSmSaver (style.display, smWindow, &style.smSaver) != Success)
	_DtSimpleError (progName, DtWarning, NULL, ERR2, NULL);
    }
  else smWindow = 0;
  
  
  XInternAtoms(style.display, atom_names, XtNumber(atom_names), False, atoms);

  xaDtSaveMode = atoms[XA_DT_SAVE_MODE];
  xaWmSaveYourself = atoms[XA_WM_SAVE_YOURSELF];
  xaWmDeleteWindow = atoms[XA_WM_DELETE_WINDOW];
  xaSmStmProtocol = atoms[XA_DT_SM_STM_PROTOCOL];
  xaSmSaveToHome = atoms[XA_DT_SM_SAVE_TO_HOME];
  xaSmStateChange = atoms[XA_DT_SM_STATE_CHANGE];
  xaSmRestoreDefault = atoms[XA_DT_SM_RESTORE_DEFAULT];
  xaSmLockChange = atoms[XA_DT_SM_LOCK_CHANGE];
  xaDtSmAudioInfo = atoms[XA_DT_SM_AUDIO_INFO];
  xaDtSmScreenInfo = atoms[XA_DT_SM_SCREEN_INFO];
  xaDtSmKeyboardInfo = atoms[XA_DT_SM_KEYBOARD_INFO];
  xaDtSmPointerInfo = atoms[XA_DT_SM_POINTER_INFO];
  xaDtSmFontInfo = atoms[XA_DT_SM_FONT_INFO];
  xaDtSmPreeditInfo = atoms[XA_DT_SM_PREEDIT_INFO];
  
}

/*************************************<->*************************************
 *
 *  SetWindowProperites ()
 *
 *
 *  Description:
 *  -----------
 *  Add the WM_DELETE_WINDOW and WM_SAVE_YOURSELF properties to the 
 *  dtstyle main window 
 *
 *************************************<->***********************************/
void 
SetWindowProperties( void )
{

    /* Add WM_SAVE_YOURSELF property to the main window */
    XmAddWMProtocolCallback(style.shell, xaWmSaveYourself, saveSessionCB, NULL);

    /* Add WM_DELETE_WINDOW property to the main window */
    XmAddWMProtocolCallback(style.shell, xaWmDeleteWindow, activateCB_exitBtn, NULL);

}


/*************************************<->*************************************
 *
 *  HandleWorkspaceChange
 *
 *
 *  Description:
 *  -----------
 *  Do processing required when workspace changes. A DtWsmWsChangeProc.
 *
 *************************************<->***********************************/
static void
HandleWorkspaceChange (
    Widget              widget,
    Atom                aWs,
    Pointer             client_data)
{
    CheckWorkspace ();	/* Backdrop may need to update colors */
}

/*************************************<->*************************************
 *
 *  ListenForWorkspaceChange ()
 *
 *
 *  Description:
 *  -----------
 *  
 *
 *************************************<->***********************************/
void 
ListenForWorkspaceChange( void )
{

  DtWsmAddCurrentWorkspaceCallback (style.shell, 
				    (DtWsmWsChangeProc) HandleWorkspaceChange, NULL);
  
}

/************************************************************************
 * WorkspaceStartupTimer()
 *
 * Check to see if the workspace manager is ready
 ************************************************************************/
static void 
WorkspaceStartupTimer( 
	XtPointer client_data,
	XtIntervalId *id)
{
    Atom	     aWS;
    intptr_t count = (intptr_t) client_data;

    if (DtWsmGetCurrentWorkspace (style.display, style.root, &aWS) 
	 	== Success)
    {
	/*
	 * OK, the workspace manager is ready. Get the
	 * colors and redraw the bitmap.
	 */
        CheckWorkspace ();
    }
    else if (--count > 0)
    {
	/* wait a little longer for the workspace manager */
	client_data = (XtPointer) count;
	(void) XtAppAddTimeOut (XtWidgetToApplicationContext(style.shell), 
		WS_STARTUP_TIME, WorkspaceStartupTimer, client_data);
    }
}

/*************************************<->*************************************
 *
 *  ReparentNotify ()
 *
 *
 *  Description:
 *  -----------
 *  Dtstyle has been reparented.  
 *  The parenting happens twice when the window manager has been restarted.
 *  First Dtstyle gets reparented to the root window, then reparented to
 *  the window manager.
 *
 *************************************<->***********************************/
void 
MwmReparentNotify(
        Widget w,
        XtPointer client_data,
        XEvent *event )
{
    if ((event->type == ReparentNotify) &&
        (event->xreparent.parent != style.root))
    {
	if (style.backdropDialog && XtIsManaged(style.backdropDialog))
	{
	    client_data = (XtPointer) WS_STARTUP_RETRY_COUNT;
	    (void) XtAppAddTimeOut (XtWidgetToApplicationContext(style.shell), 
		WS_STARTUP_TIME, WorkspaceStartupTimer, client_data);
	}
    }
}

/*************************************<->*************************************
 *
 *  GetSessionSaveMode ()
 *
 *
 *  Description:
 *  -----------
 *  Get the session save mode from the Session Manager
 *
 *************************************<->***********************************/
void 
GetSessionSaveMode( 
    unsigned char **mode ) 
{

    Atom actualType;
    int actualFormat;
    unsigned long nitems;
    unsigned long leftover;

    XGetWindowProperty(style.display, RootWindow(style.display, 0),
                         xaDtSaveMode,0L,
                         (long)BUFSIZ,False,AnyPropertyType,&actualType,
                         &actualFormat,&nitems,&leftover,
                         mode);

}


/*************************************<->*************************************
 *
 *  SmSaveHomeSession ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager to save the home session
 *
 *************************************<->***********************************/
void 
SmSaveHomeSession(
    int origStartState,
    int origConfirmMode)
{
  XClientMessageEvent stmToSmMessage;

  if (smWindow != 0)
  {
   /*
    * Tell session manager save home state using current smStartState
    * and smConfirmMode. Note that the session state will retain the
    * original smStartState and smConfirmMode values.
    */
    stmToSmMessage.type = ClientMessage;
    stmToSmMessage.window = smWindow;
    stmToSmMessage.message_type = xaSmStmProtocol;
    stmToSmMessage.format = 32;
    stmToSmMessage.data.l[0] = xaSmSaveToHome;
    stmToSmMessage.data.l[1] = style.smState.smStartState;
    stmToSmMessage.data.l[2] = style.smState.smConfirmMode;
    stmToSmMessage.data.l[3] = CurrentTime;
    XSendEvent(style.display, smWindow, False, NoEventMask,
                          (XEvent *) &stmToSmMessage);
  }
}                                                                 

/*************************************<->*************************************
 *
 *  SmNewStartupSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new Startup settings
 *
 *************************************<->***********************************/
void 
SmNewStartupSettings( void )
{
  SmStateInfo state;

  if (smWindow != 0)
  {
    state.flags = SM_STATE_START | SM_STATE_CONFIRM;
    state.smStartState = style.smState.smStartState;
    state.smConfirmMode = style.smState.smConfirmMode;
    _DtSetSmState(style.display, smWindow, &state);
  }
}                                                                 

/*************************************<->*************************************
 *
 *  SmRestoreDefault ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager to restore the default value to one of the settings
 *
 *************************************<->***********************************/
static void 
SmRestoreDefault(Atom toRestore)
{
     XClientMessageEvent stmToSmMessage;

    if (smWindow != 0)
    {
         stmToSmMessage.type = ClientMessage;
         stmToSmMessage.window = smWindow;
         stmToSmMessage.message_type = xaSmStmProtocol;
         stmToSmMessage.format = 32;
         stmToSmMessage.data.l[0] = xaSmRestoreDefault;
         stmToSmMessage.data.l[1] = toRestore;
         stmToSmMessage.data.l[2] = CurrentTime;
         XSendEvent(style.display, smWindow, False, NoEventMask,
                          (XEvent *) &stmToSmMessage);
    }
}                                                                 


/*************************************<->*************************************
 *
 *  SmDefaultAudioSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager to set default Xserver audio settings
 *
 *************************************<->***********************************/
void 
SmDefaultAudioSettings( void )
{
    if (smWindow != 0)
    {
        /*  Delete the property to indicate default settings to SM */
        SmRestoreDefault(xaDtSmAudioInfo);
    }
}                                                                 

/*************************************<->*************************************
 *
 *  SmNewAudioSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new Xserver audio settings
 *
 *************************************<->***********************************/
void 
SmNewAudioSettings(
    int volume,
    int tone,
    int duration )
{

    PropDtSmAudioInfo	audioProp;

    if (smWindow != 0)
    {
        /*
         * Set the property on the Session Manager window
         * indicating the new screen saver settings
         */

        audioProp.flags = 0;
        audioProp.smBellPercent = (CARD32) volume;
        audioProp.smBellPitch = (CARD32) tone;
        audioProp.smBellDuration = (CARD32) duration;
        XChangeProperty (style.display, smWindow,
                         xaDtSmAudioInfo, 
                         xaDtSmAudioInfo,
                         32, PropModeReplace, 
                         (unsigned char *)&audioProp,
                         PROP_DT_SM_AUDIO_INFO_ELEMENTS);
    }
}                                                                 

/*************************************<->*************************************
 *
 *  SmDefaultScreenSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager to set default Xserver screen settings
 *
 *************************************<->***********************************/
void 
SmDefaultScreenSettings( void )
{
    if (smWindow != 0)
    {
        /*  Delete the property to indicate default settings to SM */
        SmRestoreDefault(xaDtSmScreenInfo);
    }
}                                                                 


/*************************************<->*************************************
 *
 *  SmNewScreenSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new Xserver screen settings
 *
 *************************************<->***********************************/
void 
SmNewScreenSettings( 
    int timeout,
    int blanking,
    int interval,
    int exposures )

{

    PropDtSmScreenInfo	screenProp;

    if (smWindow != 0)
    {
        /*
         * Set the property on the Session Manager window
         * indicating the new screen saver settings
         */

        screenProp.flags = 0;
        screenProp.smTimeout = (CARD32) timeout;  /* 0-7200 */
        screenProp.smInterval = (CARD32) interval;    /* -1  */
        screenProp.smPreferBlank = (CARD32) blanking; /* 0,1 */
        screenProp.smAllowExp = (CARD32) exposures;
        XChangeProperty (style.display, smWindow,
                         xaDtSmScreenInfo, 
                         xaDtSmScreenInfo,
                         32, PropModeReplace, 
                         (unsigned char *)&screenProp,
                         PROP_DT_SM_SCREEN_INFO_ELEMENTS);
    }
}                                                                 

/*************************************<->*************************************
 *
 *  SmNewSaverSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new saver settings: 
 *  saver timeout, lock timeout, cycle timeout, selected saver list
 *
 *************************************<->***********************************/
void 
SmNewSaverSettings( 
    int saverTime,
    int lockTime,
    int cycleTime,		   
    char *selsaversList)
{
  SmStateInfo state;     /* structure that will contain new state info */
  SmSaverInfo saver;     /* structure that will contain selected saver list */

  int saverTime_change;  /* saver timeout */
  int lockTime_change;   /* lock timeout */
  int cycleTime_change;  /* saver timeout */

  if (smWindow != 0)
    {
      lockTime_change = (style.smState.smLockTimeout != lockTime);
      saverTime_change = (style.smState.smSaverTimeout != saverTime);
      cycleTime_change = (style.smState.smCycleTimeout != cycleTime);
      state.flags = (saverTime_change ? SM_STATE_SAVERTIMEOUT : 0) | 
                    (lockTime_change ?  SM_STATE_LOCKTIMEOUT  : 0) |
		    (cycleTime_change ? SM_STATE_CYCLETIMEOUT : 0);
      
      if (state.flags)
	{
	  state.smSaverTimeout = saverTime;
	  state.smLockTimeout = lockTime;
	  state.smCycleTimeout = cycleTime;
	  
	  _DtSetSmState(style.display, smWindow, &state);
	}
      saver.saverList = selsaversList;
      _DtSetSmSaver(style.display, smWindow, &saver);
    }
  
}


/*************************************<->*************************************
 *
 *  SmNewSaverTime()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new saver timeout
 *  used for telling the session manager if it should run savers or not when
 *  the user enables/disables saver toggle (or the savers toggle in no saver
 *  extension mode) without having to press OK. A zero is sent to the session 
 *  manager when no savers should be run.  
 *************************************<->***********************************/
void 
SmNewSaverTime(int saverTime)
{
  SmStateInfo state;     /* structure that will contain new state info */

  if (smWindow != 0)
    {
      state.flags = SM_STATE_SAVERTIMEOUT;
      state.smSaverTimeout = saverTime;
      _DtSetSmState(style.display, smWindow, &state);
    }
}



/*************************************<->*************************************
 *
 *  SmDefaultKeyboardSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager to set default Xserver Keyboard settings
 *
 *************************************<->***********************************/
void 
SmDefaultKeyboardSettings( void )
{
    if (smWindow != 0)
    {
        /*  Delete the property to indicate default settings to SM */
        SmRestoreDefault(xaDtSmKeyboardInfo);
    }
}                                                                 

/*************************************<->*************************************
 *
 *  SmNewKeyboardSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new Xserver Keyboard settings
 *
 *************************************<->***********************************/
void 
SmNewKeyboardSettings(
    int keyClickPercent,
    int  autoRepeat)
{

    PropDtSmKeyboardInfo	KeyboardProp;

    if (smWindow != 0)
    {
        /*
         * Set the property on the Session Manager window
         * indicating the new screen saver settings
         */

        KeyboardProp.flags = 0;
        KeyboardProp.smKeyClickPercent = (CARD32) keyClickPercent;
        KeyboardProp.smGlobalAutoRepeat = (CARD32) autoRepeat;
        XChangeProperty (style.display, smWindow,
                         xaDtSmKeyboardInfo, 
                         xaDtSmKeyboardInfo,
                         32, PropModeReplace, 
                         (unsigned char *)&KeyboardProp,
                         PROP_DT_SM_KEYBOARD_INFO_ELEMENTS);
    }
}                                                                 

/*************************************<->*************************************
 *
 *  SmDefaultPointerSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager to set default Xserver Pointer settings
 *
 *************************************<->***********************************/
void 
SmDefaultPointerSettings( void )
{
    if (smWindow != 0)
    {
        /*  Delete the property to indicate default settings to SM */
        SmRestoreDefault(xaDtSmPointerInfo);
    }
}                                                                 

/*************************************<->*************************************
 *
 *  SmNewPointerSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new Xserver Pointer settings
 *
 *************************************<->***********************************/
void 
SmNewPointerSettings(
    char *pointerString)
{

    if (smWindow != 0)
    {
        /*
         * Set the property on the Session Manager window
         * indicating the new screen saver settings
         */

        XChangeProperty (style.display, smWindow,
                         xaDtSmPointerInfo, 
                         XA_STRING,
                         8, PropModeReplace, 
                         (unsigned char *)pointerString,
                         strlen(pointerString));
    }
}                                                                 

/*************************************<->*************************************
 *
 *  SmNewPreeditSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new preeditType resources
 *
 *************************************<->***********************************/
void 
SmNewPreeditSettings(
    char *preeditResourceString)
{

                     
    if (smWindow != 0)
    {
        /*
         * Set the property on the Session Manager window
         * indicating the new font resource string
         */

        XChangeProperty (style.display, smWindow,
                         xaDtSmPreeditInfo, 
                         XA_STRING,
                         8, PropModeReplace, 
                         (unsigned char *)preeditResourceString,
                         strlen(preeditResourceString));
    }
}                                                                 
/*************************************<->*************************************
 *
 *  SmNewFontSettings ()
 *
 *  Description:
 *  -----------
 *  Tell Session Manager about new font resources
 *
 *************************************<->***********************************/
void
SmNewFontSettings(
    char *fontResourceString)
{

                    
    if (smWindow != 0)
    {
        /*
         * Set the property on the Session Manager window
         * indicating the new font resource string
         */

        XChangeProperty (style.display, smWindow,
                         xaDtSmFontInfo,
                         XA_STRING,
                         8, PropModeReplace,
                         (unsigned char *)fontResourceString,
                         strlen(fontResourceString));
    }
}

/*************************************************************************
 * contains_newline
 *
 * Returns non-zero if the given NUL-terminated string contains an embedded
 * '\n' or '\r'.  Used to reject font strings that would inject arbitrary
 * X resources when interpolated into an Xrm database string (newline is
 * a resource separator in Xrm).
 *
 * NULL is treated as "no newlines".
 *************************************************************************/
static int
contains_newline(const char *s)
{
    if (s == NULL) return 0;
    return (strchr(s, '\n') != NULL || strchr(s, '\r') != NULL);
}

/*************************************************************************
 * BuildFontResourceString
 *
 * Build the font resource string from current font settings.
 * This extracts the fontres construction from Font.c's ButtonCB OK case
 * into a reusable function for the system-wide apply path.
 *
 * Returns: Newly allocated string (caller must XtFree), or NULL on error
 *************************************************************************/
char *
BuildFontResourceString(void)
{
    char fontres[8192];
    int selectedFamily;
    int selectedSize;
    int selectedIndex;
    Boolean hasCustom;
    const char *customSys;
    const char *customUser;
    const char *sysStr, *userStr;
    int len;
    char *fntstr, *fntsetstr;
    char *result;

    selectedIndex = FontDataGetSelectedIndex();
    if (selectedIndex < 0 || selectedIndex >= FONT_INDEX(MAX_FONT_FAMILIES, MAX_FONT_SIZES))
        return NULL;

    selectedFamily = FONT_FAMILY(selectedIndex);
    selectedSize = FONT_SIZE(selectedIndex);

    hasCustom = FontDataHasCustomFont();
    customSys = FontDataGetCustomSysStr();
    customUser = FontDataGetCustomUserStr();

    sysStr = (hasCustom && customSys) ? customSys :
             style.xrdb.fontChoice[selectedIndex].sysStr;
    userStr = (hasCustom && customUser) ? customUser :
              style.xrdb.fontChoice[selectedIndex].userStr;

    if (!sysStr || !userStr)
        return NULL;

    /* Reject font strings containing newlines — they would inject
     * arbitrary X resources into the Xrm database string we build below. */
    if (contains_newline(sysStr) || contains_newline(userStr) ||
        contains_newline(customSys) || contains_newline(customUser))
        return NULL;

    /* FontSet: find first font entry delimited by ":" or "=" */
    len = strcspn(userStr, ":=");
    fntsetstr = (char *)XtCalloc(1, len + 1);
    memcpy(fntsetstr, userStr, len);

    /* Convert semicolons to commas for old X apps */
    {
        char *p = strstr(fntsetstr, ";");
        while (p) { *p = ','; p = strstr(p + 1, ";"); }
    }

    /* Font: find first font entry delimited by comma, colon, or equals */
    len = strcspn(fntsetstr, ",:=");
    fntstr = (char *)XtCalloc(1, len + 1);
    memcpy(fntstr, fntsetstr, len);

    /* Build the resource string */
    if (hasCustom) {
        len = snprintf(fontres, sizeof(fontres),
            "*systemFont: %s\n*userFont: %s\n*FontList: %s\n"
            "*buttonFontList: %s\n*labelFontList: %s\n*textFontList: %s\n"
            "*XmText*FontList: %s\n*XmTextField*FontList: %s\n"
            "*DtEditor*textFontList: %s\n*Font: %s\n*FontSet: %s\n"
            "*FontFamily: %d\n*CustomSysFont: %s\n*CustomUserFont: %s\n"
            "*CustomFamily: %d\n*CustomSize: %d\n",
            sysStr, userStr, sysStr, sysStr, sysStr, userStr,
            userStr, userStr, userStr, fntstr, fntsetstr,
            selectedFamily,
            customSys ? customSys : "",
            customUser ? customUser : "",
            selectedFamily, selectedSize);
        if (len < 0 || len >= sizeof(fontres)) {
            XtFree(fntstr);
            XtFree(fntsetstr);
            return NULL;
        }
    } else {
        len = snprintf(fontres, sizeof(fontres),
            "*systemFont: %s\n*userFont: %s\n*FontList: %s\n"
            "*buttonFontList: %s\n*labelFontList: %s\n*textFontList: %s\n"
            "*XmText*FontList: %s\n*XmTextField*FontList: %s\n"
            "*DtEditor*textFontList: %s\n*Font: %s\n*FontSet: %s\n"
            "*FontFamily: %d\n",
            sysStr, userStr, sysStr, sysStr, sysStr, userStr,
            userStr, userStr, userStr, fntstr, fntsetstr,
            selectedFamily);
        if (len < 0 || len >= sizeof(fontres)) {
            XtFree(fntstr);
            XtFree(fntsetstr);
            return NULL;
        }
    }

    XtFree(fntstr);
    XtFree(fntsetstr);

    result = XtNewString(fontres);
    return result;
}

/*************************************************************************
 * RequestSystemWideApply
 *
 * Write the current font resource string to a user-writable temp file,
 * then invoke pkexec dtstyle_sysapply to merge it into
 * /etc/dt/app-defaults/Dtstyle as root.
 *
 * Returns: 0 on success (pkexec launched or direct write succeeded),
 *         -1 on failure
 *************************************************************************/
int
RequestSystemWideApply(char *fontResourceString)
{
    char tmppath[PATH_MAX];
    int fd;
    FILE *fp;
    int ret;
    pid_t pid;
    int status;

    if (fontResourceString == NULL)
        return -1;

    /* Write resource string to a user-writable temp file */
    snprintf(tmppath, sizeof(tmppath), "/tmp/dtstyle-sysfont-XXXXXX");
    fd = mkstemp(tmppath);
    if (fd < 0)
        return -1;

    fp = fdopen(fd, "w");
    if (!fp) {
        close(fd);
        unlink(tmppath);
        return -1;
    }

    fputs(fontResourceString, fp);
    fclose(fp);

    /* Check if pkexec is available */
    if (access(PKEXECPATH, X_OK) != 0) {
        /* No pkexec - try direct write (will work only if user is root) */
        ret = SystemApplyFontDirect(fontResourceString);
        unlink(tmppath);
        return ret;
    }

    /* Invoke pkexec helper to merge the temp file into the system
     * app-defaults as root.  Use fork+exec to avoid invoking a shell
     * (CWE-78: OS command injection via system()). */
    pid = fork();
    if (pid < 0) {
        unlink(tmppath);
        return -1;
    }
    if (pid == 0) {
        /* Child: exec pkexec with the helper script */
        execl(PKEXECPATH, "pkexec", "--disable-internal-agent",
              CDE_INSTALLATION_TOP "/bin/dtstyle_sysapply.sh",
              tmppath, (char *)NULL);
        _exit(127);  /* exec failed */
    }
    /* Parent: wait for child */
    while (waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR) {
            unlink(tmppath);
            return -1;
        }
    }
    unlink(tmppath);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        return 0;
    return -1;
}

/*************************************************************************
 * SystemApplyFontDirect (static)
 *
 * Direct write to <CDE_CONFIGURATION_TOP>/app-defaults/Dtstyle
 * (root only).  Uses XrmGetFileDatabase + XrmMergeDatabases +
 * XrmPutFileDatabase for a proper Xrm merge (G14: not raw cat).
 *
 * Returns: 0 on success, -1 on failure.
 *************************************************************************/
static int
SystemApplyFontDirect(char *fontResourceString)
{
    char filepath[PATH_MAX];
    char tmppath[PATH_MAX];
    XrmDatabase db;
    XrmDatabase newdb;
    int fd;
    mode_t old_umask;

    if (fontResourceString == NULL)
        return -1;

    snprintf(filepath, sizeof(filepath), "%s/app-defaults/Dtstyle",
             CDE_CONFIGURATION_TOP);

    /* Check write permission on the target file.  ENOENT is acceptable
     * because the XrmGetFileDatabase call below will create an empty
     * database in that case. */
    if (access(filepath, W_OK) != 0 && errno != ENOENT) {
        return -1;
    }

    /* Parse existing file + merge new resources (G14: Xrm merge, not cat) */
    db = XrmGetFileDatabase(filepath);
    newdb = XrmGetStringDatabase(fontResourceString);
    if (newdb == NULL) {
        if (db) XrmDestroyDatabase(db);
        return -1;
    }
    if (db == NULL) {
        db = newdb;
    } else {
        XrmMergeDatabases(newdb, &db);
    }

    /* Write to temp file first (atomic via rename) */
    snprintf(tmppath, sizeof(tmppath), "%s/app-defaults/Dtstyle.XXXXXX",
             CDE_CONFIGURATION_TOP);
    old_umask = umask(0077);
    fd = mkstemp(tmppath);
    umask(old_umask);
    if (fd < 0) {
        XrmDestroyDatabase(db);
        return -1;
    }
    close(fd);

    XrmPutFileDatabase(db, tmppath);
    XrmDestroyDatabase(db);

    if (rename(tmppath, filepath) != 0) {
        unlink(tmppath);
        return -1;
    }

    return 0;
}

