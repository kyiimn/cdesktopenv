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
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

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

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/usr/dt"
#endif

#ifndef CDE_CONFIGURATION_TOP
#define CDE_CONFIGURATION_TOP "/etc/dt"
#endif

static volatile sig_atomic_t interrupted = 0;

static void
sigint_handler(int sig)
{
    (void)sig;
    interrupted = 1;
}

/*
 * Determine the .dt output directory.
 * If user specified --output-dir, use that.
 * Otherwise, use $HOME/.dt/types/ for user-local .dt files.
 */
static gchar *
get_dt_output_dir(const dtxdg2appmgr_Options *opts)
{
    if (opts->output_dir)
        return g_strdup(opts->output_dir);

    const gchar *home = g_get_home_dir();
    if (home)
        return g_build_filename(home, ".dt", "types", NULL);

    return g_strdup(CDE_CONFIGURATION_TOP "/appconfig/types/C");
}

/*
 * Determine the icon output directory.
 * If user specified --icon-dir, use that.
 * Otherwise, use $HOME/.dt/icons/ for user-local icons.
 */
static gchar *
get_icon_output_dir(const dtxdg2appmgr_Options *opts)
{
    if (opts->icon_output_dir)
        return g_strdup(opts->icon_output_dir);

    const gchar *home = g_get_home_dir();
    if (home)
        return g_build_filename(home, ".dt", "icons", NULL);

    return g_strdup(CDE_INSTALLATION_TOP "/appconfig/icons/C");
}

/*
 * Determine the appmanager output directory.
 * Uses DTUSERSESSION env var for the session subdirectory,
 * falling back to "C" (CDE default).
 */
static gchar *
get_appmanager_dir(const dtxdg2appmgr_Options *opts)
{
    const gchar *session = g_getenv("DTUSERSESSION");
    if (session == NULL || *session == '\0')
        session = "C";

    return g_build_filename(opts->output_dir, session, NULL);
}

/*
 * Ensure a directory exists, creating it if necessary.
 * Returns TRUE on success or if directory already exists.
 */
static gboolean
ensure_dir(const gchar *path, GError **error)
{
    if (g_mkdir_with_parents(path, 0755) != 0) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Failed to create directory '%s': %s", path, g_strerror(errno));
        return FALSE;
    }
    return TRUE;
}

int
main(int argc, char **argv)
{
    dtxdg2appmgr_Options *opts = NULL;
    gchar **data_dirs = NULL;
    gchar *lang = NULL;
    GSList *entries = NULL;
    GHashTable *cache = NULL;
    gchar *dt_output_dir = NULL;
    gchar *icon_output_dir = NULL;
    gchar *appmanager_dir = NULL;
    GHashTable *groups_seen = NULL;   
    gint exit_code = 0;
    GError *error = NULL;
    gint processed = 0;
    gint skipped = 0;
    gint errors = 0;

    /* 1. Set up signal handling */
    {
        struct sigaction sa;
        struct sigaction sa_ign;

        /* Handle SIGINT, SIGTERM, SIGHUP - graceful shutdown */
        sa.sa_handler = sigint_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;  /* NOT SA_RESTART - we want EINTR */
        sigaction(SIGINT,  &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGHUP,  &sa, NULL);

        /* Ignore SIGPIPE (broken pipe from icon conversion child) */
        sa_ign.sa_handler = SIG_IGN;
        sigemptyset(&sa_ign.sa_mask);
        sa_ign.sa_flags = 0;
        sigaction(SIGPIPE, &sa_ign, NULL);
    }

    /* 2. Parse CLI options */
    opts = dtxdg2appmgr_parse_options(&argc, &argv);
    if (opts == NULL) {
        dtxdg2appmgr_print_usage(argv[0]);
        return 1;
    }

    if (opts->verbose) {
        g_print("dtxdg2appmgr version %s\n", DTXDG2APPMGR_VERSION);
    }

    /* 3. Get XDG data directories */
    data_dirs = dtxdg2appmgr_xdg_get_data_dirs();
    if (data_dirs == NULL || data_dirs[0] == NULL) {
        g_printerr("Error: no XDG data directories found\n");
        exit_code = 1;
        goto cleanup;
    }

    if (opts->verbose) {
        gint i;
        g_print("XDG data directories:\n");
        for (i = 0; data_dirs[i] != NULL; i++)
            g_print("  %s\n", data_dirs[i]);
    }

    /* 4. Get language */
    lang = dtxdg2appmgr_xdg_get_language();
    if (opts->verbose)
        g_print("Language: %s\n", lang ? lang : "(null)");

    /* 5. Scan for .desktop files */
    if (opts->verbose)
        g_print("Scanning for .desktop files...\n");

    entries = dtxdg2appmgr_xdg_scan_applications(data_dirs);
    if (entries == NULL && opts->verbose) {
        g_print("No .desktop files found.\n");
    }

    if (opts->verbose)
        g_print("Found %d .desktop files\n", g_slist_length(entries));

    /* 6. Determine output directories */
    dt_output_dir = get_dt_output_dir(opts);
    icon_output_dir = get_icon_output_dir(opts);
    appmanager_dir = get_appmanager_dir(opts);

    if (opts->verbose) {
        g_print("Output directories:\n");
        g_print("  .dt files:    %s\n", dt_output_dir);
        g_print("  icons:       %s\n", icon_output_dir);
        g_print("  appmanager:  %s\n", appmanager_dir);
    }

    
    if (!opts->dry_run) {
        if (!ensure_dir(dt_output_dir, &error)) {
            g_printerr("Error: %s\n", error->message);
            g_error_free(error);
            error = NULL;
            errors++;
        }
        if (!ensure_dir(icon_output_dir, &error)) {
            g_printerr("Error: %s\n", error->message);
            g_error_free(error);
            error = NULL;
            errors++;
        }
    }

    /* 7. Load cache */
    cache = dtxdg2appmgr_cache_load(opts->cache_file);
    if (cache == NULL)
        cache = g_hash_table_new_full(g_str_hash, g_str_equal,
                                       g_free,
                                       (GDestroyNotify)dtxdg2appmgr_cache_entry_free);

    
    groups_seen = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    /* 8. Process each .desktop entry */
    GSList *iter;
    for (iter = entries; iter != NULL && !interrupted; iter = iter->next) {
        dtxdg2appmgr_DesktopEntry *entry = (dtxdg2appmgr_DesktopEntry *)iter->data;

        /* 8a. Filter: skip entries that shouldn't be included */
        if (!dtxdg2appmgr_desktop_should_include(entry)) {
            if (opts->verbose)
                g_print("  SKIP (filtered): %s\n", entry->id);
            skipped++;
            continue;
        }

        /* 8b. Cache check: skip if valid (unless force_rebuild) */
        if (!opts->force_rebuild && dtxdg2appmgr_cache_is_valid(cache, entry->path)) {
            if (opts->verbose)
                g_print("  SKIP (cached):   %s\n", entry->id);
            skipped++;
            continue;
        }

        if (opts->verbose)
            g_print("  Processing:      %s\n", entry->id);

        /* 8c. Get primary category and map to CDE group (always _XDG suffix) */
        gchar *cats_joined = entry->categories ? g_strjoinv(";", entry->categories) : g_strdup("");
        gchar *primary_cat = dtxdg2appmgr_get_primary_category(cats_joined);
        gchar *group = dtxdg2appmgr_category_to_group(primary_cat);
        g_free(primary_cat);
        g_free(cats_joined);

        if (group == NULL) {
            group = g_strdup("Other_XDG");
        }

        /* Record group for later group .dt file writing */
        if (!g_hash_table_contains(groups_seen, group))
            g_hash_table_add(groups_seen, g_strdup(group));

        /* 8d. Collision check: adjust action_name if needed */
        gchar *group_dir = g_build_filename(appmanager_dir, group, NULL);
        gchar *action_name = dtxdg2appmgr_sanitize_action_name(entry->name);

        if (dtxdg2appmgr_is_collision(action_name, group_dir)) {
            gchar *original = action_name;
            gchar *id_sanitized = dtxdg2appmgr_sanitize_action_name(entry->id);
            action_name = g_strconcat(original, "_", id_sanitized, NULL);
            g_free(original);
            g_free(id_sanitized);
        }

        /* 8e. Sanitize action name (already done above, but ensure it's clean) */
        gchar *clean_name = dtxdg2appmgr_sanitize_action_name(action_name);
        g_free(action_name);
        action_name = clean_name;

        /* 8f. Resolve icon */
        dtxdg2appmgr_IconResolution icon_res = { NULL, NULL };
        gboolean icon_found = dtxdg2appmgr_icon_resolve(entry->icon,
                                                         (const gchar **)data_dirs,
                                                         &icon_res);

        gchar *icon_basename = NULL;
        if (icon_found) {
            icon_basename = dtxdg2appmgr_generate_pm_basename(action_name);

            /* 8g. Convert icons to XPM set (3 sizes) */
            if (!opts->dry_run) {
                if (!dtxdg2appmgr_convert_to_xpm_set(icon_res.absolute_path,
                                                      icon_basename,
                                                      icon_output_dir,
                                                      &error)) {
                    g_printerr("Warning: icon conversion failed for %s: %s\n",
                               entry->id, error->message);
                    g_error_free(error);
                    error = NULL;
                    
                }
            } else if (opts->verbose) {
                g_print("    Would convert icon: %s -> %s\n",
                        icon_res.absolute_path, icon_basename);
            }
        } else if (opts->verbose) {
            g_print("    Icon not found for: %s\n", entry->icon ? entry->icon : "(none)");
        }

        /* 8h. Parse exec line to CDE string */
        gchar *exec_string = dtxdg2appmgr_exec_to_cde_string(entry->exec,
                                                               entry->name,
                                                               entry->path,
                                                               entry->icon ? entry->icon : "");

        /* 8i. Write .dt file */
        if (!opts->dry_run) {
            if (!dtxdg2appmgr_write_dt_file(entry, action_name, exec_string,
                                            icon_basename, dt_output_dir, &error)) {
                g_printerr("Error writing .dt file for %s: %s\n",
                           entry->id, error->message);
                g_error_free(error);
                error = NULL;
                errors++;
                g_free(exec_string);
                g_free(icon_basename);
                g_free(action_name);
                g_free(group);
                g_free(group_dir);
                dtxdg2appmgr_icon_resolution_free(&icon_res);
                continue;
            }
        } else if (opts->verbose) {
            g_print("    Would write .dt: %s/%s.dt\n", dt_output_dir, action_name);
        }

        
        gchar *dt_path = g_build_filename(dt_output_dir, action_name, NULL);
        gchar *dt_path_ext = g_strconcat(dt_path, ".dt", NULL);
        g_free(dt_path);

        /* 8j. Write appmanager stub */
        gboolean stub_ok = TRUE;
        if (!opts->dry_run) {
            if (!ensure_dir(group_dir, &error)) {
                g_printerr("Error creating appmanager group dir %s: %s\n",
                           group_dir, error->message);
                g_error_free(error);
                error = NULL;
                errors++;
                stub_ok = FALSE;
            } else if (!dtxdg2appmgr_write_appmanager_stub(group_dir, action_name,
                                                            &error)) {
                g_printerr("Error writing appmanager stub for %s: %s\n",
                           entry->id, error->message);
                g_error_free(error);
                error = NULL;
                errors++;
                stub_ok = FALSE;
            }
        } else if (opts->verbose) {
            g_print("    Would write stub: %s/%s\n", group_dir, action_name);
        }

        /* 8k. Update cache */
        if (!opts->dry_run && stub_ok) {
            dtxdg2appmgr_cache_update(cache, entry->path, dt_path_ext,
                                      (glong)entry->mtime);
        }

        processed++;
        g_free(dt_path_ext);
        g_free(exec_string);
        g_free(icon_basename);
        g_free(action_name);
        g_free(group);
        g_free(group_dir);
        dtxdg2appmgr_icon_resolution_free(&icon_res);
    }

    
    if (interrupted) {
        g_printerr("\nInterrupted by signal. Saving cache...\n");
    }

    /* 9. Write group .dt files for each unique group encountered */
    if (!opts->dry_run) {
        GHashTableIter hiter;
        gpointer key;
        g_hash_table_iter_init(&hiter, groups_seen);
        while (g_hash_table_iter_next(&hiter, &key, NULL)) {
            const gchar *group_name = (const gchar *)key;
            if (!dtxdg2appmgr_write_group_dt(group_name, dt_output_dir, &error)) {
                g_printerr("Error writing group .dt for %s: %s\n",
                           group_name, error->message);
                g_error_free(error);
                error = NULL;
                errors++;
            }
        }
    } else if (opts->verbose) {
        GHashTableIter hiter;
        gpointer key;
        g_hash_table_iter_init(&hiter, groups_seen);
        while (g_hash_table_iter_next(&hiter, &key, NULL)) {
            g_print("  Would write group .dt: %s/%s.dt\n",
                    dt_output_dir, (const gchar *)key);
        }
    }

    /* 10. Save cache */
    if (!opts->dry_run) {
        if (!dtxdg2appmgr_cache_save(cache, opts->cache_file)) {
            g_printerr("Warning: failed to save cache to %s\n", opts->cache_file);
        } else if (opts->verbose) {
            g_print("Cache saved to %s\n", opts->cache_file);
        }
    }

    
    if (opts->verbose) {
        g_print("\nSummary: %d processed, %d skipped, %d errors\n",
                processed, skipped, errors);
    }

    if (errors > 0)
        exit_code = 1;

cleanup:
    g_hash_table_destroy(groups_seen);
    if (cache)
        dtxdg2appmgr_cache_free(cache);
    dtxdg2appmgr_xdg_free_entry_list(entries);
    g_free(lang);
    dtxdg2appmgr_xdg_free_data_dirs(data_dirs);
    g_free(dt_output_dir);
    g_free(icon_output_dir);
    g_free(appmanager_dir);
    dtxdg2appmgr_options_free(opts);

    return exit_code;
}