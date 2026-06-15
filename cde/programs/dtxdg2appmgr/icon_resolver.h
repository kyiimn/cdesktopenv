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

typedef struct {
    char *icon_path;
    char *icon_name;
    int   size;
} dtxdg2appmgr_IconResolution;

extern dtxdg2appmgr_IconResolution *dtxdg2appmgr_icon_resolve(const char *icon_name,
                                                              int size);
extern void dtxdg2appmgr_icon_resolution_free(dtxdg2appmgr_IconResolution *res);

#endif /* _DTXD2A_ICON_RESOLVER_H */