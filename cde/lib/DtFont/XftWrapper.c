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
#include <Xm/Display.h>

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
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <fontconfig/fontconfig.h>

#include <Dt/DtFont.h>
#include <DtI/DtFontI.h>

static void
DtFontGetDisplayArg(Widget widget, Cardinal *size, XrmValue *value)
{
    if (widget == NULL)
        XtErrorMsg("missingWidget", "DtFontGetDisplayArg", "XtToolkitError",
                   "DtFontGetDisplayArg called without a widget to reference",
                   (String *)NULL, (Cardinal *)NULL);

    value->size = sizeof(Display *);
    value->addr = (XPointer)&DisplayOfScreen(XtScreenOfObject(widget));
}

static XtConvertArgRec dtFontDisplayArg[] = {
    { XtProcedureArg, (XtPointer)DtFontGetDisplayArg, 0 },
};

static void
CvtStringToDtFontListDestroy(XtAppContext app, XrmValue *to,
                             XtPointer converter_data,
                             XrmValue *args, Cardinal *num_args)
{
    (void)app;
    (void)converter_data;
    (void)args;
    (void)num_args;

    if (to != NULL && to->addr != NULL)
        XmFontListFree(*((XmFontList *)to->addr));
}

static Boolean
CvtStringToDtFontList(Display *dpy, XrmValue *args, Cardinal *num_args,
                      XrmValue *from, XrmValue *to,
                      XtPointer *converter_data)
{
    const char *from_string;
    XmFontList fl;
    XmFontListEntry entry;

    (void)args;
    (void)num_args;
    (void)converter_data;

    if (from == NULL || from->addr == NULL)
        return FALSE;

    from_string = (const char *)from->addr;

    /*
     * If the pattern starts with '-' it is an XLFD name — load it as a
     * core X11 font via XmFontListEntryLoad so that the original font
     * metrics are preserved.  XftFontOpenName can also match XLFD
     * patterns, but the resulting Xft font has different metrics
     * (anti-aliased rasterisation, different ascent/descent) that break
     * Motif widget layout (icon positioning, label truncation, list
     * redraw artifacts).  Only non-XLFD patterns (fontconfig names like
     * "Sans-12" or "UbuntuMono Nerd Font") go through the Xft path.
     */
    if (from_string[0] != '-') {
        fl = _DtFontCreateXmFontList(dpy, from_string);
        if (fl != NULL) {
            if (to->addr != NULL) {
                if (to->size < sizeof(XmFontList)) {
                    to->size = sizeof(XmFontList);
                    XmFontListFree(fl);
                    return FALSE;
                }
                *((XmFontList *)to->addr) = fl;
            } else {
                to->addr = (XPointer)&fl;
            }
            to->size = sizeof(XmFontList);
            return TRUE;
        }
    }

    /* XLFD patterns, or fontconfig patterns that Xft couldn't open,
     * fall through to the core X11 font path. */
    entry = XmFontListEntryLoad(dpy, (char *)from_string, XmFONT_IS_FONT,
                                XmFONTLIST_DEFAULT_TAG);
    if (entry != NULL) {
        fl = XmFontListAppendEntry(NULL, entry);
        XmFontListEntryFree(&entry);
        if (fl != NULL) {
            if (to->addr != NULL) {
                if (to->size < sizeof(XmFontList)) {
                    to->size = sizeof(XmFontList);
                    XmFontListFree(fl);
                    return FALSE;
                }
                *((XmFontList *)to->addr) = fl;
            } else {
                to->addr = (XPointer)&fl;
            }
            to->size = sizeof(XmFontList);
            return TRUE;
        }
    }

    return FALSE;
}

static Boolean
CvtStringToDtFontStruct(Display *dpy, XrmValue *args, Cardinal *num_args,
                          XrmValue *from, XrmValue *to,
                          XtPointer *converter_data)
{
    const char *from_string;
    XFontStruct *xfs;

    (void)args;
    (void)num_args;
    (void)converter_data;

    if (from == NULL || from->addr == NULL)
        return FALSE;

    from_string = (const char *)from->addr;

    /*
     * FontStruct resources (*font: ...) expect an XFontStruct.
     * XLFD patterns always go through XLoadQueryFont directly -
     * they are core X11 font names.
     */
    if (from_string[0] == '-') {
        xfs = XLoadQueryFont(dpy, from_string);
        if (xfs != NULL) {
            if (to->addr != NULL) {
                if (to->size < sizeof(XFontStruct *)) {
                    to->size = sizeof(XFontStruct *);
                    return FALSE;
                }
                *((XFontStruct **)to->addr) = xfs;
            } else {
                to->addr = (XPointer)&xfs;
            }
            to->size = sizeof(XFontStruct *);
            return TRUE;
        }
    }

    /*
     * Non-XLFD (fontconfig) patterns cannot be loaded directly as
     * XFontStruct. Resolve the pattern through fontconfig to a
     * matching core X11 font name, then load that. If resolution
     * fails, try the original pattern as-is, and finally "fixed".
     */
    if (from_string[0] != '-') {
        FcPattern *pat = FcNameParse((const FcChar8 *)from_string);
        if (pat != NULL) {
            FcResult result;
            FcPattern *match = FcFontMatch(NULL, pat, &result);
            if (match != NULL) {
                FcChar8 *family = NULL;
                FcChar8 *file = NULL;
                double dsize = 0.0;
                int size = 0;
                char core_pattern[256];

                if (FcPatternGetString(match, FC_FAMILY, 0, &family)
                        == FcResultMatch
                    && family != NULL) {
                    if (FcPatternGetDouble(match, FC_PIXEL_SIZE, 0, &dsize)
                            == FcResultMatch
                        && dsize > 0.0)
                        size = (int)(dsize + 0.5);

                    if (size > 0) {
                        snprintf(core_pattern, sizeof(core_pattern),
                                 "-%s-0-0-0-0-*-0-%d-*-*-*-*-*",
                                 (char *)family, size);
                        xfs = XLoadQueryFont(dpy, core_pattern);
                        if (xfs == NULL) {
                            snprintf(core_pattern, sizeof(core_pattern),
                                     "-%s-*-*-*-*-*-*-%d-*-*-*-*-*-*",
                                     (char *)family, size);
                            xfs = XLoadQueryFont(dpy, core_pattern);
                        }
                        if (xfs == NULL) {
                            snprintf(core_pattern, sizeof(core_pattern),
                                     "%s-%d", (char *)family, size);
                            xfs = XLoadQueryFont(dpy, core_pattern);
                        }
                    } else {
                        snprintf(core_pattern, sizeof(core_pattern),
                                 "-%s-*-*-*-*-*-*-*-*-*-*-*-*-*",
                                 (char *)family);
                        xfs = XLoadQueryFont(dpy, core_pattern);
                    }
                }

                /*
                 * Some core X11 fonts can be located by the fontconfig
                 * file path when FcPattern exposes FC_FILE.
                 */
                if (xfs == NULL
                    && FcPatternGetString(match, FC_FILE, 0, &file)
                           == FcResultMatch
                    && file != NULL) {
                    xfs = XLoadQueryFont(dpy, (const char *)file);
                }

                FcPatternDestroy(match);
            }
            FcPatternDestroy(pat);
        }

        /* Try the original pattern as-is in case it names a core font. */
        if (xfs == NULL)
            xfs = XLoadQueryFont(dpy, from_string);
    }

    /* Last resort: "fixed" so Motif doesn't crash. */
    if (xfs == NULL)
        xfs = XLoadQueryFont(dpy, "fixed");

    if (xfs != NULL) {
        if (to->addr != NULL) {
            if (to->size < sizeof(XFontStruct *)) {
                to->size = sizeof(XFontStruct *);
                XFreeFont(dpy, xfs);
                return FALSE;
            }
            *((XFontStruct **)to->addr) = xfs;
        } else {
            to->addr = (XPointer)&xfs;
        }
        to->size = sizeof(XFontStruct *);
        return TRUE;
    }

    return FALSE;
}

void
DtFontInit(Display *dpy)
{
    static Boolean initialized = False;

    if (initialized)
        return;
    initialized = True;

    (void)dpy;

    XtSetTypeConverter(XmRString, XmRFontList, CvtStringToDtFontList,
                       dtFontDisplayArg, XtNumber(dtFontDisplayArg),
                       XtCacheByDisplay, CvtStringToDtFontListDestroy);

    XtSetTypeConverter(XmRString, XmRFontStruct, CvtStringToDtFontStruct,
                       dtFontDisplayArg, XtNumber(dtFontDisplayArg),
                       XtCacheByDisplay, NULL);
}

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

    font = XftFontOpenName(dpy, screen, pattern);
    if (font != NULL)
        return font;

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
    if (xft == NULL) {
        return NULL;
    }

    {
        XmFontListEntry entry;
        XmFontType      ftype = XmFONT_IS_XFT;
        Widget          xmDisplay = XmGetXmDisplay(dpy);

        if (xmDisplay != NULL) {
            entry = XmFontListEntryCreate_r(
                        (char *)XmFONTLIST_DEFAULT_TAG,
                        ftype,
                        (XtPointer)xft,
                        xmDisplay);
        } else {
            entry = XmFontListEntryCreate(
                        (char *)XmFONTLIST_DEFAULT_TAG,
                        ftype,
                        (XtPointer)xft);
        }
        if (entry != NULL) {
            fl = XmFontListAppendEntry(NULL, entry);
            XmFontListEntryFree(&entry);
            if (fl != NULL) {
                return fl;
            }
        }
    }

    /*
     * Strategy 2: load a core X11 font from the family name.
     * This loses anti-aliasing but works with any Motif build.
     * Try family-size (e.g. "Noto Sans-12"), original pattern
     * (might be an XLFD), then "fixed" as last resort.
     */
    {
        XFontStruct  *xfs = NULL;
        FcPattern    *pat = xft->pattern;
        char          core_pattern[256];
        FcChar8      *family = NULL;
        int           size = 12;
        double       dsize;

        if (FcPatternGetString(pat, FC_FAMILY, 0, &family) == FcResultMatch
            && family != NULL) {
            if (FcPatternGetDouble(pat, FC_PIXEL_SIZE, 0, &dsize)
                    == FcResultMatch
                && dsize > 0.0)
                size = (int)(dsize + 0.5);

            snprintf(core_pattern, sizeof(core_pattern),
                     "%s-%d", (char *)family, size);
            xfs = XLoadQueryFont(dpy, core_pattern);
        }

        if (xfs == NULL) {
            xfs = XLoadQueryFont(dpy, pattern);
        }

        if (xfs == NULL) {
            xfs = XLoadQueryFont(dpy, "fixed");
        }

        if (xfs != NULL) {
            fl = XmFontListCreate(xfs, XmFONTLIST_DEFAULT_TAG);
            if (fl != NULL) {
                XftFontClose(dpy, xft);
                return fl;
            }
            XFreeFont(dpy, xfs);
        }
    }

    /* All strategies failed. */
    XftFontClose(dpy, xft);
    return NULL;
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
