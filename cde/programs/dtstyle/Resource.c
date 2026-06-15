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
      XtOffset(ApplicationDataPtr, numFamilies), XmRImmediate, (caddr_t) 2
  },
};

XtResource familyNames_resources[] = {
  {"fontFamily0", "FontFamily0", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[0]), XmRString, "system" },
  {"fontFamily1", "FontFamily1", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[1]), XmRString, "user" },
  {"fontFamily2", "FontFamily2", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[2]), XmRString, "" },
  {"fontFamily3", "FontFamily3", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyNames[3]), XmRString, "" },
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
      XtOffset(ApplicationDataPtr, familyLabels[2]), XmRString, "" },
  {"fontFamily3Label", "FontFamily3Label", XmRString, sizeof (String),
      XtOffset(ApplicationDataPtr, familyLabels[3]), XmRString, "" },
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
 * GetFamily0FontResources
 *
 *  Description:
 *  -----------
 *  This function retrieves the Dtstyle Font Family 0 font resources
 ************************************************************************/
void
GetFamily0FontResources(void)
{
    /* sysStr/userStr already loaded by GetFontStrResources for Family 0 — skip to avoid overwriting */
    XtGetApplicationResources(style.shell, &style.xrdb, family0SysFont_resources,
                              XtNumber(family0SysFont_resources), NULL, 0);
    XtGetApplicationResources(style.shell, &style.xrdb, family0UserFont_resources,
                              XtNumber(family0UserFont_resources), NULL, 0);
}

/************************************************************************
 * GetFamily1FontResources
 *
 *  Description:
 *  -----------
 *  This function retrieves the Dtstyle Font Family 1 font resources
 ************************************************************************/
void
GetFamily1FontResources(void)
{
    XtGetApplicationResources(style.shell, &style.xrdb, family1SysStr_resources,
                              XtNumber(family1SysStr_resources), NULL, 0);
    XtGetApplicationResources(style.shell, &style.xrdb, family1SysFont_resources,
                              XtNumber(family1SysFont_resources), NULL, 0);
    XtGetApplicationResources(style.shell, &style.xrdb, family1UserStr_resources,
                              XtNumber(family1UserStr_resources), NULL, 0);
    XtGetApplicationResources(style.shell, &style.xrdb, family1UserFont_resources,
                              XtNumber(family1UserFont_resources), NULL, 0);
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
    XtGetApplicationResources(style.shell, &style.xrdb, resources,
                              XtNumber(resources), NULL, 0);
    GetFontStrResources();
    GetFamilyNamesResources();
    GetFamily0FontResources();
    GetFamily1FontResources();
}



