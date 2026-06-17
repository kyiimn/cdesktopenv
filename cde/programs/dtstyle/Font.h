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
/* $XConsortium: Font.h /main/4 1995/10/30 13:09:39 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Font.h
 **
 **   Project:     DT 3.0
 **
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _font_h
#define _font_h

/* typedef statements */

typedef struct {
  XmFontList sysFont;
  XmFontList userFont;
  String     sysStr;
  String     userStr;
  XmString   pointSize;
  /* Added for font family selection feature */
  String     familyName;   /* family identifier (e.g., "system", "user", "serif") */
  String     familyLabel;  /* display label for UI list (e.g., "System", "User", "Serif") */
} Fontset;

/* FontData holds dtstyle's font dialog state. The full struct
 * definition is in Font.c and is intentionally not exposed here.
 * Protocol.c uses the read-only accessors below to read the currently
 * selected font + custom-override strings when building the system-wide
 * font resource string. */

extern int          FontDataGetSelectedIndex(void);
extern Boolean      FontDataHasCustomFont(void);
extern String       FontDataGetCustomSysStr(void);
extern String       FontDataGetCustomUserStr(void);

/*
 * FontDataSetCustomFont
 *   Called by FontPicker.c (FontPickerApply) to install a custom font
 *   override on the currently selected slot. Frees any previous override
 *   strings + XmFontLists and replaces them with the given ones. Either
 *   argument may be NULL, meaning "no override for that role". Passing
 *   both NULL is equivalent to clearing the override (hasCustomFont
 *   becomes False and the picker's selection is forgotten).
 *
 *   This is a write accessor — it complements the read accessors above.
 *   Font.c owns the FontData struct; callers outside this file must use
 *   these helpers rather than touching the struct directly.
 */
extern void FontDataSetCustomFont(
    String sysStr,
    String userStr,
    int source);

/* External Interface */


extern void popup_fontBB( Widget shell) ;
extern void restoreFonts( Widget shell, XrmDatabase db) ;
extern void saveFonts( int fd) ;


#endif /* _font_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
