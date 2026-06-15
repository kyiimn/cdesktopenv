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

#ifndef _DTXD2A_PATH_SCANNER_H
#define _DTXD2A_PATH_SCANNER_H

#include <glib.h>

/*
 * Returns a NULL-terminated array of XDG data directories.
 * The user directory ($HOME/.local/share) is always first (highest
 * priority), followed by directories from $XDG_DATA_DIRS or the
 * default /usr/local/share:/usr/share.
 * Caller must free with dtxdg2appmgr_xdg_free_data_dirs().
 */
gchar **dtxdg2appmgr_xdg_get_data_dirs(void);

/*
 * Scans the "applications" subdirectory of each data directory for
 * .desktop files.  Returns a GSList of dtxdg2appmgr_DesktopEntry*
 * pointers.  Hidden files (starting with '.') are skipped.
 * Caller must free with dtxdg2appmgr_xdg_free_entry_list().
 */
GSList *dtxdg2appmgr_xdg_scan_applications(gchar **data_dirs);

/*
 * Returns the current language code normalized from the $LANG
 * environment variable (e.g. "ko_KR.UTF-8" -> "ko", "en_US" -> "en",
 * "C" or unset -> "en").  Caller must free with g_free().
 */
gchar *dtxdg2appmgr_xdg_get_language(void);

/*
 * Free a data-dirs array previously returned by
 * dtxdg2appmgr_xdg_get_data_dirs().
 */
void dtxdg2appmgr_xdg_free_data_dirs(gchar **dirs);

/*
 * Free an entry list previously returned by
 * dtxdg2appmgr_xdg_scan_applications().
 */
void dtxdg2appmgr_xdg_free_entry_list(GSList *entries);

#endif /* _DTXD2A_PATH_SCANNER_H */