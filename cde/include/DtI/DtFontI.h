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
/* $TOG: DtFontI.h /main/1 2025/06/14 cde-xft-migration $ */
/*
 *  (c) Copyright 2025 Hewlett-Packard Company
 *  (c) Copyright 2025 International Business Machines Corp.
 *  (c) Copyright 2025 Sun Microsystems, Inc.
 *  (c) Copyright 2025 Novell, Inc.
 */

#ifndef _DtFontI_H
#define _DtFontI_H

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Dt/DtFont.h>

/* USE_XFT is controlled by configure, not by Motif's Xm.h */
#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#include <fontconfig/fontconfig.h>
#endif

/*
 * _DtFont — internal representation behind the opaque DtFont handle.
 *
 * The 'use_xft' field determines which union member is active:
 *   use_xft == True  → font.xft_font and fset.xft_unused are valid
 *   use_xft == False → font.xfs and fset.fontset are valid (core X fonts)
 */
struct _DtFont {
    Display     *display;
    int          screen;
    Boolean      use_xft;        /* True if using Xft, False for core fonts */
#ifdef USE_XFT
    XftFont     *xft_font;       /* Xft font handle (USE_XFT path) */
#endif
    XFontStruct *xfs;            /* core X font struct */
    XFontSet     fontset;        /* core X font set (may be NULL) */
};

/*
 * Internal accessor functions (implemented in XftWrapper.c)
 */
extern DtFont _DtFontPrivateCreate(
        Display         *dpy,
        int              screen,
        const char      *pattern,
        Boolean          force_xft);

#ifdef USE_XFT
extern XftFont *__DtFontGetXftFont(DtFont font);
#endif

extern XFontStruct *__DtFontGetXFontStruct(DtFont font);
extern XFontSet __DtFontGetXFontSet(DtFont font);
extern Boolean _DtFontIsXft(DtFont font);

#endif /* _DtFontI_H */