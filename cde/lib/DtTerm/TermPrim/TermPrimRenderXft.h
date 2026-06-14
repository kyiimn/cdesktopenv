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
/*
 * $TOG: TermPrimRenderXft.h /main/1 2025/06/14 cde-xft-migration $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimRenderXft_h
#define	_Dt_TermPrimRenderXft_h

#include "TermPrimRender.h"

/*
 * The factory function uses XftFont, so we must not declare it unless
 * Xft is available. The .c file is wrapped in the same guard.
 */
#if defined(USE_XFT) || defined(HAVE_XFT)
#include <X11/Xft/Xft.h>

extern TermFont _DtTermPrimRenderXftCreate(
    Widget		  w,
    XftFont		 *xftFont
);
#endif /* USE_XFT || HAVE_XFT */

#endif	/* _Dt_TermPrimRenderXft_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
