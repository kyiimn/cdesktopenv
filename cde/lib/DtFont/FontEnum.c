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
/* $TOG: FontEnum.c /main/1 2025/06/17 cde-font-picker $ */

/*
 * FontEnum.c - Font enumeration library for CDE.
 *
 * Always compiled. Provides X11 core font enumeration (XListFonts) and,
 * when USE_XFT is defined by configure, fontconfig enumeration (FcFontList).
 * The two sources are merged by DtEnumerateFontFamilies / DtEnumerateFontSizes
 * with fontconfig entries preferred for duplicate family names.
 *
 * The USE_XFT save/undef/restore pattern below mirrors the one in DtFont.h.
 * Motif 2.3+ defines USE_XFT in Xm.h unconditionally; the dance ensures only
 * the configure-driven -DUSE_XFT (or its absence) controls the build path.
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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef USE_XFT
#include <fontconfig/fontconfig.h>
#include <X11/Xft/Xft.h>
#endif

#include <Dt/DtFont.h>

/*
 * <Dt/FontEnum.h> forward-declares XmFontList as `struct _XmFontListRec *`
 * (guarded by _XmFontList_H, a name Motif's Xm.h does not actually use).
 * The real Motif typedef is `struct __XmRenderTableRec **`. The two are
 * incompatible, so we pre-define the guard here to suppress the broken
 * forward declaration and let the real Xm.h typedef from above stand.
 */
#ifndef _XmFontList_H
#define _XmFontList_H
#endif
#include <Dt/FontEnum.h>

/* ------------------------------------------------------------------
 * Internal helpers (always compiled)
 * ------------------------------------------------------------------ */

/*
 * Parse an XLFD name and return a strdup'd copy of the family name
 * (the 2nd dash-delimited field, after the leading '-'). The caller
 * is responsible for free()ing the returned string.
 *
 * XLFD format: -foundry-family-weight-slant-width-addstyle-pixelsize-
 *               pointsize-resolution-resolution-spacing-avgwidth-
 *               registry-encoding
 *
 * Returns NULL if the input is malformed.
 */
static char *
xlfd_to_family(const char *xlfd)
{
    const char *p;
    const char *dash;
    size_t      len;

    if (xlfd == NULL || xlfd[0] != '-')
        return NULL;

    p = xlfd + 1;
    dash = strchr(p, '-');
    if (dash == NULL)
        len = strlen(p);
    else
        len = (size_t)(dash - p);

    if (len == 0)
        return NULL;

    {
        char *family = (char *)malloc(len + 1);
        if (family == NULL)
            return NULL;
        memcpy(family, p, len);
        family[len] = '\0';
        return family;
    }
}

/*
 * Case-insensitive comparison for family deduplication.
 * Returns 1 if s1 and s2 are equal ignoring case, 0 otherwise.
 * NULL inputs are treated as not-equal.
 */
static int
family_strcasecmp(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL)
        return 0;
    return (strcasecmp(s1, s2) == 0) ? 1 : 0;
}

/*
 * Check whether `family` is already present in the `seen` array
 * (which holds `count` previously-accepted family names).
 */
static int
is_duplicate_family(char **seen, int count, const char *family)
{
    int i;
    for (i = 0; i < count; i++) {
        if (family_strcasecmp(seen[i], family))
            return 1;
    }
    return 0;
}

/*
 * Build a DtFontList from a list of XLFD names, deduplicating by
 * family name. Each unique family becomes one DtFontInfo entry whose
 * xlfd field holds the first matching XLFD. `source` records the
 * provenance of every entry in the result.
 */
static DtFontList *
build_list_from_xlfd(char **font_names, int count, DtFontEnumSource source)
{
    DtFontList *list;
    char      **seen;
    int         seen_count;
    int         i;

    if (font_names == NULL || count <= 0)
        return NULL;

    list = (DtFontList *)calloc(1, sizeof(DtFontList));
    if (list == NULL)
        return NULL;

    /* Allocate worst-case array; we'll only use the first list->count slots. */
    list->fonts = (DtFontInfo *)calloc((size_t)count, sizeof(DtFontInfo));
    if (list->fonts == NULL) {
        free(list);
        return NULL;
    }

    seen = (char **)calloc((size_t)count, sizeof(char *));
    if (seen == NULL) {
        free(list->fonts);
        free(list);
        return NULL;
    }
    seen_count = 0;

    for (i = 0; i < count; i++) {
        char *family = xlfd_to_family(font_names[i]);
        if (family == NULL)
            continue;

        if (is_duplicate_family(seen, seen_count, family)) {
            free(family);
            continue;
        }

        seen[seen_count++] = family;

        list->fonts[list->count].family_name = family;
        list->fonts[list->count].full_name   = strdup(family);
        list->fonts[list->count].xlfd        = strdup(font_names[i]);
        list->fonts[list->count].fc_pattern  = NULL;
        list->fonts[list->count].is_scalable = 1;
        list->fonts[list->count].pixel_size  = 0;
        list->fonts[list->count].source      = source;
        list->count++;
    }

    free(seen);

    if (list->count == 0) {
        free(list->fonts);
        free(list);
        return NULL;
    }

    return list;
}

/* ------------------------------------------------------------------
 * X11 core font enumeration (always compiled)
 * ------------------------------------------------------------------ */

DtFontList *
DtEnumerateFontFamilies(Display *dpy, int screen)
{
    char       **font_names;
    int          count;
    DtFontList  *x11_list;

    (void)screen;

    if (dpy == NULL)
        return NULL;

    /* The '-' pattern matches every font on the server. */
    font_names = XListFonts(dpy, "-", 8192, &count);
    if (font_names == NULL || count <= 0) {
        if (font_names != NULL)
            XFreeFontNames(font_names);
        return NULL;
    }

    x11_list = build_list_from_xlfd(font_names, count, DtFontEnumCoreX11);
    XFreeFontNames(font_names);

#ifdef USE_XFT
    if (x11_list != NULL) {
        extern DtFontList *DtEnumerateFontFamiliesFC(Display *dpy, int screen);
        DtFontList *fc_list = DtEnumerateFontFamiliesFC(dpy, screen);
        if (fc_list != NULL) {
            extern DtFontList *DtMergeFontLists(DtFontList *fc,
                                                DtFontList *x11);
            DtFontList *merged = DtMergeFontLists(fc_list, x11_list);
            DtFreeFontList(fc_list);
            DtFreeFontList(x11_list);
            return merged;
        }
    }
#endif

    return x11_list;
}

DtFontList *
DtEnumerateFontSizes(Display *dpy, int screen,
                     const char *family_name,
                     DtFontEnumSource source)
{
    char        pattern[256];
    char      **font_names;
    int         count;
    DtFontList *list;
    int         i;
    char      **unique_sizes;
    int         unique_count;

    if (dpy == NULL || family_name == NULL || family_name[0] == '\0')
        return NULL;

    /*
     * Build an XLFD pattern: -foundry-family-*-*-*-*-*-*-*-*-*-*-*
     * The leading '-' and the 14 dashes thereafter give us 14 wildcards
     * (one per field after foundry/family), which XListFonts accepts.
     */
    (void)screen;

    snprintf(pattern, sizeof(pattern), "-*-%s-*", family_name);
    font_names = XListFonts(dpy, pattern, 4096, &count);
    if (font_names == NULL || count <= 0) {
        if (font_names != NULL)
            XFreeFontNames(font_names);
#ifdef USE_XFT
        if (source == DtFontEnumFontconfig || source == DtFontEnumAll) {
            /* Defer to the fontconfig helper. */
            extern DtFontList *DtEnumerateFontSizesFC(Display *dpy, int screen,
                                                     const char *family_name);
            return DtEnumerateFontSizesFC(dpy, screen, family_name);
        }
#endif
        return NULL;
    }

    list = (DtFontList *)calloc(1, sizeof(DtFontList));
    if (list == NULL) {
        XFreeFontNames(font_names);
        return NULL;
    }

    list->fonts = (DtFontInfo *)calloc((size_t)count, sizeof(DtFontInfo));
    if (list->fonts == NULL) {
        free(list);
        XFreeFontNames(font_names);
        return NULL;
    }

    unique_sizes = (char **)calloc((size_t)count, sizeof(char *));
    if (unique_sizes == NULL) {
        free(list->fonts);
        free(list);
        XFreeFontNames(font_names);
        return NULL;
    }
    unique_count = 0;

    for (i = 0; i < count; i++) {
        /*
         * XLFD: -foundry-family-weight-slant-width-addstyle-pixelsize-...
         * Field index 6 is pixel size (zero-based from after the leading
         * '-'). dtcm/font.c uses sscanf with a similar layout.
         */
        int pixel_size = 0;
        char *family = xlfd_to_family(font_names[i]);
        char sizebuf[16];
        char unique_key[64];

        if (family == NULL)
            continue;

        /* Only accept sizes for the requested family. */
        if (strcasecmp(family, family_name) != 0) {
            free(family);
            continue;
        }

        sscanf(font_names[i],
               "-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%d",
               &pixel_size);

        snprintf(sizebuf, sizeof(sizebuf), "%d", pixel_size);
        snprintf(unique_key, sizeof(unique_key), "%s:%d", family, pixel_size);

        if (is_duplicate_family(unique_sizes, unique_count, unique_key)) {
            free(family);
            continue;
        }

        unique_sizes[unique_count++] = strdup(unique_key);

        list->fonts[list->count].family_name = family;
        list->fonts[list->count].full_name   = strdup(family);
        list->fonts[list->count].xlfd        = strdup(font_names[i]);
        list->fonts[list->count].fc_pattern  = NULL;
        list->fonts[list->count].is_scalable = 0;
        list->fonts[list->count].pixel_size  = pixel_size;
        list->fonts[list->count].source      = source;
        list->count++;

        free(family);
    }

    for (i = 0; i < unique_count; i++)
        free(unique_sizes[i]);
    free(unique_sizes);

    XFreeFontNames(font_names);

    if (list->count == 0) {
        free(list->fonts);
        free(list);
        list = NULL;
    }

#ifdef USE_XFT
    if (source == DtFontEnumFontconfig || source == DtFontEnumAll) {
        extern DtFontList *DtEnumerateFontSizesFC(Display *dpy, int screen,
                                                  const char *family_name);
        DtFontList *fc_list = DtEnumerateFontSizesFC(dpy, screen, family_name);
        if (fc_list != NULL) {
            extern DtFontList *DtMergeFontLists(DtFontList *fc,
                                                DtFontList *x11);
            DtFontList *merged = DtMergeFontLists(fc_list, list);
            DtFreeFontList(fc_list);
            if (list != NULL)
                DtFreeFontList(list);
            return merged;
        }
    }
#endif

    return list;
}

/* ------------------------------------------------------------------
 * Public: free a font list
 * ------------------------------------------------------------------ */

void
DtFreeFontList(DtFontList *list)
{
    int i;

    if (list == NULL)
        return;

    if (list->fonts != NULL) {
        for (i = 0; i < list->count; i++) {
            if (list->fonts[i].family_name != NULL)
                free(list->fonts[i].family_name);
            if (list->fonts[i].full_name != NULL)
                free(list->fonts[i].full_name);
            if (list->fonts[i].xlfd != NULL)
                free(list->fonts[i].xlfd);
            if (list->fonts[i].fc_pattern != NULL)
                free(list->fonts[i].fc_pattern);
        }
        free(list->fonts);
    }
    free(list);
}

/* ------------------------------------------------------------------
 * Public: resolve a pattern to an XmFontList
 *
 * _DtFontCreateXmFontList accepts both XLFD and fontconfig patterns
 * (XftWrapper.c / DtFont.c), so we hand the pattern through verbatim.
 * No XLFD↔fontconfig conversion is performed here.
 * ------------------------------------------------------------------ */

XmFontList
DtGetFontXmFontList(Display *dpy, const char *pattern)
{
    if (dpy == NULL || pattern == NULL)
        return NULL;

    return _DtFontCreateXmFontList(dpy, pattern);
}

/* ------------------------------------------------------------------
 * fontconfig enumeration (USE_XFT only)
 *
 * These helpers are only compiled when configure has selected
 * --enable-xft. The non-XFT build omits the entire block so the
 * library still links cleanly on systems without fontconfig.
 * ------------------------------------------------------------------ */

#ifdef USE_XFT

static DtFontList *build_list_from_xlfd_no_free(DtFontList *src);

/*
 * Enumerate fontconfig families.
 *
 * Returns a DtFontList whose entries' family_name and fc_pattern
 * are the fontconfig family string, source = DtFontEnumFontconfig.
 * Returns NULL on any failure (including fontconfig initialization
 * problems or empty result sets).
 */
DtFontList *
DtEnumerateFontFamiliesFC(Display *dpy, int screen)
{
    FcPattern    *pat;
    FcObjectSet  *os;
    FcFontSet    *fs;
    DtFontList   *list;
    int           i;
    char        **seen;
    int           seen_count;

    (void)dpy;
    (void)screen;

    pat = FcPatternCreate();
    if (pat == NULL)
        return NULL;

    os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FULLNAME, FC_SCALABLE,
                          FC_PIXEL_SIZE, (char *)NULL);
    if (os == NULL) {
        FcPatternDestroy(pat);
        return NULL;
    }

    fs = FcFontList((FcConfig *)NULL, pat, os);
    FcObjectSetDestroy(os);
    FcPatternDestroy(pat);

    if (fs == NULL || fs->nfont <= 0) {
        if (fs != NULL)
            FcFontSetDestroy(fs);
        return NULL;
    }

    list = (DtFontList *)calloc(1, sizeof(DtFontList));
    if (list == NULL) {
        FcFontSetDestroy(fs);
        return NULL;
    }

    list->fonts = (DtFontInfo *)calloc((size_t)fs->nfont, sizeof(DtFontInfo));
    if (list->fonts == NULL) {
        free(list);
        FcFontSetDestroy(fs);
        return NULL;
    }

    seen = (char **)calloc((size_t)fs->nfont, sizeof(char *));
    if (seen == NULL) {
        free(list->fonts);
        free(list);
        FcFontSetDestroy(fs);
        return NULL;
    }
    seen_count = 0;

    for (i = 0; i < fs->nfont; i++) {
        FcChar8 *family = NULL;
        FcChar8 *style  = NULL;
        FcChar8 *fullname = NULL;
        FcBool   scalable = FcFalse;
        double   pxsize = 0.0;
        char    *family_dup;
        char    *full_dup;
        char    *fc_dup;
        char     sizebuf[32];

        FcPatternGetString(fs->fonts[i], FC_FAMILY, 0, &family);
        FcPatternGetString(fs->fonts[i], FC_STYLE, 0, &style);
        FcPatternGetString(fs->fonts[i], FC_FULLNAME, 0, &fullname);
        FcPatternGetBool(fs->fonts[i], FC_SCALABLE, 0, &scalable);
        FcPatternGetDouble(fs->fonts[i], FC_PIXEL_SIZE, 0, &pxsize);

        if (family == NULL)
            continue;

        family_dup = strdup((const char *)family);
        if (family_dup == NULL)
            continue;

        if (is_duplicate_family(seen, seen_count, family_dup)) {
            free(family_dup);
            continue;
        }
        seen[seen_count++] = family_dup;

        if (fullname != NULL)
            full_dup = strdup((const char *)fullname);
        else if (style != NULL)
            (void)snprintf(sizebuf, sizeof(sizebuf), "%s %s",
                           (const char *)family, (const char *)style),
                full_dup = strdup(sizebuf);
        else
            full_dup = strdup(family_dup);

        fc_dup = strdup(family_dup);

        list->fonts[list->count].family_name = family_dup;
        list->fonts[list->count].full_name   = full_dup;
        list->fonts[list->count].xlfd        = NULL;
        list->fonts[list->count].fc_pattern  = fc_dup;
        list->fonts[list->count].is_scalable = (scalable != FcFalse) ? 1 : 0;
        list->fonts[list->count].pixel_size  = (scalable != FcFalse) ?
                                               0 : (int)(pxsize + 0.5);
        list->fonts[list->count].source      = DtFontEnumFontconfig;
        list->count++;
    }

    free(seen);
    FcFontSetDestroy(fs);

    if (list->count == 0) {
        free(list->fonts);
        free(list);
        return NULL;
    }

    return list;
}

/*
 * Enumerate sizes for a family using fontconfig.
 *
 * Builds a pattern with FC_FAMILY set to family_name and queries
 * FcFontList. Each unique (family, pixel_size) pair becomes an entry.
 * If a font is scalable, the entry uses pixel_size = 0.
 */
DtFontList *
DtEnumerateFontSizesFC(Display *dpy, int screen, const char *family_name)
{
    FcPattern    *pat;
    FcObjectSet  *os;
    FcFontSet    *fs;
    DtFontList   *list;
    int           i;
    char        **seen;
    int           seen_count;
    char          sizebuf[32];
    char          unique_key[96];

    (void)dpy;
    (void)screen;

    if (family_name == NULL || family_name[0] == '\0')
        return NULL;

    pat = FcPatternCreate();
    if (pat == NULL)
        return NULL;

    FcPatternAddString(pat, FC_FAMILY, (const FcChar8 *)family_name);

    os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_SCALABLE, FC_PIXEL_SIZE,
                          (char *)NULL);
    if (os == NULL) {
        FcPatternDestroy(pat);
        return NULL;
    }

    fs = FcFontList((FcConfig *)NULL, pat, os);
    FcObjectSetDestroy(os);
    FcPatternDestroy(pat);

    if (fs == NULL || fs->nfont <= 0) {
        if (fs != NULL)
            FcFontSetDestroy(fs);
        return NULL;
    }

    list = (DtFontList *)calloc(1, sizeof(DtFontList));
    if (list == NULL) {
        FcFontSetDestroy(fs);
        return NULL;
    }

    list->fonts = (DtFontInfo *)calloc((size_t)fs->nfont, sizeof(DtFontInfo));
    if (list->fonts == NULL) {
        free(list);
        FcFontSetDestroy(fs);
        return NULL;
    }

    seen = (char **)calloc((size_t)fs->nfont, sizeof(char *));
    if (seen == NULL) {
        free(list->fonts);
        free(list);
        FcFontSetDestroy(fs);
        return NULL;
    }
    seen_count = 0;

    for (i = 0; i < fs->nfont; i++) {
        FcChar8 *family = NULL;
        FcBool   scalable = FcFalse;
        double   pxsize = 0.0;
        int      pixel_size;
        char    *family_dup;
        char    *fc_dup;

        FcPatternGetString(fs->fonts[i], FC_FAMILY, 0, &family);
        FcPatternGetBool(fs->fonts[i], FC_SCALABLE, 0, &scalable);
        FcPatternGetDouble(fs->fonts[i], FC_PIXEL_SIZE, 0, &pxsize);

        if (family == NULL)
            continue;

        family_dup = strdup((const char *)family);
        if (family_dup == NULL)
            continue;

        pixel_size = (scalable != FcFalse) ? 0 : (int)(pxsize + 0.5);

        snprintf(sizebuf, sizeof(sizebuf), "%d", pixel_size);
        snprintf(unique_key, sizeof(unique_key), "%s:%s",
                 family_dup, sizebuf);

        if (is_duplicate_family(seen, seen_count, unique_key)) {
            free(family_dup);
            continue;
        }
        seen[seen_count++] = strdup(unique_key);

        fc_dup = strdup(family_dup);

        list->fonts[list->count].family_name = family_dup;
        list->fonts[list->count].full_name   = strdup(family_dup);
        list->fonts[list->count].xlfd        = NULL;
        list->fonts[list->count].fc_pattern  = fc_dup;
        list->fonts[list->count].is_scalable = (scalable != FcFalse) ? 1 : 0;
        list->fonts[list->count].pixel_size  = pixel_size;
        list->fonts[list->count].source      = DtFontEnumFontconfig;
        list->count++;
    }

    for (i = 0; i < seen_count; i++)
        free(seen[i]);
    free(seen);
    FcFontSetDestroy(fs);

    if (list->count == 0) {
        free(list->fonts);
        free(list);
        return NULL;
    }

    return list;
}

/*
 * Merge an FC list and an X11 list, preferring FC entries for
 * duplicate family names. Result is allocated; the inputs are NOT
 * freed (caller's responsibility).
 */
DtFontList *
DtMergeFontLists(DtFontList *fc, DtFontList *x11)
{
    DtFontList *result;
    int         total;
    int         i;
    char      **seen;
    int         seen_count;

    if (fc == NULL && x11 == NULL)
        return NULL;
    if (fc == NULL)
        return build_list_from_xlfd_no_free(x11);
    if (x11 == NULL) {
        result = (DtFontList *)calloc(1, sizeof(DtFontList));
        if (result == NULL)
            return NULL;
        result->fonts = (DtFontInfo *)calloc((size_t)fc->count,
                                             sizeof(DtFontInfo));
        if (result->fonts == NULL) {
            free(result);
            return NULL;
        }
        seen = (char **)calloc((size_t)fc->count, sizeof(char *));
        if (seen == NULL) {
            free(result->fonts);
            free(result);
            return NULL;
        }
        seen_count = 0;
        for (i = 0; i < fc->count; i++) {
            char *fam = strdup(fc->fonts[i].family_name ?
                               fc->fonts[i].family_name : "");
            if (fam == NULL)
                continue;
            if (is_duplicate_family(seen, seen_count, fam)) {
                free(fam);
                continue;
            }
            seen[seen_count++] = fam;
            result->fonts[result->count++] = fc->fonts[i];
        }
        free(seen);
        return result;
    }

    total = fc->count + x11->count;
    result = (DtFontList *)calloc(1, sizeof(DtFontList));
    if (result == NULL)
        return NULL;

    result->fonts = (DtFontInfo *)calloc((size_t)total, sizeof(DtFontInfo));
    if (result->fonts == NULL) {
        free(result);
        return NULL;
    }

    seen = (char **)calloc((size_t)total, sizeof(char *));
    if (seen == NULL) {
        free(result->fonts);
        free(result);
        return NULL;
    }
    seen_count = 0;

    /* Step 1: copy FC entries first, recording their families. */
    for (i = 0; i < fc->count; i++) {
        char *fam = strdup(fc->fonts[i].family_name ?
                           fc->fonts[i].family_name : "");
        if (fam == NULL)
            continue;
        if (is_duplicate_family(seen, seen_count, fam)) {
            free(fam);
            continue;
        }
        seen[seen_count++] = fam;
        result->fonts[result->count++] = fc->fonts[i];
    }

    /* Step 2: copy X11 entries, skipping families already in the result. */
    for (i = 0; i < x11->count; i++) {
        char *fam = strdup(x11->fonts[i].family_name ?
                           x11->fonts[i].family_name : "");
        if (fam == NULL)
            continue;
        if (is_duplicate_family(seen, seen_count, fam)) {
            free(fam);
            continue;
        }
        seen[seen_count++] = fam;
        result->fonts[result->count++] = x11->fonts[i];
    }

    free(seen);

    return result;
}

/*
 * Local copy helper for the X11-only path in DtMergeFontLists. We
 * deep-copy the input list so the caller can free the original.
 */
static DtFontList *
build_list_from_xlfd_no_free(DtFontList *src)
{
    DtFontList *dst;
    int         i;

    if (src == NULL || src->count <= 0)
        return NULL;

    dst = (DtFontList *)calloc(1, sizeof(DtFontList));
    if (dst == NULL)
        return NULL;

    dst->fonts = (DtFontInfo *)calloc((size_t)src->count, sizeof(DtFontInfo));
    if (dst->fonts == NULL) {
        free(dst);
        return NULL;
    }

    for (i = 0; i < src->count; i++) {
        if (src->fonts[i].family_name != NULL)
            dst->fonts[dst->count].family_name =
                strdup(src->fonts[i].family_name);
        if (src->fonts[i].full_name != NULL)
            dst->fonts[dst->count].full_name =
                strdup(src->fonts[i].full_name);
        if (src->fonts[i].xlfd != NULL)
            dst->fonts[dst->count].xlfd =
                strdup(src->fonts[i].xlfd);
        if (src->fonts[i].fc_pattern != NULL)
            dst->fonts[dst->count].fc_pattern =
                strdup(src->fonts[i].fc_pattern);
        dst->fonts[dst->count].is_scalable = src->fonts[i].is_scalable;
        dst->fonts[dst->count].pixel_size  = src->fonts[i].pixel_size;
        dst->fonts[dst->count].source      = src->fonts[i].source;
        dst->count++;
    }

    return dst;
}

#endif /* USE_XFT */
