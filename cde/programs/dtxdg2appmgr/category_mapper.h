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

#ifndef _DTXD2A_CATEGORY_MAPPER_H
#define _DTXD2A_CATEGORY_MAPPER_H

#include <glib.h>

/*
 * Map an XDG category name to a CDE Application Manager group name.
 * Always appends "_XDG" suffix to avoid collisions with built-in
 * CDE group names (Desktop_Apps, Desktop_Tools, Information,
 * System_Admin, Office, Internet, Education, Graphics, System,
 * Games, Media_Tools, TeX).
 *
 * Returns a newly allocated string that the caller must g_free().
 */
gchar *dtxdg2appmgr_category_to_group(const gchar *category);

/*
 * Extract the primary (first) category from a semicolon-separated
 * XDG Categories list. Strips desktop-specific prefixes (X-*, GTK,
 * Motif, GNOME, Qt) per desktop2dt algorithm.
 *
 * Returns a newly allocated string that the caller must g_free().
 */
gchar *dtxdg2appmgr_get_primary_category(const gchar *categories);

/*
 * Check whether a file named "name" already exists in "group_dir",
 * indicating a collision with an existing CDE Application Manager
 * entry.
 *
 * Returns TRUE if collision detected, FALSE otherwise.
 */
gboolean dtxdg2appmgr_is_collision(const gchar *name, const gchar *group_dir);

/*
 * Sanitize an arbitrary string into a valid CDE .dt ACTION name.
 * Replaces spaces, hyphens, and non-alphanumeric characters with
 * underscores, collapses consecutive underscores, and strips
 * leading/trailing underscores.
 *
 * Returns a newly allocated string that the caller must g_free().
 */
gchar *dtxdg2appmgr_sanitize_action_name(const gchar *name);

#endif /* _DTXD2A_CATEGORY_MAPPER_H */