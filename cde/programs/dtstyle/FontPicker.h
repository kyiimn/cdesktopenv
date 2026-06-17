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
/* $TOG: FontPicker.h /main/1 2025/06/17 cde-font-picker $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        FontPicker.h
 **
 **   Project:     DT 3.0
 **
 **   Description: Font picker dialog data structures and prototypes.
 **                The picker is a separate dialog (PopupFontPicker) that
 **                enumerates available font families/sizes via the
 **                Dt/FontEnum API and lets the user apply a font to the
 **                current slot (Apply) or system-wide (Apply System-Wide).
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _FontPicker_H
#define _FontPicker_H

#include <X11/Xlib.h>
#include "Main.h"          /* brings in Xm.h + Style */

/*
 * USE_XFT save/undef/restore dance. Motif 2.3+ defines USE_XFT in
 * Xm.h unconditionally; we need the configure-driven state. Since
 * "Main.h" already includes Xm.h above, we must re-sync here.
 */
#ifdef USE_XFT
#define _CDE_CONFIG_USE_XFT_PICKER 1
#endif

#ifdef USE_XFT
#undef USE_XFT
#endif

#ifdef _CDE_CONFIG_USE_XFT_PICKER
#define USE_XFT 1
#undef _CDE_CONFIG_USE_XFT_PICKER
#endif

/*
 * Dt/FontEnum.h is a standalone header (no circular dep risk). It
 * provides:
 *   - DtFontList  (with DtFontInfo members)
 *   - DtFontEnumSource enum (DtFontEnumCoreX11 / DtFontEnumFontconfig /
 *     DtFontEnumAll)
 *   - DtEnumerateFontFamilies / DtEnumerateFontSizes / DtFreeFontList /
 *     DtGetFontXmFontList prototypes
 * It forward-declares XmFontList so it does not force an Xm include.
 */
#include <Dt/FontEnum.h>

typedef struct {
    Widget       pickerDialog;      /* Shell for the picker */
    Widget       familyList;        /* ScrolledList of font families */
    Widget       sizeList;          /* ScrolledList of available sizes */
    Widget       previewLabel;      /* Label showing sample text in selected font */
    Widget       previewText;       /* TextField showing sample text in selected font */
    Widget       sourceOption;      /* OptionMenu: Core X11 / Fontconfig / All */
    Widget       applyButton;       /* Apply to current slot */
    Widget       systemWideButton;  /* Apply system-wide (root) */
    Widget       cancelButton;

    DtFontList  *availableFonts;    /* Current enumeration result */
    char        *selectedFamily;    /* Currently selected family name */
    char        *selectedXlfd;      /* Currently selected XLFD (or NULL) */
    char        *selectedFcPattern; /* Currently selected fc pattern (USE_XFT, or NULL) */
    XmFontList   currentFontList;   /* Resolved XmFontList for preview (ref-counted) */
    int          selectedSize;      /* Currently selected pixel size */
    DtFontEnumSource currentSource; /* Current enumeration source */
    Boolean      pickerActive;      /* True while picker is shown */
} FontPickerData;

extern FontPickerData fontPicker;

/* Public functions */
extern void CreateFontPicker(Widget parent);
extern void PopupFontPicker(int familyIdx, int sizeIdx);
extern void PopdownFontPicker(void);
extern void FontPickerApply(void);
extern void FontPickerApplySystemWide(void);

#endif /* _FontPicker_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
