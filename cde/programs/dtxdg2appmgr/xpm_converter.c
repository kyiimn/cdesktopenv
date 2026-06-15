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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>
#include "xpm_converter.h"

typedef struct {
    gint width;
    const gchar *suffix;
} SizeEntry;

static const SizeEntry size_table[] = {
    { 16, "t" },
    { 32, "m" },
    { 48, "l" },
};

#define NUM_SIZES (sizeof(size_table) / sizeof(size_table[0]))

static gboolean
convert_with_imagemagick(const gchar *src_path,
                         gint width,
                         gint height,
                         const gchar *xpm_path,
                         GError **error)
{
    gchar *resize_arg;
    gchar *argv[8];
    gint exit_status;
    gboolean ok;

    resize_arg = g_strdup_printf("%dx%d", width, height);

    argv[0] = "convert";
    argv[1] = "-background";
    argv[2] = "none";
    argv[3] = "-resize";
    argv[4] = resize_arg;
    argv[5] = (gchar *)src_path;
    argv[6] = (gchar *)xpm_path;
    argv[7] = NULL;

    ok = g_spawn_sync(NULL, argv, NULL,
                      G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL,
                      NULL, NULL, NULL, NULL, &exit_status, error);
    g_free(resize_arg);

    if (!ok)
        return FALSE;

    if (!g_spawn_check_wait_status(exit_status, error)) {
        g_clear_error(error);
        return FALSE;
    }

    return TRUE;
}

static gboolean
convert_with_netpbm(const gchar *src_path,
                    gint width,
                    gint height,
                    const gchar *xpm_path,
                    GError **error)
{
    gchar *quoted_src;
    gchar *quoted_dst;
    gchar *cmdline;
    gchar *argv[4];
    gint exit_status;
    gboolean ok;

    quoted_src = g_shell_quote(src_path);
    quoted_dst = g_shell_quote(xpm_path);
    cmdline = g_strdup_printf(
        "pngtopnm %s | pnmscale -xysize %d %d | ppmtoxpm > %s",
        quoted_src, width, height, quoted_dst);

    argv[0] = "/bin/sh";
    argv[1] = "-c";
    argv[2] = cmdline;
    argv[3] = NULL;

    ok = g_spawn_sync(NULL, argv, NULL,
                      G_SPAWN_SEARCH_PATH,
                      NULL, NULL, NULL, NULL, &exit_status, error);
    g_free(quoted_src);
    g_free(quoted_dst);
    g_free(cmdline);

    if (!ok)
        return FALSE;

    if (!g_spawn_check_wait_status(exit_status, error)) {
        g_clear_error(error);
        return FALSE;
    }

    return TRUE;
}

static gboolean
is_png_file(const gchar *path)
{
    const gchar *ext;

    if (!path)
        return FALSE;

    ext = strrchr(path, '.');
    if (!ext)
        return FALSE;

    return g_ascii_strcasecmp(ext, ".png") == 0;
}

static gchar *
xpm_path_to_pm_path(const gchar *xpm_path)
{
    gchar *copy;
    gchar *dot;

    copy = g_strdup(xpm_path);
    dot = strrchr(copy, '.');
    if (dot)
        *dot = '\0';

    return g_strconcat(copy, ".pm", NULL);
}

static gboolean
rename_xpm_to_pm(const gchar *xpm_path, GError **error)
{
    gchar *pm_path;
    gint rc;

    pm_path = xpm_path_to_pm_path(xpm_path);

    rc = rename(xpm_path, pm_path);
    if (rc != 0) {
        gint saved_errno = errno;
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(saved_errno),
                    "Failed to rename %s to %s: %s",
                    xpm_path, pm_path, g_strerror(saved_errno));
        g_free(pm_path);
        return FALSE;
    }

    g_free(pm_path);
    return TRUE;
}

gboolean
dtxdg2appmgr_convert_to_xpm_set(const gchar *src_path,
                                 const gchar *base_name,
                                 const gchar *output_dir,
                                 GError **error)
{
    gchar *created_pm[NUM_SIZES];
    gboolean created_flag[NUM_SIZES];
    gboolean all_ok = TRUE;

    if (!src_path || !base_name || !output_dir) {
        g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                    "src_path, base_name, and output_dir must not be NULL");
        return FALSE;
    }

    g_mkdir_with_parents(output_dir, 0755);

    memset(created_pm, 0, sizeof(created_pm));
    memset(created_flag, 0, sizeof(created_flag));

    for (gsize i = 0; i < NUM_SIZES; i++) {
        gchar *xpm_path;
        gchar *filename;
        gboolean converted = FALSE;

        filename = g_strdup_printf("%s.%s.xpm", base_name,
                                   size_table[i].suffix);
        xpm_path = g_build_filename(output_dir, filename, NULL);
        g_free(filename);

        if (convert_with_imagemagick(src_path,
                                      size_table[i].width,
                                      size_table[i].width,
                                      xpm_path, NULL)) {
            converted = TRUE;
        } else if (is_png_file(src_path)) {
            converted = convert_with_netpbm(src_path,
                                            size_table[i].width,
                                            size_table[i].width,
                                            xpm_path, NULL);
        }

        if (converted) {
            if (rename_xpm_to_pm(xpm_path, NULL)) {
                created_pm[i] = xpm_path_to_pm_path(xpm_path);
                created_flag[i] = TRUE;
                g_free(xpm_path);
            } else {
                unlink(xpm_path);
                g_free(xpm_path);
                all_ok = FALSE;
            }
        } else {
            g_free(xpm_path);
            all_ok = FALSE;
        }
    }

    if (!all_ok) {
        for (gsize i = 0; i < NUM_SIZES; i++) {
            if (created_flag[i] && created_pm[i]) {
                unlink(created_pm[i]);
            }
            g_free(created_pm[i]);
        }
        g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                    "Failed to convert some icon sizes for %s", src_path);
        return FALSE;
    }

    for (gsize i = 0; i < NUM_SIZES; i++)
        g_free(created_pm[i]);

    return TRUE;
}

gboolean
dtxdg2appmgr_check_tool_available(const gchar *tool_name)
{
    gchar *found;

    if (!tool_name)
        return FALSE;

    found = g_find_program_in_path(tool_name);
    if (found) {
        g_free(found);
        return TRUE;
    }

    return FALSE;
}

gchar *
dtxdg2appmgr_generate_pm_basename(const gchar *app_name)
{
    GString *sanitized;
    gchar *result;

    if (!app_name)
        return NULL;

    sanitized = g_string_new("xdg-");

    for (const gchar *p = app_name; *p; p++) {
        if (g_ascii_isalnum(*p)) {
            g_string_append_c(sanitized, *p);
        } else {
            g_string_append_c(sanitized, '_');
        }
    }

    result = g_string_free(sanitized, FALSE);
    return result;
}