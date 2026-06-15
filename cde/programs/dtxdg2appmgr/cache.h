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

#ifndef _DTXD2A_CACHE_H
#define _DTXD2A_CACHE_H

#include <glib.h>

typedef struct {
    gchar *desktop_path;   /* absolute path of .desktop file */
    gchar *dt_path;        /* path of generated .dt file      */
    glong  mtime;          /* mtime of .desktop file when cached */
} dtxdg2appmgr_CacheEntry;

GHashTable *dtxdg2appmgr_cache_load(const gchar *cache_file);
gboolean    dtxdg2appmgr_cache_save(GHashTable *cache, const gchar *cache_file);
gboolean    dtxdg2appmgr_cache_is_valid(GHashTable *cache, const gchar *desktop_path);
void        dtxdg2appmgr_cache_entry_free(dtxdg2appmgr_CacheEntry *entry);
void        dtxdg2appmgr_cache_update(GHashTable *cache, const gchar *desktop_path,
                                      const gchar *dt_path, glong mtime);
void        dtxdg2appmgr_cache_free(GHashTable *cache);

#endif /* _DTXD2A_CACHE_H */