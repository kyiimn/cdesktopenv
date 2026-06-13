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
/* $TOG: TermPrimRenderXft.c /main/1 2025/06/14 cde-xft-migration $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*
 * TermPrimRenderXft.c - Xft-backed vtable implementation for DtTerm.
 *
 * This file supplies the same vtable shape as TermPrimRenderFont.c and
 * TermPrimRenderFontSet.c, but uses Xft/fontconfig APIs to render text.
 * It participates in the vtable as another TermFont provider, alongside
 * the core-font and fontset renderers.
 *
 * Compiled only when USE_XFT is defined by configure (--enable-xft).
 * When USE_XFT is undefined the file contributes no symbols.
 *
 * <Xm/Xm.h> unconditionally defines USE_XFT 1 if no prior definition
 * exists.  The save/undef/restore pattern below cancels Motif's define
 * so the configure-driven -DUSE_XFT (or its absence) is the only thing
 * controlling the build path here.  This mirrors XftWrapper.c.
 */

#ifdef USE_XFT
#define _CDE_CONFIG_USE_XFT 1
#endif

#include "TermHeader.h"
#include "TermPrimP.h"
#include "TermPrimDebug.h"
#include "TermPrimRenderP.h"
#include "TermPrimRenderXft.h"

#ifdef USE_XFT
#undef USE_XFT
#endif

#ifdef _CDE_CONFIG_USE_XFT
#define USE_XFT 1
#undef _CDE_CONFIG_USE_XFT
#endif

#ifdef USE_XFT

#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft.h>

/*
 * Per-XftFont lazy XftDraw cache.
 *
 * XftDraw is a drawable-bound render context. We allocate it on first
 * use for a (Display, Drawable) pair, then call XftDrawChange when the
 * drawable may have changed (e.g. window re-realize). Storage is in
 * the TermXftFontRec so the cache is naturally reclaimed when the
 * vtable entry is destroyed.
 */
typedef struct _TermXftFontRec {
    XftFont *xftFont;
    XftDraw *xftDraw;
    Drawable lastDrawable;
    int      ascent;
    int      height;
    int      width;       /* cell width (monospace advance) */
} TermXftFontRec, *TermXftFont;

static XftDraw *
getOrCreateXftDraw(Widget w, TermXftFont termXftFont)
{
    Display *dpy = XtDisplay(w);
    Drawable win = XtWindow(w);

    if (termXftFont->xftDraw == NULL) {
        termXftFont->xftDraw = XftDrawCreate(
            dpy, win,
            DefaultVisual(dpy, DefaultScreen(dpy)),
            DefaultColormap(dpy, DefaultScreen(dpy)));
        termXftFont->lastDrawable = win;
    } else if (termXftFont->lastDrawable != win) {
        XftDrawChange(termXftFont->xftDraw, win);
        termXftFont->lastDrawable = win;
    }

    return termXftFont->xftDraw;
}

static Boolean
allocXftColor(Widget w, Pixel pixel, XftColor *out)
{
    Display *dpy = XtDisplay(w);
    int      scr = DefaultScreen(dpy);
    char     name[12];

    snprintf(name, sizeof(name), "#%06lx", (unsigned long)(pixel & 0xFFFFFFul));
    return XftColorAllocName(dpy,
                             DefaultVisual(dpy, scr),
                             DefaultColormap(dpy, scr),
                             name, out);
}

static void
FontXftRenderFunction(
    Widget		  w,
    TermFont		  font,
    Pixel		  fg,
    Pixel		  bg,
    unsigned long	  flags,
    int			  x,
    int			  y,
    unsigned char	 *string,
    int			  len
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData      *tpd = tw->term.tpd;
    TermXftFont           termXftFont = (TermXftFont) font->fontInfo;
    XftDraw              *xftDraw;
    XftColor              xftFgColor;
    XftColor              xftBgColor;
    Pixel                 drawFg;
    Pixel                 drawBg;
    int                   cellWidth;
    int                   cellHeight;
    int                   width;

    if (termXftFont == NULL || termXftFont->xftFont == NULL || len <= 0)
        return;

    xftDraw = getOrCreateXftDraw(w, termXftFont);
    if (xftDraw == NULL)
        return;

    /*
     * SECURE mode: text is rendered in the background color, so the
     * foreground fill itself is invisible. We still emit the
     * background rect first to overwrite the previous glyph, then
     * re-draw the background-colored text on top.
     */
    if (TermIS_SECURE(flags))
        drawFg = bg;
    else
        drawFg = fg;
    drawBg = bg;

    if (!allocXftColor(w, drawBg, &xftBgColor))
        return;
    if (!allocXftColor(w, drawFg, &xftFgColor)) {
        XftColorFree(XtDisplay(w),
                     DefaultVisual(XtDisplay(w), DefaultScreen(XtDisplay(w))),
                     DefaultColormap(XtDisplay(w), DefaultScreen(XtDisplay(w))),
                     &xftBgColor);
        return;
    }

    cellWidth  = (int) tpd->cellWidth;
    cellHeight = (int) tpd->cellHeight;
    width = len * cellWidth;

    /*
     * Guardrail G8: background fill MUST precede foreground text.
     * The rectangle covers the cell from baseline-ascent to the
     * bottom of descent, matching the cell height in pixels.
     */
    XftDrawRect(xftDraw, &xftBgColor,
                x, y - termXftFont->ascent,
                (unsigned int) width,
                (unsigned int) cellHeight);

    XftDrawString8(xftDraw, &xftFgColor,
                   termXftFont->xftFont,
                   x, y,
                   (const FcChar8 *) string, len);

    /* handle overstrike: draw text shifted by 1 pixel to the right. */
    if (TermIS_OVERSTRIKE(flags)) {
        XftDrawString8(xftDraw, &xftFgColor,
                       termXftFont->xftFont,
                       x + 1, y,
                       (const FcChar8 *) string, len);
    }

    /* handle the underline enhancement. */
    if (TermIS_UNDERLINE(flags)) {
        XftDrawRect(xftDraw, &xftFgColor,
                    x, y + cellHeight - 1,
                    (unsigned int) width, 1);
    }

    XftColorFree(XtDisplay(w),
                 DefaultVisual(XtDisplay(w), DefaultScreen(XtDisplay(w))),
                 DefaultColormap(XtDisplay(w), DefaultScreen(XtDisplay(w))),
                 &xftFgColor);
    XftColorFree(XtDisplay(w),
                 DefaultVisual(XtDisplay(w), DefaultScreen(XtDisplay(w))),
                 DefaultColormap(XtDisplay(w), DefaultScreen(XtDisplay(w))),
                 &xftBgColor);
}

static void
FontXftDestroyFunction(
    Widget		  w,
    TermFont		  font
)
{
    TermXftFont termXftFont;

    if (font == NULL)
        return;

    termXftFont = (TermXftFont) font->fontInfo;
    if (termXftFont != NULL) {
        /*
         * The XftFont is owned by the DtFont (or whoever created the
         * underlying Xft handle) and is not closed here.  We only
         * dispose of the per-vtable XftDraw and the wrapper record.
         */
        if (termXftFont->xftDraw != NULL) {
            XftDrawDestroy(termXftFont->xftDraw);
            termXftFont->xftDraw = NULL;
        }
        (void) XtFree((char *) termXftFont);
    }
    (void) XtFree((char *) font);
}

static void
FontXftExtentsFunction(
    Widget		  w,
    TermFont		  font,
    unsigned char	 *string,
    int			  len,
    int			 *widthReturn,
    int			 *heightReturn,
    int			 *ascentReturn
)
{
    TermXftFont termXftFont = (TermXftFont) font->fontInfo;

    if (termXftFont == NULL || termXftFont->xftFont == NULL)
        return;

    if (widthReturn) {
        *widthReturn = len * termXftFont->width;
    }
    if (heightReturn) {
        *heightReturn = termXftFont->height;
    }
    if (ascentReturn) {
        *ascentReturn = termXftFont->ascent;
    }
}

TermFont
_DtTermPrimRenderXftCreate(
    Widget		  w,
    XftFont		 *xftFont
)
{
    TermFont     termFont;
    TermXftFont  termXftFont;

    if (xftFont == NULL)
        return NULL;

    termFont = (TermFont) XtMalloc(sizeof(TermFontRec));
    if (termFont == NULL)
        return NULL;
    termFont->renderFunction  = FontXftRenderFunction;
    termFont->destroyFunction = FontXftDestroyFunction;
    termFont->extentsFunction = FontXftExtentsFunction;
    termFont->fontInfo        = NULL;

    termXftFont = (TermXftFont) XtMalloc(sizeof(TermXftFontRec));
    if (termXftFont == NULL) {
        XtFree((char *) termFont);
        return NULL;
    }
    termXftFont->xftFont      = xftFont;
    termXftFont->xftDraw      = NULL;
    termXftFont->lastDrawable = None;
    termXftFont->ascent       = xftFont->ascent;
    termXftFont->height       = xftFont->ascent + xftFont->descent;
    termXftFont->width        = xftFont->max_advance_width;

    termFont->fontInfo = (XtPointer) termXftFont;

    return termFont;
}

#else /* !USE_XFT */

/* Xft renderer not available; the core-font and fontset renderers are used. */

#endif /* USE_XFT */
