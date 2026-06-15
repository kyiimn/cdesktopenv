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

#ifndef _DTXD2A_XPM_CONVERTER_H
#define _DTXD2A_XPM_CONVERTER_H

#include <glib.h>

typedef struct {
    gint tiny;    /* 16x16 */
    gint medium;  /* 32x32 */
    gint large;   /* 48x48 */
} dtxdg2appmgr_IconSizes;

gboolean dtxdg2appmgr_convert_to_xpm_set(const gchar *src_path,
                                         const gchar *base_name,
                                         const gchar *output_dir,
                                         GError **error);

gboolean dtxdg2appmgr_check_tool_available(const gchar *tool_name);

gchar *dtxdg2appmgr_generate_pm_basename(const gchar *app_name);

#endif /* _DTXD2A_XPM_CONVERTER_H */