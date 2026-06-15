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
 * License along with these libraries and programs; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <cde_config.h>
#endif

#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "path_scanner.h"
#include "desktop_parser.h"

gchar **
dtxdg2appmgr_xdg_get_data_dirs(void)
{
    GPtrArray *dirs;
    const gchar *xdg_data_dirs;
    const gchar *home;
    gchar **sys_dirs;
    gchar **result;
    int i;

    dirs = g_ptr_array_new();

    home = g_get_home_dir();
    if (home != NULL)
    {
        gchar *user_dir = g_build_filename(home, ".local", "share", NULL);
        g_ptr_array_add(dirs, user_dir);
    }

    xdg_data_dirs = g_getenv("XDG_DATA_DIRS");
    if (xdg_data_dirs != NULL && xdg_data_dirs[0] != '\0')
    {
        sys_dirs = g_strsplit(xdg_data_dirs, ":", -1);
    }
    else
    {
        sys_dirs = g_strsplit("/usr/local/share:/usr/share", ":", -1);
    }

    for (i = 0; sys_dirs[i] != NULL; i++)
    {
        if (sys_dirs[i][0] != '\0')
        {
            g_ptr_array_add(dirs, g_strdup(sys_dirs[i]));
        }
    }

    g_strfreev(sys_dirs);

    g_ptr_array_add(dirs, NULL);

    result = (gchar **)g_ptr_array_free(dirs, FALSE);
    return result;
}

/*
 * Recursively scan a directory for .desktop files.
 * Adds parsed entries to the list if desktop_should_include() returns TRUE.
 * Skips hidden files, symlinks to directories, and already-visited paths
 * to prevent infinite recursion through symlink loops.
 */
static void
scan_directory_recursive(const gchar *dir_path,
                        const gchar *lang,
                        GSList **entries,
                        GHashTable *visited)
{
    GDir *dir;
    const gchar *name;
    gchar *full_path;
    dtxdg2appmgr_DesktopEntry *entry;

    dir = g_dir_open(dir_path, 0, NULL);
    if (dir == NULL)
        return;

    while ((name = g_dir_read_name(dir)) != NULL)
    {
        if (name[0] == '.')
            continue;

        full_path = g_build_filename(dir_path, name, NULL);

        if (g_file_test(full_path, G_FILE_TEST_IS_DIR))
        {
            /* Skip symlinks to directories — don't follow for security */
            if (g_file_test(full_path, G_FILE_TEST_IS_SYMLINK))
            {
                g_free(full_path);
                continue;
            }

            gchar *canonical = realpath(full_path, NULL);
            if (canonical == NULL)
            {
                g_free(full_path);
                continue;
            }

            if (g_hash_table_contains(visited, canonical))
            {
                g_free(canonical);
                g_free(full_path);
                continue;
            }

            g_hash_table_add(visited, canonical);
            scan_directory_recursive(full_path, lang, entries, visited);
            g_free(full_path);
        }
        else if (g_str_has_suffix(name, ".desktop"))
        {
            entry = dtxdg2appmgr_desktop_parse(full_path, lang);
            g_free(full_path);

            if (entry == NULL)
                continue;

            if (dtxdg2appmgr_desktop_should_include(entry))
            {
                *entries = g_slist_prepend(*entries, entry);
            }
            else
            {
                dtxdg2appmgr_desktop_entry_free(entry);
            }
        }
        else
        {
            g_free(full_path);
        }
    }

    g_dir_close(dir);
}

GSList *
dtxdg2appmgr_xdg_scan_applications(gchar **data_dirs)
{
    GSList *entries = NULL;
    gchar *apps_dir;
    gchar *lang;
    GHashTable *visited;
    int i;

    if (data_dirs == NULL)
        return NULL;

    lang = dtxdg2appmgr_xdg_get_language();
    visited = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    for (i = 0; data_dirs[i] != NULL; i++)
    {
        apps_dir = g_build_filename(data_dirs[i], "applications", NULL);
        scan_directory_recursive(apps_dir, lang, &entries, visited);
        g_free(apps_dir);
    }

    g_hash_table_destroy(visited);
    g_free(lang);
    return entries;
}

gchar *
dtxdg2appmgr_xdg_get_language(void)
{
    const gchar *lang;
    gchar *result;
    gchar *underscore;

    lang = g_getenv("LANG");

    if (lang == NULL || lang[0] == '\0' || g_strcmp0(lang, "C") == 0)
        return g_strdup("en");

    result = g_strdup(lang);

    underscore = strchr(result, '_');
    if (underscore != NULL)
        *underscore = '\0';

    if (result[0] == '\0')
    {
        g_free(result);
        return g_strdup("en");
    }

    return result;
}

void
dtxdg2appmgr_xdg_free_data_dirs(gchar **dirs)
{
    g_strfreev(dirs);
}

void
dtxdg2appmgr_xdg_free_entry_list(GSList *entries)
{
    g_slist_free_full(entries, (GDestroyNotify)dtxdg2appmgr_desktop_entry_free);
}