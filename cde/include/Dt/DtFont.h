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
/* $TOG: DtFont.h /main/1 2025/06/14 cde-xft-migration $ */
/*
 *  (c) Copyright 2025 Hewlett-Packard Company
 *  (c) Copyright 2025 International Business Machines Corp.
 *  (c) Copyright 2025 Sun Microsystems, Inc.
 *  (c) Copyright 2025 Novell, Inc.
 */

#ifndef _DtFont_H
#define _DtFont_H

#include <X11/Xlib.h>

/*
 * Motif 2.3+ defines USE_XFT in Xm.h. CDE needs to control this
 * macro itself based on configure --enable-xft, so we save the
 * configure-driven state, include Xm.h, undef Motif's definition,
 * then restore if configure enabled Xft.
 */
#ifdef USE_XFT
#define _CDE_CONFIG_USE_XFT 1
#endif

#include <Xm/Xm.h>

#ifdef USE_XFT
#undef USE_XFT
#endif

#ifdef _CDE_CONFIG_USE_XFT
#define USE_XFT 1
#undef _CDE_CONFIG_USE_XFT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Opaque font handle type
 */
typedef struct _DtFont *DtFont;

/*
 * Functions
 */

extern DtFont DtFontCreate(
		Display		*dpy,
		int		screen,
		const char	*pattern);

extern void DtFontDestroy(
		Display		*dpy,
		DtFont		font);

extern void DtFont_DrawString(
		Display		*dpy,
		Drawable	draw,
		GC		gc,
		DtFont		font,
		int		x,
		int		y,
		const char	*str,
		int		len);

extern void DtFont_DrawImageString(
		Display		*dpy,
		Drawable	draw,
		GC		gc,
		DtFont		font,
		int		x,
		int		y,
		const char	*str,
		int		len,
		unsigned long	bg_pixel);

extern int DtFont_TextWidth(
		DtFont		font,
		const char	*str,
		int		len);

extern int DtFont_Ascent(
		DtFont		font);

extern int DtFont_Descent(
		DtFont		font);

extern XmFontList _DtFontCreateXmFontList(
		Display		*dpy,
		const char	*pattern);

extern void DtFontInit(
		Display		*dpy);

#ifdef __cplusplus
}
#endif

#endif /* _DtFont_H */