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

#ifndef _DTXD2A_ICON_RESOLVER_H
#define _DTXD2A_ICON_RESOLVER_H

#include <glib.h>

typedef struct {
    gchar *absolute_path;  /* found icon's absolute path */
    gchar *format;          /* extension: "png", "svg", "xpm", "jpg" */
} dtxdg2appmgr_IconResolution;

/*
 * dtxdg2appmgr_icon_resolve - find an icon file using the XDG Icon Theme
 * Specification search algorithm.
 *
 * @icon_name:  icon name from .desktop file (e.g. "firefox")
 * @data_dirs:  NULL-terminated array of XDG data directories
 *              (e.g. from XDG_DATA_DIRS or default list)
 * @result:     pointer to IconResolution struct to fill in
 *
 * Returns: TRUE if icon found, FALSE otherwise.
 *          On TRUE, result->absolute_path and result->format are set
 *          to heap-allocated strings the caller must free with
 *          dtxdg2appmgr_icon_resolution_free().
 */
extern gboolean dtxdg2appmgr_icon_resolve(const gchar *icon_name,
                                          const gchar **data_dirs,
                                          dtxdg2appmgr_IconResolution *result);

/*
 * dtxdg2appmgr_icon_resolution_free - free heap memory inside an
 * IconResolution struct. Does NOT free the struct itself.
 */
extern void dtxdg2appmgr_icon_resolution_free(dtxdg2appmgr_IconResolution *res);

#endif /* _DTXD2A_ICON_RESOLVER_H */