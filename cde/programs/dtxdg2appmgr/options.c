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

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/usr/dt"
#endif

#ifndef CDE_LOGFILES_TOP
#define CDE_LOGFILES_TOP "/var/dt"
#endif

static GOptionEntry option_entries[] = {
    { "verbose",      'v', 0, G_OPTION_ARG_NONE,   NULL, "Verbose output",                              NULL },
    { "retain",       'r', 0, G_OPTION_ARG_NONE,   NULL, "Retain temporary files",                      NULL },
    { "output-dir",   'o', 0, G_OPTION_ARG_STRING,  NULL, "CDE appconfig output directory",              "DIR" },
    { "icon-dir",     'i', 0, G_OPTION_ARG_STRING,  NULL, "Icon output directory",                       "DIR" },
    { "cache-file",   'c', 0, G_OPTION_ARG_STRING,  NULL, "Cache file path",                             "FILE" },
    { "force",        'f', 0, G_OPTION_ARG_NONE,   NULL, "Force rebuild ignoring cache",                NULL },
    { "dry-run",      '\0', 0, G_OPTION_ARG_NONE,   NULL, "Show what would be done without writing files", NULL },
    { NULL }
};

dtxdg2appmgr_Options *
dtxdg2appmgr_parse_options(int *argc, char ***argv)
{
    GError *error = NULL;
    GOptionContext *context;
    dtxdg2appmgr_Options *opts;
    gboolean verbose = FALSE;
    gboolean retain = FALSE;
    gchar *output_dir = NULL;
    gchar *icon_output_dir = NULL;
    gchar *cache_file = NULL;
    gboolean force_rebuild = FALSE;
    gboolean dry_run = FALSE;

    option_entries[0].arg_data = &verbose;
    option_entries[1].arg_data = &retain;
    option_entries[2].arg_data = &output_dir;
    option_entries[3].arg_data = &icon_output_dir;
    option_entries[4].arg_data = &cache_file;
    option_entries[5].arg_data = &force_rebuild;
    option_entries[6].arg_data = &dry_run;

    context = g_option_context_new("- convert XDG .desktop entries to CDE actions");
    g_option_context_add_main_entries(context, option_entries, NULL);

    if (!g_option_context_parse(context, argc, argv, &error)) {
        g_printerr("%s\n", error->message);
        g_error_free(error);
        g_option_context_free(context);
        return NULL;
    }

    g_option_context_free(context);

    opts = g_new0(dtxdg2appmgr_Options, 1);
    opts->verbose = verbose;
    opts->retain = retain;
    opts->output_dir = output_dir ? output_dir : g_strdup(CDE_INSTALLATION_TOP "/appconfig/appmanager");
    opts->icon_output_dir = icon_output_dir ? icon_output_dir : g_strdup(CDE_INSTALLATION_TOP "/appconfig/icons/C");
    opts->cache_file = cache_file ? cache_file : g_strdup(CDE_LOGFILES_TOP "/xdg-cache.db");
    opts->force_rebuild = force_rebuild;
    opts->dry_run = dry_run;

    return opts;
}

void
dtxdg2appmgr_print_usage(const char *progname)
{
    fprintf(stderr,
        "Usage: %s [OPTIONS]\n"
        "\n"
        "Convert XDG .desktop entries into CDE Application Manager actions.\n"
        "\n"
        "Options:\n"
        "  -v, --verbose       Verbose output\n"
        "  -r, --retain        Retain temporary files\n"
        "  -o, --output-dir DIR   CDE appconfig output directory\n"
        "                          (default: " CDE_INSTALLATION_TOP "/appconfig/appmanager)\n"
        "  -i, --icon-dir DIR     Icon output directory\n"
        "                          (default: " CDE_INSTALLATION_TOP "/appconfig/icons/C)\n"
        "  -c, --cache-file FILE  Cache file path\n"
        "                          (default: " CDE_LOGFILES_TOP "/xdg-cache.db)\n"
        "  -f, --force         Force rebuild ignoring cache\n"
        "      --dry-run       Show what would be done without writing files\n"
        "\n"
        "Environment:\n"
        "  DTUSERSESSION       If set, output-dir defaults to\n"
        "                      " CDE_INSTALLATION_TOP "/appconfig/appmanager/$DTUSERSESSION\n",
        progname);
}

void
dtxdg2appmgr_options_free(dtxdg2appmgr_Options *opts)
{
    if (opts == NULL) return;
    g_free(opts->output_dir);
    g_free(opts->icon_output_dir);
    g_free(opts->cache_file);
    g_free(opts);
}