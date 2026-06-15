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

#ifdef HAVE_CONFIG_H
#include <cde_config.h>
#endif

#include <glib.h>
#include <string.h>
#include "icon_resolver.h"

static const gchar *extensions[] = { ".png", ".svg", ".xpm", ".jpg", ".jpeg", NULL };
static const gchar *ext_formats[] = { "png", "svg", "xpm", "jpg", "jpg", NULL };
static const gchar *sizes[] = { "48x48", "32x32", "16x16", NULL };
static const gchar *size_subdirs[] = { "apps", NULL };

static gboolean
try_path(const gchar *path, dtxdg2appmgr_IconResolution *result)
{
    if (!g_file_test(path, G_FILE_TEST_IS_REGULAR))
        return FALSE;

    result->absolute_path = g_strdup(path);

    for (gint i = 0; extensions[i] != NULL; i++) {
        if (g_str_has_suffix(path, extensions[i])) {
            result->format = g_strdup(ext_formats[i]);
            return TRUE;
        }
    }

    result->format = g_strdup("unknown");
    return TRUE;
}

static gboolean
search_in_dir(const gchar *dir, const gchar *icon_name,
              dtxdg2appmgr_IconResolution *result)
{
    gchar *path;

    path = g_build_filename(dir, icon_name, NULL);
    if (try_path(path, result)) {
        g_free(path);
        return TRUE;
    }
    g_free(path);

    for (gint ei = 0; extensions[ei] != NULL; ei++) {
        gchar *name_with_ext = g_strconcat(icon_name, extensions[ei], NULL);
        path = g_build_filename(dir, name_with_ext, NULL);
        g_free(name_with_ext);
        if (try_path(path, result)) {
            g_free(path);
            return TRUE;
        }
        g_free(path);
    }

    for (gint si = 0; sizes[si] != NULL; si++) {
        path = g_build_filename(dir, sizes[si], icon_name, NULL);
        if (try_path(path, result)) {
            g_free(path);
            return TRUE;
        }
        g_free(path);

        for (gint ei = 0; extensions[ei] != NULL; ei++) {
            gchar *name_with_ext = g_strconcat(icon_name, extensions[ei], NULL);
            path = g_build_filename(dir, sizes[si], name_with_ext, NULL);
            g_free(name_with_ext);
            if (try_path(path, result)) {
                g_free(path);
                return TRUE;
            }
            g_free(path);
        }

        for (gint sdi = 0; size_subdirs[sdi] != NULL; sdi++) {
            path = g_build_filename(dir, sizes[si], size_subdirs[sdi], icon_name, NULL);
            if (try_path(path, result)) {
                g_free(path);
                return TRUE;
            }
            g_free(path);

            for (gint ei = 0; extensions[ei] != NULL; ei++) {
                gchar *name_with_ext = g_strconcat(icon_name, extensions[ei], NULL);
                path = g_build_filename(dir, sizes[si], size_subdirs[sdi],
                                        name_with_ext, NULL);
                g_free(name_with_ext);
                if (try_path(path, result)) {
                    g_free(path);
                    return TRUE;
                }
                g_free(path);
            }
        }
    }

    return FALSE;
}

gboolean
dtxdg2appmgr_icon_resolve(const gchar *icon_name, const gchar **data_dirs,
                           dtxdg2appmgr_IconResolution *result)
{
    const gchar *home;

    if (icon_name == NULL || result == NULL)
        return FALSE;

    result->absolute_path = NULL;
    result->format = NULL;

    if (icon_name[0] == '/') {
        result->absolute_path = g_strdup(icon_name);
        for (gint i = 0; extensions[i] != NULL; i++) {
            if (g_str_has_suffix(icon_name, extensions[i])) {
                result->format = g_strdup(ext_formats[i]);
                return TRUE;
            }
        }
        result->format = g_strdup("unknown");
        return TRUE;
    }

    home = g_get_home_dir();

    if (home != NULL && home[0] != '\0') {
        gchar *dir = g_build_filename(home, ".local", "share", "icons", NULL);
        gboolean found = search_in_dir(dir, icon_name, result);
        g_free(dir);
        if (found)
            return TRUE;
    }

    if (data_dirs != NULL) {
        for (gint i = 0; data_dirs[i] != NULL; i++) {
            gchar *dir = g_build_filename(data_dirs[i], "icons", NULL);
            gboolean found = search_in_dir(dir, icon_name, result);
            g_free(dir);
            if (found)
                return TRUE;
        }
    }

    if (home != NULL && home[0] != '\0') {
        gchar *dir = g_build_filename(home, ".icons", NULL);
        gboolean found = search_in_dir(dir, icon_name, result);
        g_free(dir);
        if (found)
            return TRUE;
    }

    if (search_in_dir("/usr/share/pixmaps", icon_name, result))
        return TRUE;

    return FALSE;
}

void
dtxdg2appmgr_icon_resolution_free(dtxdg2appmgr_IconResolution *res)
{
    if (res == NULL) return;
    g_free(res->absolute_path);
    g_free(res->format);
    res->absolute_path = NULL;
    res->format = NULL;
}