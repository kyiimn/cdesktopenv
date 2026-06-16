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
/* $XConsortium: Resource.c /main/6 1996/07/19 10:21:08 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Resource.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the Dtstyle resources
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#ifdef USE_XFT
#define _CDE_SAVED_USE_XFT 1
#endif
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#ifdef _CDE_SAVED_USE_XFT
#define USE_XFT 1
#undef _CDE_SAVED_USE_XFT
#endif

#include "Main.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Resource.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define DEF_FONT "Fixed"

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/opt/dt"
#endif
/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/


/*++++++++++++++++++++++++++++++++++++++*/
/* Application Resources                */
/*++++++++++++++++++++++++++++++++++++++*/

XtResource sysFont_resources[] = {

  {"systemFont1", "SystemFont1", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[0].sysFont), XmRString, 
      "-adobe-helvetica-medium-r-normal--10-*-iso8859-1"
  },
  {"systemFont2", "SystemFont2", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[1].sysFont), XmRString, 
      "-adobe-helvetica-medium-r-normal--12-*-iso8859-1"
  },
  {"systemFont3", "SystemFont3", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[2].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--14-*-iso8859-1"
  },
  {"systemFont4", "SystemFont4", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[3].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--17-*-iso8859-1"
  },
  {"systemFont5", "SystemFont5", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[4].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--18-*-iso8859-1"
  },
  {"systemFont6", "SystemFont6", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[5].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--20-*-iso8859-1"
  },
  {"systemFont7", "SystemFont7", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[6].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--24-*-iso8859-1"
  },
};

XtResource userFont_resources[] = {

  {"userFont1", "UserFont1", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[0].userFont), XmRString, 
      "-adobe-courier-medium-r-normal--10-*-iso8859-1"
  },
  {"userFont2", "UserFont2", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[1].userFont), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1"
  },
  {"userFont3", "UserFont3", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[2].userFont), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1"
  },
  {"userFont4", "UserFont4", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[3].userFont), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1"
  },
  {"userFont5", "UserFont5", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[4].userFont), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1"
  },
  {"userFont6", "UserFont6", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[5].userFont), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1"
  },
  {"userFont7", "UserFont7", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[6].userFont), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1"
  },
};

XtResource sysStr_resources[] = {

  {"systemFont1", "SystemFont1", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[0].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--10-*-iso8859-1"
  },
  {"systemFont2", "SystemFont2", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[1].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--12-*-iso8859-1"
  },
  {"systemFont3", "SystemFont3", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[2].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--14-*-iso8859-1"
  },
  {"systemFont4", "SystemFont4", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[3].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--17-*-iso8859-1"
  },
  {"systemFont5", "SystemFont5", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[4].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--18-*-iso8859-1"
  },
  {"systemFont6", "SystemFont6", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[5].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--20-*-iso8859-1"
  },
  {"systemFont7", "SystemFont7", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[6].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--24-*-iso8859-1"
  },
};

XtResource userStr_resources[] = {

  {"userFont1", "UserFont1", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[0].userStr), XmRString,
      "-adobe-courier-medium-r-normal--10-*-iso8859-1"
  },
  {"userFont2", "UserFont2", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[1].userStr), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1"
  },
  {"userFont3", "UserFont3", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[2].userStr), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1"
  },
  {"userFont4", "UserFont4", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[3].userStr), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1"
  },
  {"userFont5", "UserFont5", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[4].userStr), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1"
  },
  {"userFont6", "UserFont6", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[5].userStr), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1"
  },
  {"userFont7", "UserFont7", XmRString, sizeof (XmString),
      XtOffset(ApplicationDataPtr, fontChoice[6].userStr), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1"
  },
};

/*++++++++++++++++++++++++++++++++++++++*/
/* Font Family Selection Resources       */
/*++++++++++++++++++++++++++++++++++++++*/

XtResource numFamilies_resources[] = {
  {"numFontFamilies", "NumFontFamilies", XmRInt, sizeof (int),
      XtOffset(ApplicationDataPtr, numFamilies), XmRImmediate, (caddr_t) 4
  },
};

XtResource familyNames_resources[] = {
  {"fontFamily0", "FontFamily0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[0]), XmRString, "system" },
  {"fontFamily1", "FontFamily1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[1]), XmRString, "user" },
  {"fontFamily2", "FontFamily2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[2]), XmRString, "serif" },
  {"fontFamily3", "FontFamily3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[3]), XmRString, "monospace" },
  {"fontFamily4", "FontFamily4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[4]), XmRString, "" },
  {"fontFamily5", "FontFamily5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[5]), XmRString, "" },
  {"fontFamily6", "FontFamily6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[6]), XmRString, "" },
  {"fontFamily7", "FontFamily7", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[7]), XmRString, "" },
};

XtResource familyLabels_resources[] = {
  {"fontFamily0Label", "FontFamily0Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[0]), XmRString, "System" },
  {"fontFamily1Label", "FontFamily1Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[1]), XmRString, "User" },
  {"fontFamily2Label", "FontFamily2Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[2]), XmRString, "Serif" },
  {"fontFamily3Label", "FontFamily3Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[3]), XmRString, "Monospace" },
  {"fontFamily4Label", "FontFamily4Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[4]), XmRString, "" },
  {"fontFamily5Label", "FontFamily5Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[5]), XmRString, "" },
  {"fontFamily6Label", "FontFamily6Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[6]), XmRString, "" },
  {"fontFamily7Label", "FontFamily7Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[7]), XmRString, "" },
};

/* FontFamily0 System Font string resources - maps to existing SystemFont1-7 defaults */
XtResource family0SysStr_resources[] = {
  {"fontFamily0SystemFont0", "FontFamily0SystemFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,0)].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--10-*-iso8859-1" },
  {"fontFamily0SystemFont1", "FontFamily0SystemFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,1)].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily0SystemFont2", "FontFamily0SystemFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,2)].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--14-*-iso8859-1" },
  {"fontFamily0SystemFont3", "FontFamily0SystemFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,3)].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily0SystemFont4", "FontFamily0SystemFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,4)].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--18-*-iso8859-1" },
  {"fontFamily0SystemFont5", "FontFamily0SystemFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,5)].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--20-*-iso8859-1" },
  {"fontFamily0SystemFont6", "FontFamily0SystemFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,6)].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--24-*-iso8859-1" },
};

/* FontFamily0 System FontList resources */
XtResource family0SysFont_resources[] = {
  {"fontFamily0SystemFont0", "FontFamily0SystemFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,0)].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--10-*-iso8859-1" },
  {"fontFamily0SystemFont1", "FontFamily0SystemFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,1)].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily0SystemFont2", "FontFamily0SystemFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,2)].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--14-*-iso8859-1" },
  {"fontFamily0SystemFont3", "FontFamily0SystemFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,3)].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily0SystemFont4", "FontFamily0SystemFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,4)].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--18-*-iso8859-1" },
  {"fontFamily0SystemFont5", "FontFamily0SystemFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,5)].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--20-*-iso8859-1" },
  {"fontFamily0SystemFont6", "FontFamily0SystemFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,6)].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--24-*-iso8859-1" },
};

/* FontFamily0 User Font string resources - maps to existing UserFont1-7 defaults */
XtResource family0UserStr_resources[] = {
  {"fontFamily0UserFont0", "FontFamily0UserFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,0)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--10-*-iso8859-1" },
  {"fontFamily0UserFont1", "FontFamily0UserFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,1)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily0UserFont2", "FontFamily0UserFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,2)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1" },
  {"fontFamily0UserFont3", "FontFamily0UserFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,3)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily0UserFont4", "FontFamily0UserFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,4)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1" },
  {"fontFamily0UserFont5", "FontFamily0UserFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,5)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1" },
  {"fontFamily0UserFont6", "FontFamily0UserFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,6)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1" },
};

/* FontFamily0 User FontList resources */
XtResource family0UserFont_resources[] = {
  {"fontFamily0UserFont0", "FontFamily0UserFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,0)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--10-*-iso8859-1" },
  {"fontFamily0UserFont1", "FontFamily0UserFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,1)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily0UserFont2", "FontFamily0UserFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,2)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1" },
  {"fontFamily0UserFont3", "FontFamily0UserFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,3)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily0UserFont4", "FontFamily0UserFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,4)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1" },
  {"fontFamily0UserFont5", "FontFamily0UserFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,5)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1" },
  {"fontFamily0UserFont6", "FontFamily0UserFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(0,6)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1" },
};

/* FontFamily1 System Font string resources - user family defaults */
XtResource family1SysStr_resources[] = {
  {"fontFamily1SystemFont0", "FontFamily1SystemFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,0)].sysStr), XmRString,
      "-dt-interface user-medium-r-normal-xxs*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont1", "FontFamily1SystemFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,1)].sysStr), XmRString,
      "-dt-interface user-medium-r-normal-xs*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont2", "FontFamily1SystemFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,2)].sysStr), XmRString,
      "-dt-interface user-medium-r-normal-s*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont3", "FontFamily1SystemFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,3)].sysStr), XmRString,
      "-dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont4", "FontFamily1SystemFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,4)].sysStr), XmRString,
      "-dt-interface user-medium-r-normal-l*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont5", "FontFamily1SystemFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,5)].sysStr), XmRString,
      "-dt-interface user-medium-r-normal-xl*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont6", "FontFamily1SystemFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,6)].sysStr), XmRString,
      "-dt-interface user-medium-r-normal-xxl*-*-*-*-*-*-*-*-*:" },
};

/* FontFamily1 System FontList resources */
XtResource family1SysFont_resources[] = {
  {"fontFamily1SystemFont0", "FontFamily1SystemFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,0)].sysFont), XmRString,
      "-dt-interface user-medium-r-normal-xxs*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont1", "FontFamily1SystemFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,1)].sysFont), XmRString,
      "-dt-interface user-medium-r-normal-xs*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont2", "FontFamily1SystemFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,2)].sysFont), XmRString,
      "-dt-interface user-medium-r-normal-s*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont3", "FontFamily1SystemFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,3)].sysFont), XmRString,
      "-dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont4", "FontFamily1SystemFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,4)].sysFont), XmRString,
      "-dt-interface user-medium-r-normal-l*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont5", "FontFamily1SystemFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,5)].sysFont), XmRString,
      "-dt-interface user-medium-r-normal-xl*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1SystemFont6", "FontFamily1SystemFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,6)].sysFont), XmRString,
      "-dt-interface user-medium-r-normal-xxl*-*-*-*-*-*-*-*-*:" },
};

/* FontFamily1 User Font string resources - user family defaults */
XtResource family1UserStr_resources[] = {
  {"fontFamily1UserFont0", "FontFamily1UserFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,0)].userStr), XmRString,
      "-dt-interface user-medium-r-normal-xxs*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont1", "FontFamily1UserFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,1)].userStr), XmRString,
      "-dt-interface user-medium-r-normal-xs*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont2", "FontFamily1UserFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,2)].userStr), XmRString,
      "-dt-interface user-medium-r-normal-s*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont3", "FontFamily1UserFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,3)].userStr), XmRString,
      "-dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont4", "FontFamily1UserFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,4)].userStr), XmRString,
      "-dt-interface user-medium-r-normal-l*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont5", "FontFamily1UserFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,5)].userStr), XmRString,
      "-dt-interface user-medium-r-normal-xl*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont6", "FontFamily1UserFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,6)].userStr), XmRString,
      "-dt-interface user-medium-r-normal-xxl*-*-*-*-*-*-*-*-*:" },
};

/* FontFamily1 User FontList resources */
XtResource family1UserFont_resources[] = {
  {"fontFamily1UserFont0", "FontFamily1UserFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,0)].userFont), XmRString,
      "-dt-interface user-medium-r-normal-xxs*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont1", "FontFamily1UserFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,1)].userFont), XmRString,
      "-dt-interface user-medium-r-normal-xs*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont2", "FontFamily1UserFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,2)].userFont), XmRString,
      "-dt-interface user-medium-r-normal-s*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont3", "FontFamily1UserFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,3)].userFont), XmRString,
      "-dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont4", "FontFamily1UserFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,4)].userFont), XmRString,
      "-dt-interface user-medium-r-normal-l*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont5", "FontFamily1UserFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,5)].userFont), XmRString,
      "-dt-interface user-medium-r-normal-xl*-*-*-*-*-*-*-*-*:" },
  {"fontFamily1UserFont6", "FontFamily1UserFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(1,6)].userFont), XmRString,
      "-dt-interface user-medium-r-normal-xxl*-*-*-*-*-*-*-*-*:" },
};

/* FontFamily2-7 resource tables — defaults are empty strings.
 * Admin must configure them via xrdb if they want families 2-7 populated.
 * numFamilies defaults to 2, so these tables are not loaded by default. */

/* FontFamily2 System Font string resources - Serif (LucidaBright proportional) */
XtResource family2SysStr_resources[] = {
  {"fontFamily2SystemFont0", "FontFamily2SystemFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,0)].sysStr), XmRString,
      "-b&h-lucidabright-medium-r-normal--10-100-75-75-p-56-iso8859-1" },
  {"fontFamily2SystemFont1", "FontFamily2SystemFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,1)].sysStr), XmRString,
      "-b&h-lucidabright-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily2SystemFont2", "FontFamily2SystemFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,2)].sysStr), XmRString,
      "-b&h-lucidabright-medium-r-normal--14-140-75-75-p-80-iso8859-1" },
  {"fontFamily2SystemFont3", "FontFamily2SystemFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,3)].sysStr), XmRString,
      "-b&h-lucidabright-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily2SystemFont4", "FontFamily2SystemFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,4)].sysStr), XmRString,
      "-b&h-lucidabright-medium-r-normal--19-190-75-75-p-109-iso8859-1" },
  {"fontFamily2SystemFont5", "FontFamily2SystemFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,5)].sysStr), XmRString,
      "-b&h-lucidabright-medium-r-normal--20-140-100-100-p-114-iso8859-1" },
  {"fontFamily2SystemFont6", "FontFamily2SystemFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,6)].sysStr), XmRString,
      "-b&h-lucidabright-medium-r-normal--24-240-75-75-p-137-iso8859-1" },
};

/* FontFamily2 System FontList resources - Serif (LucidaBright proportional) */
XtResource family2SysFont_resources[] = {
  {"fontFamily2SystemFont0", "FontFamily2SystemFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,0)].sysFont), XmRString,
      "-b&h-lucidabright-medium-r-normal--10-100-75-75-p-56-iso8859-1" },
  {"fontFamily2SystemFont1", "FontFamily2SystemFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,1)].sysFont), XmRString,
      "-b&h-lucidabright-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily2SystemFont2", "FontFamily2SystemFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,2)].sysFont), XmRString,
      "-b&h-lucidabright-medium-r-normal--14-140-75-75-p-80-iso8859-1" },
  {"fontFamily2SystemFont3", "FontFamily2SystemFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,3)].sysFont), XmRString,
      "-b&h-lucidabright-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily2SystemFont4", "FontFamily2SystemFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,4)].sysFont), XmRString,
      "-b&h-lucidabright-medium-r-normal--19-190-75-75-p-109-iso8859-1" },
  {"fontFamily2SystemFont5", "FontFamily2SystemFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,5)].sysFont), XmRString,
      "-b&h-lucidabright-medium-r-normal--20-140-100-100-p-114-iso8859-1" },
  {"fontFamily2SystemFont6", "FontFamily2SystemFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,6)].sysFont), XmRString,
      "-b&h-lucidabright-medium-r-normal--24-240-75-75-p-137-iso8859-1" },
};

/* FontFamily2 User Font string resources - Serif user font (LucidaTypewriter) */
XtResource family2UserStr_resources[] = {
  {"fontFamily2UserFont0", "FontFamily2UserFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,0)].userStr), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-10-100-75-75-m-60-iso8859-1" },
  {"fontFamily2UserFont1", "FontFamily2UserFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,1)].userStr), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-11-80-100-100-m-70-iso8859-1" },
  {"fontFamily2UserFont2", "FontFamily2UserFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,2)].userStr), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-14-140-75-75-m-90-iso8859-1" },
  {"fontFamily2UserFont3", "FontFamily2UserFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,3)].userStr), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-17-120-100-100-m-100-iso8859-1" },
  {"fontFamily2UserFont4", "FontFamily2UserFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,4)].userStr), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-19-190-75-75-m-110-iso8859-1" },
  {"fontFamily2UserFont5", "FontFamily2UserFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,5)].userStr), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-20-140-100-100-m-120-iso8859-1" },
  {"fontFamily2UserFont6", "FontFamily2UserFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,6)].userStr), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-24-240-75-75-m-140-iso8859-1" },
};

/* FontFamily2 User FontList resources - Serif user font (LucidaTypewriter) */
XtResource family2UserFont_resources[] = {
  {"fontFamily2UserFont0", "FontFamily2UserFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,0)].userFont), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-10-100-75-75-m-60-iso8859-1" },
  {"fontFamily2UserFont1", "FontFamily2UserFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,1)].userFont), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-11-80-100-100-m-70-iso8859-1" },
  {"fontFamily2UserFont2", "FontFamily2UserFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,2)].userFont), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-14-140-75-75-m-90-iso8859-1" },
  {"fontFamily2UserFont3", "FontFamily2UserFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,3)].userFont), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-17-120-100-100-m-100-iso8859-1" },
  {"fontFamily2UserFont4", "FontFamily2UserFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,4)].userFont), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-19-190-75-75-m-110-iso8859-1" },
  {"fontFamily2UserFont5", "FontFamily2UserFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,5)].userFont), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-20-140-100-100-m-120-iso8859-1" },
  {"fontFamily2UserFont6", "FontFamily2UserFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(2,6)].userFont), XmRString,
      "-b&h-lucidatypewriter-medium-r-normal-sans-24-240-75-75-m-140-iso8859-1" },
};

/* FontFamily3 System Font string resources - Monospace (Courier fixed-width) */
XtResource family3SysStr_resources[] = {
  {"fontFamily3SystemFont0", "FontFamily3SystemFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,0)].sysStr), XmRString,
      "-adobe-courier-medium-r-normal--10-*-iso8859-1" },
  {"fontFamily3SystemFont1", "FontFamily3SystemFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,1)].sysStr), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily3SystemFont2", "FontFamily3SystemFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,2)].sysStr), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1" },
  {"fontFamily3SystemFont3", "FontFamily3SystemFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,3)].sysStr), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily3SystemFont4", "FontFamily3SystemFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,4)].sysStr), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1" },
  {"fontFamily3SystemFont5", "FontFamily3SystemFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,5)].sysStr), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1" },
  {"fontFamily3SystemFont6", "FontFamily3SystemFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,6)].sysStr), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1" },
};

/* FontFamily3 System FontList resources - Monospace (Courier fixed-width) */
XtResource family3SysFont_resources[] = {
  {"fontFamily3SystemFont0", "FontFamily3SystemFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,0)].sysFont), XmRString,
      "-adobe-courier-medium-r-normal--10-*-iso8859-1" },
  {"fontFamily3SystemFont1", "FontFamily3SystemFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,1)].sysFont), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily3SystemFont2", "FontFamily3SystemFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,2)].sysFont), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1" },
  {"fontFamily3SystemFont3", "FontFamily3SystemFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,3)].sysFont), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily3SystemFont4", "FontFamily3SystemFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,4)].sysFont), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1" },
  {"fontFamily3SystemFont5", "FontFamily3SystemFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,5)].sysFont), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1" },
  {"fontFamily3SystemFont6", "FontFamily3SystemFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,6)].sysFont), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1" },
};

/* FontFamily3 User Font string resources - Monospace (Courier fixed-width) */
XtResource family3UserStr_resources[] = {
  {"fontFamily3UserFont0", "FontFamily3UserFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,0)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--10-*-iso8859-1" },
  {"fontFamily3UserFont1", "FontFamily3UserFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,1)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily3UserFont2", "FontFamily3UserFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,2)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1" },
  {"fontFamily3UserFont3", "FontFamily3UserFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,3)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily3UserFont4", "FontFamily3UserFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,4)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1" },
  {"fontFamily3UserFont5", "FontFamily3UserFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,5)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1" },
  {"fontFamily3UserFont6", "FontFamily3UserFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,6)].userStr), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1" },
};

/* FontFamily3 User FontList resources - Monospace (Courier fixed-width) */
XtResource family3UserFont_resources[] = {
  {"fontFamily3UserFont0", "FontFamily3UserFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,0)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--10-*-iso8859-1" },
  {"fontFamily3UserFont1", "FontFamily3UserFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,1)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1" },
  {"fontFamily3UserFont2", "FontFamily3UserFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,2)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1" },
  {"fontFamily3UserFont3", "FontFamily3UserFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,3)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1" },
  {"fontFamily3UserFont4", "FontFamily3UserFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,4)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1" },
  {"fontFamily3UserFont5", "FontFamily3UserFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,5)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1" },
  {"fontFamily3UserFont6", "FontFamily3UserFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(3,6)].userFont), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1" },
};

/* FontFamily4 System Font string resources */
XtResource family4SysStr_resources[] = {
  {"fontFamily4SystemFont0", "FontFamily4SystemFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,0)].sysStr), XmRString, "" },
  {"fontFamily4SystemFont1", "FontFamily4SystemFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,1)].sysStr), XmRString, "" },
  {"fontFamily4SystemFont2", "FontFamily4SystemFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,2)].sysStr), XmRString, "" },
  {"fontFamily4SystemFont3", "FontFamily4SystemFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,3)].sysStr), XmRString, "" },
  {"fontFamily4SystemFont4", "FontFamily4SystemFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,4)].sysStr), XmRString, "" },
  {"fontFamily4SystemFont5", "FontFamily4SystemFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,5)].sysStr), XmRString, "" },
  {"fontFamily4SystemFont6", "FontFamily4SystemFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,6)].sysStr), XmRString, "" },
};

/* FontFamily4 System FontList resources */
XtResource family4SysFont_resources[] = {
  {"fontFamily4SystemFont0", "FontFamily4SystemFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,0)].sysFont), XmRString, "" },
  {"fontFamily4SystemFont1", "FontFamily4SystemFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,1)].sysFont), XmRString, "" },
  {"fontFamily4SystemFont2", "FontFamily4SystemFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,2)].sysFont), XmRString, "" },
  {"fontFamily4SystemFont3", "FontFamily4SystemFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,3)].sysFont), XmRString, "" },
  {"fontFamily4SystemFont4", "FontFamily4SystemFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,4)].sysFont), XmRString, "" },
  {"fontFamily4SystemFont5", "FontFamily4SystemFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,5)].sysFont), XmRString, "" },
  {"fontFamily4SystemFont6", "FontFamily4SystemFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,6)].sysFont), XmRString, "" },
};

/* FontFamily4 User Font string resources */
XtResource family4UserStr_resources[] = {
  {"fontFamily4UserFont0", "FontFamily4UserFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,0)].userStr), XmRString, "" },
  {"fontFamily4UserFont1", "FontFamily4UserFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,1)].userStr), XmRString, "" },
  {"fontFamily4UserFont2", "FontFamily4UserFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,2)].userStr), XmRString, "" },
  {"fontFamily4UserFont3", "FontFamily4UserFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,3)].userStr), XmRString, "" },
  {"fontFamily4UserFont4", "FontFamily4UserFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,4)].userStr), XmRString, "" },
  {"fontFamily4UserFont5", "FontFamily4UserFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,5)].userStr), XmRString, "" },
  {"fontFamily4UserFont6", "FontFamily4UserFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,6)].userStr), XmRString, "" },
};

/* FontFamily4 User FontList resources */
XtResource family4UserFont_resources[] = {
  {"fontFamily4UserFont0", "FontFamily4UserFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,0)].userFont), XmRString, "" },
  {"fontFamily4UserFont1", "FontFamily4UserFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,1)].userFont), XmRString, "" },
  {"fontFamily4UserFont2", "FontFamily4UserFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,2)].userFont), XmRString, "" },
  {"fontFamily4UserFont3", "FontFamily4UserFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,3)].userFont), XmRString, "" },
  {"fontFamily4UserFont4", "FontFamily4UserFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,4)].userFont), XmRString, "" },
  {"fontFamily4UserFont5", "FontFamily4UserFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,5)].userFont), XmRString, "" },
  {"fontFamily4UserFont6", "FontFamily4UserFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(4,6)].userFont), XmRString, "" },
};

/* FontFamily5 System Font string resources */
XtResource family5SysStr_resources[] = {
  {"fontFamily5SystemFont0", "FontFamily5SystemFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,0)].sysStr), XmRString, "" },
  {"fontFamily5SystemFont1", "FontFamily5SystemFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,1)].sysStr), XmRString, "" },
  {"fontFamily5SystemFont2", "FontFamily5SystemFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,2)].sysStr), XmRString, "" },
  {"fontFamily5SystemFont3", "FontFamily5SystemFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,3)].sysStr), XmRString, "" },
  {"fontFamily5SystemFont4", "FontFamily5SystemFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,4)].sysStr), XmRString, "" },
  {"fontFamily5SystemFont5", "FontFamily5SystemFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,5)].sysStr), XmRString, "" },
  {"fontFamily5SystemFont6", "FontFamily5SystemFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,6)].sysStr), XmRString, "" },
};

/* FontFamily5 System FontList resources */
XtResource family5SysFont_resources[] = {
  {"fontFamily5SystemFont0", "FontFamily5SystemFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,0)].sysFont), XmRString, "" },
  {"fontFamily5SystemFont1", "FontFamily5SystemFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,1)].sysFont), XmRString, "" },
  {"fontFamily5SystemFont2", "FontFamily5SystemFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,2)].sysFont), XmRString, "" },
  {"fontFamily5SystemFont3", "FontFamily5SystemFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,3)].sysFont), XmRString, "" },
  {"fontFamily5SystemFont4", "FontFamily5SystemFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,4)].sysFont), XmRString, "" },
  {"fontFamily5SystemFont5", "FontFamily5SystemFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,5)].sysFont), XmRString, "" },
  {"fontFamily5SystemFont6", "FontFamily5SystemFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,6)].sysFont), XmRString, "" },
};

/* FontFamily5 User Font string resources */
XtResource family5UserStr_resources[] = {
  {"fontFamily5UserFont0", "FontFamily5UserFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,0)].userStr), XmRString, "" },
  {"fontFamily5UserFont1", "FontFamily5UserFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,1)].userStr), XmRString, "" },
  {"fontFamily5UserFont2", "FontFamily5UserFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,2)].userStr), XmRString, "" },
  {"fontFamily5UserFont3", "FontFamily5UserFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,3)].userStr), XmRString, "" },
  {"fontFamily5UserFont4", "FontFamily5UserFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,4)].userStr), XmRString, "" },
  {"fontFamily5UserFont5", "FontFamily5UserFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,5)].userStr), XmRString, "" },
  {"fontFamily5UserFont6", "FontFamily5UserFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,6)].userStr), XmRString, "" },
};

/* FontFamily5 User FontList resources */
XtResource family5UserFont_resources[] = {
  {"fontFamily5UserFont0", "FontFamily5UserFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,0)].userFont), XmRString, "" },
  {"fontFamily5UserFont1", "FontFamily5UserFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,1)].userFont), XmRString, "" },
  {"fontFamily5UserFont2", "FontFamily5UserFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,2)].userFont), XmRString, "" },
  {"fontFamily5UserFont3", "FontFamily5UserFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,3)].userFont), XmRString, "" },
  {"fontFamily5UserFont4", "FontFamily5UserFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,4)].userFont), XmRString, "" },
  {"fontFamily5UserFont5", "FontFamily5UserFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,5)].userFont), XmRString, "" },
  {"fontFamily5UserFont6", "FontFamily5UserFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(5,6)].userFont), XmRString, "" },
};

/* FontFamily6 System Font string resources */
XtResource family6SysStr_resources[] = {
  {"fontFamily6SystemFont0", "FontFamily6SystemFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,0)].sysStr), XmRString, "" },
  {"fontFamily6SystemFont1", "FontFamily6SystemFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,1)].sysStr), XmRString, "" },
  {"fontFamily6SystemFont2", "FontFamily6SystemFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,2)].sysStr), XmRString, "" },
  {"fontFamily6SystemFont3", "FontFamily6SystemFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,3)].sysStr), XmRString, "" },
  {"fontFamily6SystemFont4", "FontFamily6SystemFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,4)].sysStr), XmRString, "" },
  {"fontFamily6SystemFont5", "FontFamily6SystemFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,5)].sysStr), XmRString, "" },
  {"fontFamily6SystemFont6", "FontFamily6SystemFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,6)].sysStr), XmRString, "" },
};

/* FontFamily6 System FontList resources */
XtResource family6SysFont_resources[] = {
  {"fontFamily6SystemFont0", "FontFamily6SystemFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,0)].sysFont), XmRString, "" },
  {"fontFamily6SystemFont1", "FontFamily6SystemFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,1)].sysFont), XmRString, "" },
  {"fontFamily6SystemFont2", "FontFamily6SystemFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,2)].sysFont), XmRString, "" },
  {"fontFamily6SystemFont3", "FontFamily6SystemFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,3)].sysFont), XmRString, "" },
  {"fontFamily6SystemFont4", "FontFamily6SystemFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,4)].sysFont), XmRString, "" },
  {"fontFamily6SystemFont5", "FontFamily6SystemFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,5)].sysFont), XmRString, "" },
  {"fontFamily6SystemFont6", "FontFamily6SystemFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,6)].sysFont), XmRString, "" },
};

/* FontFamily6 User Font string resources */
XtResource family6UserStr_resources[] = {
  {"fontFamily6UserFont0", "FontFamily6UserFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,0)].userStr), XmRString, "" },
  {"fontFamily6UserFont1", "FontFamily6UserFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,1)].userStr), XmRString, "" },
  {"fontFamily6UserFont2", "FontFamily6UserFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,2)].userStr), XmRString, "" },
  {"fontFamily6UserFont3", "FontFamily6UserFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,3)].userStr), XmRString, "" },
  {"fontFamily6UserFont4", "FontFamily6UserFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,4)].userStr), XmRString, "" },
  {"fontFamily6UserFont5", "FontFamily6UserFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,5)].userStr), XmRString, "" },
  {"fontFamily6UserFont6", "FontFamily6UserFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,6)].userStr), XmRString, "" },
};

/* FontFamily6 User FontList resources */
XtResource family6UserFont_resources[] = {
  {"fontFamily6UserFont0", "FontFamily6UserFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,0)].userFont), XmRString, "" },
  {"fontFamily6UserFont1", "FontFamily6UserFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,1)].userFont), XmRString, "" },
  {"fontFamily6UserFont2", "FontFamily6UserFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,2)].userFont), XmRString, "" },
  {"fontFamily6UserFont3", "FontFamily6UserFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,3)].userFont), XmRString, "" },
  {"fontFamily6UserFont4", "FontFamily6UserFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,4)].userFont), XmRString, "" },
  {"fontFamily6UserFont5", "FontFamily6UserFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,5)].userFont), XmRString, "" },
  {"fontFamily6UserFont6", "FontFamily6UserFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(6,6)].userFont), XmRString, "" },
};

/* FontFamily7 System Font string resources */
XtResource family7SysStr_resources[] = {
  {"fontFamily7SystemFont0", "FontFamily7SystemFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,0)].sysStr), XmRString, "" },
  {"fontFamily7SystemFont1", "FontFamily7SystemFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,1)].sysStr), XmRString, "" },
  {"fontFamily7SystemFont2", "FontFamily7SystemFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,2)].sysStr), XmRString, "" },
  {"fontFamily7SystemFont3", "FontFamily7SystemFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,3)].sysStr), XmRString, "" },
  {"fontFamily7SystemFont4", "FontFamily7SystemFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,4)].sysStr), XmRString, "" },
  {"fontFamily7SystemFont5", "FontFamily7SystemFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,5)].sysStr), XmRString, "" },
  {"fontFamily7SystemFont6", "FontFamily7SystemFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,6)].sysStr), XmRString, "" },
};

/* FontFamily7 System FontList resources */
XtResource family7SysFont_resources[] = {
  {"fontFamily7SystemFont0", "FontFamily7SystemFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,0)].sysFont), XmRString, "" },
  {"fontFamily7SystemFont1", "FontFamily7SystemFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,1)].sysFont), XmRString, "" },
  {"fontFamily7SystemFont2", "FontFamily7SystemFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,2)].sysFont), XmRString, "" },
  {"fontFamily7SystemFont3", "FontFamily7SystemFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,3)].sysFont), XmRString, "" },
  {"fontFamily7SystemFont4", "FontFamily7SystemFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,4)].sysFont), XmRString, "" },
  {"fontFamily7SystemFont5", "FontFamily7SystemFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,5)].sysFont), XmRString, "" },
  {"fontFamily7SystemFont6", "FontFamily7SystemFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,6)].sysFont), XmRString, "" },
};

/* FontFamily7 User Font string resources */
XtResource family7UserStr_resources[] = {
  {"fontFamily7UserFont0", "FontFamily7UserFont0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,0)].userStr), XmRString, "" },
  {"fontFamily7UserFont1", "FontFamily7UserFont1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,1)].userStr), XmRString, "" },
  {"fontFamily7UserFont2", "FontFamily7UserFont2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,2)].userStr), XmRString, "" },
  {"fontFamily7UserFont3", "FontFamily7UserFont3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,3)].userStr), XmRString, "" },
  {"fontFamily7UserFont4", "FontFamily7UserFont4", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,4)].userStr), XmRString, "" },
  {"fontFamily7UserFont5", "FontFamily7UserFont5", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,5)].userStr), XmRString, "" },
  {"fontFamily7UserFont6", "FontFamily7UserFont6", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,6)].userStr), XmRString, "" },
};

/* FontFamily7 User FontList resources */
XtResource family7UserFont_resources[] = {
  {"fontFamily7UserFont0", "FontFamily7UserFont0", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,0)].userFont), XmRString, "" },
  {"fontFamily7UserFont1", "FontFamily7UserFont1", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,1)].userFont), XmRString, "" },
  {"fontFamily7UserFont2", "FontFamily7UserFont2", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,2)].userFont), XmRString, "" },
  {"fontFamily7UserFont3", "FontFamily7UserFont3", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,3)].userFont), XmRString, "" },
  {"fontFamily7UserFont4", "FontFamily7UserFont4", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,4)].userFont), XmRString, "" },
  {"fontFamily7UserFont5", "FontFamily7UserFont5", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,5)].userFont), XmRString, "" },
  {"fontFamily7UserFont6", "FontFamily7UserFont6", XmRFontList, sizeof (XmFontList),
      XtOffset(ApplicationDataPtr, fontChoice[FONT_INDEX(7,6)].userFont), XmRString, "" },
};

XtResource resources[] = {

  {"numFonts", "NumFonts", XmRInt, sizeof (int), 
      XtOffset(ApplicationDataPtr, numFonts), XmRImmediate, (caddr_t) 7
  },
  {"systemFont", "SystemFont", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, systemFont), XmRString, "Fixed"
  },
  {"userFont", "UserFont", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, userFont), XmRString, "Fixed"
  },
  {"systemFont", "SystemFont", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, systemFontStr), XmRString, DEF_FONT
  },
  {"userFont", "UserFont", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, userFontStr), XmRString, DEF_FONT
  },

  {"session", "Session", XmRString, sizeof (char *),
      XtOffset (ApplicationDataPtr, session), XmRImmediate, (XtPointer)NULL,
  },
  {"backdropDirectories", "BackdropDirectories", XmRString, sizeof(char *),
        XtOffset(ApplicationDataPtr, backdropDir), XmRString, NULL 
  },
  {"paletteDirectories", "PaletteDirectories", XmRString, sizeof(char *),
        XtOffset(ApplicationDataPtr, paletteDir), XmRString, NULL
  },
  {"timeoutScale", "TimeoutScale", XmRString, sizeof (String),
        XtOffset(ApplicationDataPtr, timeoutScale), XmRString, "10" 
  },
  {"lockoutScale", "LockoutScale", XmRString, sizeof (String),
        XtOffset(ApplicationDataPtr, lockoutScale), XmRString, "30" 
  },
  {"writeXrdbImmediate", "WriteXrdbImmediate", XmRBoolean, sizeof(Boolean) ,
        XtOffset(ApplicationDataPtr, writeXrdbImmediate), XmRImmediate, (XtPointer)True
  },
  {"writeXrdbColors", "WriteXrdbColors", XmRBoolean, sizeof(Boolean) ,
        XtOffset(ApplicationDataPtr, writeXrdbColors), XmRImmediate, (XtPointer)True
  },
  {"componentList", "ComponentList", XtRString, sizeof(String) ,
        XtOffset(ApplicationDataPtr, componentList), XmRImmediate, 
        "Color Font Backdrop Keyboard Mouse Audio Screen Dtwm Startup"
  },
  {"imServerHosts", "ImServerHosts", XmRXmStringTable, sizeof(XmStringTable) ,
        XtOffset(ApplicationDataPtr, imServerHosts), XmRImmediate, 
        NULL
  },
  {"preeditType", "PreeditType", XmRXmStringTable, sizeof(XmStringTable) ,
        XtOffset(ApplicationDataPtr, preeditType), XmRString, 
        "OnTheSpot,OverTheSpot,OffTheSpot,Root"
  },
  {"pipeTimeOut", "PipeTimeOut", XmRInt, sizeof (int), 
      XtOffset(ApplicationDataPtr, pipeTimeOut), XmRImmediate, (caddr_t) 100
  },
};

/************************************************************************
 * GetSysFontResources
 *
 *  Description:
 *  -----------
 *  This function is used to retrieve the Dtstyle System Font resources 
 ************************************************************************/
void
GetSysFontResource(int i)
{
    if (i < 0 || i >= XtNumber(sysFont_resources))
        return;
    XtGetApplicationResources(style.shell, &style.xrdb,
                            &sysFont_resources[i],
                              1, NULL, 0);
}

/************************************************************************
 * GetUserFontResources
 *
 *  Description:
 *  -----------
 *  This function is used to retrieve the Dtstyle User Font resources 
 ************************************************************************/
void
GetUserFontResource(int i)
{
    if (i < 0 || i >= XtNumber(userFont_resources))
        return;
    XtGetApplicationResources(style.shell, &style.xrdb,
                            &userFont_resources[i],
                              1, NULL, 0);
}

/************************************************************************
 * GetFontStrResources
 *
 *  Description:
 *  -----------
 ************************************************************************/
static void
GetFontStrResources( void )
{
    int i;

    XtGetApplicationResources(style.shell, &style.xrdb, sysStr_resources,
                              XtNumber(sysStr_resources), NULL, 0);
    XtGetApplicationResources(style.shell, &style.xrdb, userStr_resources,
                              XtNumber(userStr_resources), NULL, 0);
    for (i=0; i<style.xrdb.numFonts; i++) {
      style.xrdb.fontChoice[i].userFont = NULL;
      style.xrdb.fontChoice[i].sysFont = NULL;
    }
}

/************************************************************************
 * GetFamilyNamesResources
 *
 *  Description:
 *  -----------
 *  This function retrieves the Dtstyle Font Family name/label resources
 ************************************************************************/
void
GetFamilyNamesResources(void)
{
    XtGetApplicationResources(style.shell, &style.xrdb, numFamilies_resources,
                              XtNumber(numFamilies_resources), NULL, 0);
    /* Clamp numFamilies to valid range [1, MAX_FONT_FAMILIES] */
    if (style.xrdb.numFamilies < 1)
        style.xrdb.numFamilies = 1;
    if (style.xrdb.numFamilies > MAX_FONT_FAMILIES)
        style.xrdb.numFamilies = MAX_FONT_FAMILIES;
    XtGetApplicationResources(style.shell, &style.xrdb, familyNames_resources,
                              XtNumber(familyNames_resources), NULL, 0);
    XtGetApplicationResources(style.shell, &style.xrdb, familyLabels_resources,
                              XtNumber(familyLabels_resources), NULL, 0);
}

/************************************************************************
 * GetFamilyFontResources
 *
 *  Description:
 *  -----------
 *  Generalized font resource loader for any family index (0-7).
 *  Family 0: sysStr/userStr already loaded by GetFontStrResources via
 *            legacy resource names — only load sysFont/userFont to
 *            avoid overwriting admin customizations (M2 bugfix).
 *  Family 1+: load all 4 resource groups (sysStr, sysFont, userStr, userFont).
 *  Family 2-7: load all 4 groups; defaults are empty strings.
 ************************************************************************/
void
GetFamilyFontResources(int fam)
{
    switch (fam) {
    case 0:
        XtGetApplicationResources(style.shell, &style.xrdb, family0SysFont_resources,
                                  XtNumber(family0SysFont_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family0UserFont_resources,
                                  XtNumber(family0UserFont_resources), NULL, 0);
        break;
    case 1:
        XtGetApplicationResources(style.shell, &style.xrdb, family1SysStr_resources,
                                  XtNumber(family1SysStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family1SysFont_resources,
                                  XtNumber(family1SysFont_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family1UserStr_resources,
                                  XtNumber(family1UserStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family1UserFont_resources,
                                  XtNumber(family1UserFont_resources), NULL, 0);
        break;
    case 2:
        XtGetApplicationResources(style.shell, &style.xrdb, family2SysStr_resources,
                                  XtNumber(family2SysStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family2SysFont_resources,
                                  XtNumber(family2SysFont_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family2UserStr_resources,
                                  XtNumber(family2UserStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family2UserFont_resources,
                                  XtNumber(family2UserFont_resources), NULL, 0);
        break;
    case 3:
        XtGetApplicationResources(style.shell, &style.xrdb, family3SysStr_resources,
                                  XtNumber(family3SysStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family3SysFont_resources,
                                  XtNumber(family3SysFont_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family3UserStr_resources,
                                  XtNumber(family3UserStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family3UserFont_resources,
                                  XtNumber(family3UserFont_resources), NULL, 0);
        break;
    case 4:
        XtGetApplicationResources(style.shell, &style.xrdb, family4SysStr_resources,
                                  XtNumber(family4SysStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family4SysFont_resources,
                                  XtNumber(family4SysFont_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family4UserStr_resources,
                                  XtNumber(family4UserStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family4UserFont_resources,
                                  XtNumber(family4UserFont_resources), NULL, 0);
        break;
    case 5:
        XtGetApplicationResources(style.shell, &style.xrdb, family5SysStr_resources,
                                  XtNumber(family5SysStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family5SysFont_resources,
                                  XtNumber(family5SysFont_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family5UserStr_resources,
                                  XtNumber(family5UserStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family5UserFont_resources,
                                  XtNumber(family5UserFont_resources), NULL, 0);
        break;
    case 6:
        XtGetApplicationResources(style.shell, &style.xrdb, family6SysStr_resources,
                                  XtNumber(family6SysStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family6SysFont_resources,
                                  XtNumber(family6SysFont_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family6UserStr_resources,
                                  XtNumber(family6UserStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family6UserFont_resources,
                                  XtNumber(family6UserFont_resources), NULL, 0);
        break;
    case 7:
        XtGetApplicationResources(style.shell, &style.xrdb, family7SysStr_resources,
                                  XtNumber(family7SysStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family7SysFont_resources,
                                  XtNumber(family7SysFont_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family7UserStr_resources,
                                  XtNumber(family7UserStr_resources), NULL, 0);
        XtGetApplicationResources(style.shell, &style.xrdb, family7UserFont_resources,
                                  XtNumber(family7UserFont_resources), NULL, 0);
        break;
    default:
        break;  /* Family index out of range — skip */
    }
}

/************************************************************************
 * GetApplicationResources
 *
 *  Description:
 *  -----------
 *  This function is used to retrieve Dtstyle resources that are 
 * not component-specific.
 ************************************************************************/
void
GetApplicationResources( void )
{
    int fam;

    XtGetApplicationResources(style.shell, &style.xrdb, resources,
                              XtNumber(resources), NULL, 0);
    GetFontStrResources();
    GetFamilyNamesResources();
    for (fam = 0; fam < style.xrdb.numFamilies; fam++) {
        GetFamilyFontResources(fam);
    }
}



