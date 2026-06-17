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
/* $TOG: FontEnum.h /main/1 2025/06/17 cde-font-picker $ */

#ifndef _DtFontEnum_H
#define _DtFontEnum_H

#include <X11/Xlib.h>

/*
 * Forward-declare XmFontList to avoid including Xm/Xm.h here.
 * DtFont.h includes Xm.h with the USE_XFT guard dance; callers
 * that need the full XmFontList definition should include Dt/DtFont.h.
 */
#ifndef _XmFontList_H
typedef struct _XmFontListRec *XmFontList;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DtFontEnumCoreX11,      /* XListFonts — core X11 fonts only */
    DtFontEnumFontconfig,   /* FcFontList — fontconfig (requires USE_XFT) */
    DtFontEnumAll            /* Both, with fontconfig preferred */
} DtFontEnumSource;

typedef struct {
    char *family_name;      /* e.g., "Helvetica", "DejaVu Sans" */
    char *full_name;         /* e.g., "Helvetica Bold", "DejaVu Sans" */
    char *xlfd;             /* XLFD name, NULL if not available */
    char *fc_pattern;        /* fontconfig pattern, NULL if USE_XFT off */
    int   is_scalable;      /* True if scalable font */
    int   pixel_size;       /* 0 if scalable */
    DtFontEnumSource source; /* Where this font was found */
} DtFontInfo;

typedef struct {
    DtFontInfo *fonts;
    int          count;
} DtFontList;

/* Enumerate available font families */
extern DtFontList *DtEnumerateFontFamilies(Display *dpy, int screen);

/* Enumerate sizes for a specific family */
extern DtFontList *DtEnumerateFontSizes(Display *dpy, int screen,
                                         const char *family_name,
                                         DtFontEnumSource source);

/* Free font list */
extern void DtFreeFontList(DtFontList *list);

/* Get XmFontList for a font pattern (uses _DtFontCreateXmFontList internally) */
extern XmFontList DtGetFontXmFontList(Display *dpy, const char *pattern);

#ifdef __cplusplus
}
#endif

#endif /* _DtFontEnum_H */