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

typedef struct {
    int tiny;
    int small;
    int medium;
    int large;
} dtxdg2appmgr_IconSizes;

extern int dtxdg2appmgr_convert_to_xpm_set(const char *icon_path,
                                            const char *output_dir,
                                            const dtxdg2appmgr_IconSizes *sizes);
extern int dtxdg2appmgr_check_tool_available(const char *tool_name);
extern char *dtxdg2appmgr_generate_pm_basename(const char *desktop_name);

#endif /* _DTXD2A_XPM_CONVERTER_H */