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

#ifndef _DTXD2A_DT_WRITER_H
#define _DTXD2A_DT_WRITER_H

#include <glib.h>
#include "desktop_parser.h"

gboolean dtxdg2appmgr_write_dt_file(const dtxdg2appmgr_DesktopEntry *entry,
                                     const gchar *action_name,
                                     const gchar *exec_string,
                                     const gchar *icon_basename,
                                     const gchar *dt_output_dir,
                                     GError **error);

gboolean dtxdg2appmgr_write_appmanager_stub(const gchar *group_dir,
                                            const gchar *action_name,
                                            GError **error);

gboolean dtxdg2appmgr_write_group_dt(const gchar *group_name,
                                     const gchar *dt_output_dir,
                                     GError **error);

#endif /* _DTXD2A_DT_WRITER_H */