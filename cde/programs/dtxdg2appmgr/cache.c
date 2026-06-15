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
#include <glib/gstdio.h>
#include <sys/stat.h>
#include <string.h>
#include "cache.h"

static void cache_entry_free_cb(gpointer data)
{
    dtxdg2appmgr_cache_entry_free((dtxdg2appmgr_CacheEntry *)data);
}

GHashTable *
dtxdg2appmgr_cache_load(const gchar *cache_file)
{
    GHashTable *cache;
    GKeyFile   *kf;
    gchar     **groups;
    gsize       n_groups;
    GError     *err = NULL;

    cache = g_hash_table_new_full(g_str_hash, g_str_equal,
                                  g_free, cache_entry_free_cb);

    if (!g_file_test(cache_file, G_FILE_TEST_EXISTS))
        return cache;

    kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, cache_file, G_KEY_FILE_NONE, &err)) {
        g_warning("dtxdg2appmgr: failed to load cache file %s: %s",
                  cache_file, err->message);
        g_error_free(err);
        g_key_file_free(kf);
        return cache;
    }

    groups = g_key_file_get_groups(kf, &n_groups);
    for (gsize i = 0; i < n_groups; i++) {
        dtxdg2appmgr_CacheEntry *entry;
        gchar *dt_path_val;
        gint64 mtime_val;

        dt_path_val = g_key_file_get_string(kf, groups[i], "dt_path", NULL);
        if (dt_path_val == NULL)
            continue;

        mtime_val = g_key_file_get_int64(kf, groups[i], "mtime", NULL);

        entry = g_new0(dtxdg2appmgr_CacheEntry, 1);
        entry->desktop_path = g_strdup(groups[i]);
        entry->dt_path      = dt_path_val;
        entry->mtime        = (glong)mtime_val;

        g_hash_table_insert(cache, g_strdup(groups[i]), entry);
    }

    g_strfreev(groups);
    g_key_file_free(kf);
    return cache;
}

gboolean
dtxdg2appmgr_cache_save(GHashTable *cache, const gchar *cache_file)
{
    GKeyFile    *kf;
    GHashTableIter iter;
    gpointer     key, value;

    if (cache == NULL)
        return FALSE;

    kf = g_key_file_new();

    g_hash_table_iter_init(&iter, cache);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        const gchar *group = (const gchar *)key;
        dtxdg2appmgr_CacheEntry *entry = (dtxdg2appmgr_CacheEntry *)value;

        g_key_file_set_string(kf, group, "dt_path", entry->dt_path);
        g_key_file_set_int64(kf, group, "mtime", (gint64)entry->mtime);
    }

    {
        GError *err = NULL;
        if (!g_key_file_save_to_file(kf, cache_file, &err)) {
            g_warning("dtxdg2appmgr: failed to save cache file %s: %s",
                      cache_file, err->message);
            g_error_free(err);
            g_key_file_free(kf);
            return FALSE;
        }
    }

    g_chmod(cache_file, 0644);
    g_key_file_free(kf);
    return TRUE;
}

gboolean
dtxdg2appmgr_cache_is_valid(GHashTable *cache, const gchar *desktop_path)
{
    dtxdg2appmgr_CacheEntry *entry;
    struct stat st;

    if (cache == NULL || desktop_path == NULL)
        return FALSE;

    entry = g_hash_table_lookup(cache, desktop_path);
    if (entry == NULL)
        return FALSE;

    if (stat(desktop_path, &st) != 0)
        return FALSE;

    return (entry->mtime == (glong)st.st_mtime);
}

void
dtxdg2appmgr_cache_entry_free(dtxdg2appmgr_CacheEntry *entry)
{
    if (entry == NULL)
        return;
    g_free(entry->desktop_path);
    g_free(entry->dt_path);
    g_free(entry);
}

void
dtxdg2appmgr_cache_update(GHashTable *cache, const gchar *desktop_path,
                           const gchar *dt_path, glong mtime)
{
    dtxdg2appmgr_CacheEntry *entry;

    if (cache == NULL || desktop_path == NULL)
        return;

    entry = g_new0(dtxdg2appmgr_CacheEntry, 1);
    entry->desktop_path = g_strdup(desktop_path);
    entry->dt_path      = g_strdup(dt_path);
    entry->mtime        = mtime;

    g_hash_table_insert(cache, g_strdup(desktop_path), entry);
}

void
dtxdg2appmgr_cache_free(GHashTable *cache)
{
    if (cache == NULL)
        return;
    g_hash_table_destroy(cache);
}