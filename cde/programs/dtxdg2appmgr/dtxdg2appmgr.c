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

#ifdef HAVE_CONFIG_H
#include <cde_config.h>
#endif

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "desktop_parser.h"
#include "exec_parser.h"
#include "icon_resolver.h"
#include "xpm_converter.h"
#include "dt_writer.h"
#include "cache.h"
#include "path_scanner.h"
#include "category_mapper.h"

#define DTXDG2APPMGR_VERSION "0.1.0"

int
main(int argc, char **argv)
{
    dtxdg2appmgr_Options *opts;

    opts = dtxdg2appmgr_parse_options(&argc, &argv);
    if (opts == NULL) {
        dtxdg2appmgr_print_usage(argv[0]);
        return 1;
    }

    if (opts->verbose) {
        printf("dtxdg2appmgr version %s\n", DTXDG2APPMGR_VERSION);
    }

    /* TODO: implement main workflow */

    dtxdg2appmgr_options_free(opts);
    return 0;
}