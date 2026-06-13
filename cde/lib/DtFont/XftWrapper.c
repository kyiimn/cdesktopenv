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
/* $TOG: XftWrapper.c /main/1 2025/06/14 cde-xft-migration $ */
/*
 *  (c) Copyright 2025 Hewlett-Packard Company
 *  (c) Copyright 2025 International Business Machines Corp.
 *  (c) Copyright 2025 Sun Microsystems, Inc.
 *  (c) Copyright 2025 Novell, Inc.
 */

/*
 * XftWrapper.c - Xft/fontconfig implementation of the DtFont API.
 *
 * Compiled only when USE_XFT is defined by configure (--enable-xft).
 * When USE_XFT is undefined this file contributes no symbols; the
 * core-font fallback in DtFont.c handles the same API surface.
 *
 * NOTE: <Xm/Xm.h> unconditionally defines `USE_XFT 1` if no prior
 * definition exists. The save/undef/restore pattern below cancels
 * Motif's define so the configure-driven -DUSE_XFT (or its absence)
 * is the only thing controlling the build path here.
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

#ifdef USE_XFT

#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft.h>
#include <fontconfig/fontconfig.h>

#include <Dt/DtFont.h>
#include <DtI/DtFontI.h>

/*
 * Per-(Display, Drawable) XftDraw cache.
 *
 * XftDraw is a lightweight rendering context bound to a drawable.
 * Creating one per draw call is wasteful, so we keep a small open-
 * addressed hash table of (Display*, Drawable) -> XftDraw* entries.
 *
 * The table is process-wide and never shrunk. When a drawable is
 * destroyed (e.g. window destruction) stale entries linger until the
 * table is rebuilt or the process exits. This is acceptable for the
 * same reasons Xt/Xm accept the trade-off: XftDrawClose is cheap and
 * a process-lifetime leak of a few bytes per destroyed widget is
 * insignificant.
 */
typedef struct _XftDrawEntry {
    Display   *dpy;
    Drawable   draw;
    XftDraw   *xftdraw;
} XftDrawEntry;

#define XFT_DRAW_CACHE_SIZE 256

static XftDrawEntry xft_draw_cache[XFT_DRAW_CACHE_SIZE];
static int         xft_draw_cache_used = 0;

static XftDraw *
get_xft_draw(Display *dpy, Drawable draw)
{
    XftDraw *xd;
    int      i;

    /* Linear search; the table is bounded and small. */
    for (i = 0; i < xft_draw_cache_used; i++) {
        if (xft_draw_cache[i].dpy == dpy &&
            xft_draw_cache[i].draw == draw)
            return xft_draw_cache[i].xftdraw;
    }

    if (xft_draw_cache_used >= XFT_DRAW_CACHE_SIZE) {
        /* Table full: replace slot 0. Acceptable degradation. */
        XftDrawChange(xft_draw_cache[0].xftdraw, draw);
        return xft_draw_cache[0].xftdraw;
    }

    xd = XftDrawCreate(dpy, draw,
                       DefaultVisual(dpy, DefaultScreen(dpy)),
                       DefaultColormap(dpy, DefaultScreen(dpy)));
    if (xd == NULL)
        return NULL;

    xft_draw_cache[xft_draw_cache_used].dpy = dpy;
    xft_draw_cache[xft_draw_cache_used].draw = draw;
    xft_draw_cache[xft_draw_cache_used].xftdraw = xd;
    xft_draw_cache_used++;

    return xd;
}

/*
 * Color cache: avoid re-allocating the same XftColor pixel per call.
 * Keyed by (Display*, screen, pixel). Linear search, bounded.
 */
typedef struct _XftColorEntry {
    Display       *dpy;
    int            screen;
    unsigned long  pixel;
    XftColor       color;
    Boolean        valid;
} XftColorEntry;

#define XFT_COLOR_CACHE_SIZE 64

static XftColorEntry xft_color_cache[XFT_COLOR_CACHE_SIZE];
static int           xft_color_cache_used = 0;

static Boolean
get_xft_color(Display *dpy, int screen, unsigned long pixel, XftColor *out)
{
    int i;

    for (i = 0; i < xft_color_cache_used; i++) {
        if (xft_color_cache[i].valid &&
            xft_color_cache[i].dpy == dpy &&
            xft_color_cache[i].screen == screen &&
            xft_color_cache[i].pixel == pixel) {
            *out = xft_color_cache[i].color;
            return True;
        }
    }

    {
        char name[12];
        XftColor tmp;
        snprintf(name, sizeof(name), "#%06lx", pixel & 0xFFFFFFul);

        if (XftColorAllocName(dpy, DefaultVisual(dpy, screen),
                              DefaultColormap(dpy, screen),
                              name, &tmp)) {
            if (xft_color_cache_used < XFT_COLOR_CACHE_SIZE) {
                xft_color_cache[xft_color_cache_used].dpy = dpy;
                xft_color_cache[xft_color_cache_used].screen = screen;
                xft_color_cache[xft_color_cache_used].pixel = pixel;
                xft_color_cache[xft_color_cache_used].color = tmp;
                xft_color_cache[xft_color_cache_used].valid = True;
                xft_color_cache_used++;
            }
            *out = tmp;
            return True;
        }
    }

    return False;
}

/*
 * Try to open a font by pattern. The pattern can be either a fontconfig
 * name (e.g. "Sans-12", "monospace:bold") or an XLFD name; XftFontOpenName
 * accepts both.
 */
static XftFont *
open_xft_font(Display *dpy, int screen, const char *pattern)
{
    XftFont *font;

    if (pattern == NULL || *pattern == '\0')
        return NULL;

    /* Path 1: name-based open (XLFD or fontconfig name). */
    font = XftFontOpenName(dpy, screen, pattern);
    if (font != NULL)
        return font;

    /* Path 2: parse explicitly as a fontconfig pattern. This handles
     * patterns that fontconfig's name parser rejects in XftFontOpenName
     * but which are valid FcPattern syntax. */
    {
        FcPattern *p = FcNameParse((const FcChar8 *)pattern);
        if (p != NULL) {
            XftFont *f2 = XftFontOpenPattern(dpy, p);
            FcPatternDestroy(p);
            if (f2 != NULL)
                return f2;
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------ */

DtFont
DtFontCreate(Display *dpy, int screen, const char *pattern)
{
    struct _DtFont *df;
    XftFont        *font;

    if (dpy == NULL || pattern == NULL)
        return NULL;

    font = open_xft_font(dpy, screen, pattern);
    if (font == NULL)
        return NULL;

    df = (struct _DtFont *)malloc(sizeof(struct _DtFont));
    if (df == NULL) {
        XftFontClose(dpy, font);
        return NULL;
    }

    df->display = dpy;
    df->screen  = screen;
    df->use_xft = True;
    df->xft_font = font;
    df->xfs     = NULL;
    df->fontset = NULL;

    return (DtFont)df;
}

void
DtFontDestroy(Display *dpy, DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL)
        return;

    if (df->use_xft && df->xft_font != NULL) {
        XftFontClose(dpy, df->xft_font);
        df->xft_font = NULL;
    }

    if (df->xfs != NULL) {
        XFreeFont(dpy, df->xfs);
        df->xfs = NULL;
    }

    if (df->fontset != NULL) {
        XFreeFontSet(dpy, df->fontset);
        df->fontset = NULL;
    }

    free(df);
}

void
DtFont_DrawString(Display *dpy, Drawable draw, GC gc, DtFont font,
                   int x, int y, const char *str, int len)
{
    struct _DtFont *df = (struct _DtFont *)font;
    XftDraw        *xd;
    XftColor        color;
    unsigned long   pixel;
    XGCValues       gcv;

    if (df == NULL || !df->use_xft || df->xft_font == NULL)
        return;
    if (str == NULL || len <= 0)
        return;

    xd = get_xft_draw(dpy, draw);
    if (xd == NULL)
        return;

    /* Xft ignores the GC; extract the foreground pixel via GetGCValues. */
    if (XGetGCValues(dpy, gc, GCForeground, &gcv) != 0)
        pixel = gcv.foreground;
    else
        pixel = BlackPixel(dpy, df->screen);

    if (!get_xft_color(dpy, df->screen, pixel, &color))
        return;

    XftDrawString8(xd, &color, df->xft_font, x, y,
                   (const FcChar8 *)str, (int)len);
}

void
DtFont_DrawImageString(Display *dpy, Drawable draw, GC gc, DtFont font,
                        int x, int y, const char *str, int len,
                        unsigned long bg_pixel)
{
    struct _DtFont *df = (struct _DtFont *)font;
    XftDraw        *xd;
    XftColor        fg_color, bg_color;
    XGCValues       gcv;
    unsigned long   fg_pixel;
    XGlyphInfo      extents;
    int             text_width;
    int             ascent;

    if (df == NULL || !df->use_xft || df->xft_font == NULL)
        return;
    if (str == NULL || len <= 0)
        return;

    xd = get_xft_draw(dpy, draw);
    if (xd == NULL)
        return;

    if (XGetGCValues(dpy, gc, GCForeground, &gcv) != 0)
        fg_pixel = gcv.foreground;
    else
        fg_pixel = BlackPixel(dpy, df->screen);

    if (!get_xft_color(dpy, df->screen, fg_pixel, &fg_color))
        return;
    if (!get_xft_color(dpy, df->screen, bg_pixel, &bg_color))
        return;

    /*
     * Measure first so the background rectangle exactly matches the
     * rendered text bounding box (XDrawImageString semantics).
     */
    XftTextExtents8(dpy, df->xft_font, (const FcChar8 *)str, (int)len,
                    &extents);

    text_width = extents.width;
    ascent     = df->xft_font->ascent;

    /* Guardrail G8: background fill is MANDATORY. */
    XftDrawRect(xd, &bg_color,
                x, y - ascent,
                (unsigned int)text_width,
                (unsigned int)(ascent + df->xft_font->descent));

    XftDrawString8(xd, &fg_color, df->xft_font, x, y,
                   (const FcChar8 *)str, (int)len);
}

int
DtFont_TextWidth(DtFont font, const char *str, int len)
{
    struct _DtFont *df = (struct _DtFont *)font;
    XGlyphInfo      extents;

    if (df == NULL || !df->use_xft || df->xft_font == NULL)
        return 0;
    if (str == NULL || len <= 0)
        return 0;

    XftTextExtents8(df->display, df->xft_font, (const FcChar8 *)str,
                    (int)len, &extents);
    return extents.width;
}

int
DtFont_Ascent(DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL || !df->use_xft || df->xft_font == NULL)
        return 0;

    return df->xft_font->ascent;
}

int
DtFont_Descent(DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL || !df->use_xft || df->xft_font == NULL)
        return 0;

    return df->xft_font->descent;
}

XmFontList
_DtFontCreateXmFontList(Display *dpy, const char *pattern)
{
    XftFont   *xft;
    XmFontList fl;

    if (dpy == NULL || pattern == NULL)
        return NULL;

    xft = open_xft_font(dpy, DefaultScreen(dpy), pattern);
    if (xft == NULL)
        return NULL;

    /*
     * Motif 2.3+ supports Xft-backed font list entries via
     * XmFontListEntryCreate with type XmFONT_IS_XFT, then
     * XmFontListAppendEntry. The entry holds the XftFont pointer.
     */
    {
        XmFontListEntry entry;
        XmFontType      ftype = XmFONT_IS_XFT;

        entry = XmFontListEntryCreate((char *)XmFONTLIST_DEFAULT_TAG,
                                       ftype,
                                       (XtPointer)xft);
        if (entry == NULL) {
            XftFontClose(dpy, xft);
            return NULL;
        }

        fl = XmFontListAppendEntry(NULL, entry);
        XmFontListEntryFree(&entry);
    }

    /* The font list now owns a reference via the entry; XftFontClose
     * happens in DtFontDestroy. For callers that use the XmFontList
     * without going through DtFont, the entry carries the lifetime. */
    return fl;
}

/* ------------------------------------------------------------------
 * Internal accessors (declared in DtFontI.h)
 * ------------------------------------------------------------------ */

DtFont
_DtFontPrivateCreate(Display *dpy, int screen, const char *pattern,
                     Boolean force_xft)
{
    if (force_xft)
        return DtFontCreate(dpy, screen, pattern);

    /* Without force_xft, defer to DtFontCreate; future implementations
     * may probe the display and pick the best backend. */
    return DtFontCreate(dpy, screen, pattern);
}

XftFont *
__DtFontGetXftFont(DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL || !df->use_xft)
        return NULL;

    return df->xft_font;
}

XFontStruct *
__DtFontGetXFontStruct(DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL)
        return NULL;

    return df->xfs;
}

XFontSet
__DtFontGetXFontSet(DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL)
        return NULL;

    return df->fontset;
}

Boolean
_DtFontIsXft(DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL)
        return False;

    return df->use_xft;
}

#else /* !USE_XFT */

/* Xft implementation not available - see DtFont.c for core font fallback */

#endif /* USE_XFT */
