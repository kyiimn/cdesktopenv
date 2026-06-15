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

#ifndef _DTXD2A_OPTIONS_H
#define _DTXD2A_OPTIONS_H

#include <glib.h>

typedef struct {
    gboolean verbose;
    gboolean retain;
    gchar   *output_dir;
    gchar   *icon_output_dir;
    gchar   *cache_file;
    gboolean force_rebuild;
    gboolean dry_run;
} dtxdg2appmgr_Options;

dtxdg2appmgr_Options *dtxdg2appmgr_parse_options(int *argc, char ***argv);
void                   dtxdg2appmgr_print_usage(const char *progname);
void                   dtxdg2appmgr_options_free(dtxdg2appmgr_Options *opts);

#endif /* _DTXD2A_OPTIONS_H */