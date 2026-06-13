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
 * DtFont.c - Font abstraction layer for CDE
 *
 * Provides an abstraction over core X11 font rendering and Xft,
 * allowing CDE to use Xft for anti-aliased text when available.
 */

#include <Dt/DtFont.h>
#include <DtI/DtFontI.h>

/*
 * Core X11 font fallback implementations.
 * These wrap XFontStruct/XFontSet into the DtFont opaque type
 * so that the API is uniform regardless of the rendering backend.
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <stdlib.h>

DtFont
DtFontCreate(Display *dpy, int screen, const char *pattern)
{
    struct _DtFont *df;
    XFontStruct *xfs;

    xfs = XLoadQueryFont(dpy, pattern);
    if (xfs == NULL)
        return NULL;

    df = (struct _DtFont *)malloc(sizeof(struct _DtFont));
    if (df == NULL) {
        XFreeFont(dpy, xfs);
        return NULL;
    }

    df->display = dpy;
    df->screen = screen;
    df->use_xft = False;
    df->xfs = xfs;
    df->fontset = NULL;

    return (DtFont)df;
}

void
DtFontDestroy(Display *dpy, DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL)
        return;

    if (df->xfs != NULL)
        XFreeFont(dpy, df->xfs);

    if (df->fontset != NULL)
        XFreeFontSet(dpy, df->fontset);

    free(df);
}

void
DtFont_DrawString(Display *dpy, Drawable draw, GC gc, DtFont font,
                   int x, int y, const char *str, int len)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL || df->xfs == NULL)
        return;

    XSetFont(dpy, gc, df->xfs->fid);
    XDrawString(dpy, draw, gc, x, y, str, len);
}

void
DtFont_DrawImageString(Display *dpy, Drawable draw, GC gc, DtFont font,
                        int x, int y, const char *str, int len,
                        unsigned long bg_pixel)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL || df->xfs == NULL)
        return;

    XSetFont(dpy, gc, df->xfs->fid);
    XDrawImageString(dpy, draw, gc, x, y, str, len);
}

int
DtFont_TextWidth(DtFont font, const char *str, int len)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL || df->xfs == NULL)
        return 0;

    return XTextWidth(df->xfs, str, len);
}

int
DtFont_Ascent(DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL || df->xfs == NULL)
        return 0;

    return df->xfs->ascent;
}

int
DtFont_Descent(DtFont font)
{
    struct _DtFont *df = (struct _DtFont *)font;

    if (df == NULL || df->xfs == NULL)
        return 0;

    return df->xfs->descent;
}

XmFontList
_DtFontCreateXmFontList(Display *dpy, const char *pattern)
{
    XFontStruct *xfs = XLoadQueryFont(dpy, pattern);
    if (xfs == NULL)
        return NULL;

    XmFontList fl = XmFontListCreate(xfs, XmFONTLIST_DEFAULT_TAG);
    return fl;
}