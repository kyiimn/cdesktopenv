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
#include <stdlib.h>
#include <string.h>
#include "cache.h"

static GHashTable *cache_table = NULL;

void
dtxdg2appmgr_cache_load(const char *cache_file)
{
    /* TODO: implement */
    if (cache_table == NULL)
        cache_table = g_hash_table_new_full(g_str_hash, g_str_equal,
                                            g_free, NULL);
}

void
dtxdg2appmgr_cache_save(const char *cache_file)
{
    /* TODO: implement */
}

int
dtxdg2appmgr_cache_is_valid(const char *desktop_path, const char *dt_path)
{
    /* TODO: implement */
    return 0;
}

void
dtxdg2appmgr_cache_entry_free(dtxdg2appmgr_CacheEntry *entry)
{
    /* TODO: implement */
    if (entry == NULL) return;
    free(entry->desktop_path);
    free(entry->dt_path);
    free(entry);
}