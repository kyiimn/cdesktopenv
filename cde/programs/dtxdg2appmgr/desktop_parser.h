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

#ifndef _DTXD2A_DESKTOP_PARSER_H
#define _DTXD2A_DESKTOP_PARSER_H

#include <glib.h>
#include <time.h>

typedef struct {
    gchar  *id;                /* filename, e.g. "firefox.desktop" */
    gchar  *path;               /* full path */
    gchar  *name;               /* Name[lang] or Name */
    gchar  *generic_name;       /* GenericName[lang] or GenericName */
    gchar  *comment;            /* Comment[lang] or Comment */
    gchar  *exec;               /* raw Exec line */
    gchar  *icon;               /* Icon name or path */
    gchar **categories;         /* semicolon-separated Categories array */
    gchar  *type;               /* Type, default "Application" */
    gchar  *terminal;           /* Terminal, default "false" */
    gboolean no_display;       /* NoDisplay, default FALSE */
    gboolean hidden;            /* Hidden, default FALSE */
    gchar  *startup_wm_class;   /* StartupWMClass */
    time_t  mtime;              /* file mtime for cache */
    gchar  *try_exec;           /* TryExec path */
} dtxdg2appmgr_DesktopEntry;

extern dtxdg2appmgr_DesktopEntry *dtxdg2appmgr_desktop_parse(const gchar *path,
                                                              const gchar *lang);
extern void  dtxdg2appmgr_desktop_entry_free(dtxdg2appmgr_DesktopEntry *entry);
extern gboolean dtxdg2appmgr_desktop_should_include(const dtxdg2appmgr_DesktopEntry *entry);

#endif /* _DTXD2A_DESKTOP_PARSER_H */