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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "dt_writer.h"
#include "desktop_parser.h"
#include "category_mapper.h"

gboolean
dtxdg2appmgr_write_dt_file(const dtxdg2appmgr_DesktopEntry *entry,
                            const gchar *action_name,
                            const gchar *exec_string,
                            const gchar *icon_basename,
                            const gchar *dt_output_dir,
                            GError **error)
{
    gchar *dir_path;
    gchar *file_path;
    FILE *fp;
    gchar *sanitized;

    g_return_val_if_fail(entry != NULL, FALSE);
    g_return_val_if_fail(action_name != NULL, FALSE);
    g_return_val_if_fail(exec_string != NULL, FALSE);
    g_return_val_if_fail(dt_output_dir != NULL, FALSE);

    sanitized = dtxdg2appmgr_sanitize_action_name(action_name);
    if (sanitized == NULL || sanitized[0] == '\0') {
        g_free(sanitized);
        g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                    "Invalid action name for .dt file");
        return FALSE;
    }

    if (g_mkdir_with_parents(dt_output_dir, 0755) != 0) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Failed to create directory '%s': %s",
                    dt_output_dir, g_strerror(errno));
        g_free(sanitized);
        return FALSE;
    }

    {
        gchar *name_with_ext = g_strconcat(sanitized, ".dt", NULL);
        g_free(sanitized);
        file_path = g_build_filename(dt_output_dir, name_with_ext, NULL);
        g_free(name_with_ext);
    }

    fp = g_fopen(file_path, "w");
    if (fp == NULL) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Failed to open '%s' for writing: %s",
                    file_path, g_strerror(errno));
        g_free(file_path);
        return FALSE;
    }

    fprintf(fp, "ACTION %s\n", action_name);
    fprintf(fp, "{\n");
    fprintf(fp, "\tLABEL\t\t%s\n",
            entry->name ? entry->name : action_name);
    if (icon_basename && icon_basename[0] != '\0')
        fprintf(fp, "\tICON\t\t%s\n", icon_basename);
    fprintf(fp, "\tTYPE\t\tCOMMAND\n");
    fprintf(fp, "\tWINDOW_TYPE\tNO_STDIO\n");
    fprintf(fp, "\tEXEC_STRING\t%s\n", exec_string);
    if (entry->comment && entry->comment[0] != '\0')
        fprintf(fp, "\tDESCRIPTION\t%s\n", entry->comment);
    fprintf(fp, "}\n");

    fclose(fp);
    g_free(file_path);
    return TRUE;
}

gboolean
dtxdg2appmgr_write_appmanager_stub(const gchar *group_dir,
                                   const gchar *action_name,
                                   GError **error)
{
    gchar *stub_path;
    FILE *fp;

    g_return_val_if_fail(group_dir != NULL, FALSE);
    g_return_val_if_fail(action_name != NULL, FALSE);

    if (g_mkdir_with_parents(group_dir, 0755) != 0) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Failed to create directory '%s': %s",
                    group_dir, g_strerror(errno));
        return FALSE;
    }

    stub_path = g_build_filename(group_dir, action_name, NULL);

    fp = g_fopen(stub_path, "w");
    if (fp == NULL) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Failed to create stub '%s': %s",
                    stub_path, g_strerror(errno));
        g_free(stub_path);
        return FALSE;
    }
    fclose(fp);

    if (g_chmod(stub_path, 0755) != 0) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Failed to chmod stub '%s': %s",
                    stub_path, g_strerror(errno));
        g_free(stub_path);
        return FALSE;
    }

    g_free(stub_path);
    return TRUE;
}

gboolean
dtxdg2appmgr_write_group_dt(const gchar *group_name,
                             const gchar *dt_output_dir,
                             GError **error)
{
    gchar *group_xdg;
    gchar *criteria_name;
    gchar *file_path;
    FILE *fp;

    g_return_val_if_fail(group_name != NULL, FALSE);
    g_return_val_if_fail(dt_output_dir != NULL, FALSE);

    group_xdg = g_strdup(group_name);
    criteria_name = g_strdup_printf("%sCriteria1", group_xdg);

    if (g_mkdir_with_parents(dt_output_dir, 0755) != 0) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Failed to create directory '%s': %s",
                    dt_output_dir, g_strerror(errno));
        g_free(group_xdg);
        g_free(criteria_name);
        return FALSE;
    }

    file_path = g_build_filename(dt_output_dir, group_xdg, NULL);
    {
        gchar *dot_path = g_strconcat(file_path, ".dt", NULL);
        g_free(file_path);
        file_path = dot_path;
    }

    fp = g_fopen(file_path, "w");
    if (fp == NULL) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Failed to open '%s' for writing: %s",
                    file_path, g_strerror(errno));
        g_free(file_path);
        g_free(group_xdg);
        g_free(criteria_name);
        return FALSE;
    }

    fprintf(fp, "DATA_ATTRIBUTES %s\n", group_xdg);
    fprintf(fp, "{\n");
    fprintf(fp, "\tACTIONS\t\tOpenInPlace,OpenNewView\n");
    fprintf(fp, "\tLABEL\t\t%s\n", group_name);
    fprintf(fp, "\tICON\t\tDtapps\n");
    fprintf(fp, "\tDESCRIPTION\t%s Applications (XDG)\n", group_name);
    fprintf(fp, "}\n\n");

    fprintf(fp, "DATA_CRITERIA %s\n", criteria_name);
    fprintf(fp, "{\n");
    fprintf(fp, "\tDATA_ATTRIBUTES_NAME\t%s\n", group_xdg);
    fprintf(fp, "\tLABEL\t\t\t%s\n", group_name);
    fprintf(fp, "\tMODE\t\t\td\n");
    fprintf(fp, "\tPATH_PATTERN\t\t*/appmanager/*/%s\n", group_xdg);
    fprintf(fp, "}\n\n");

    fprintf(fp, "ACTION Open\n");
    fprintf(fp, "{\n");
    fprintf(fp, "\tARG_TYPE\t%s\n", group_xdg);
    fprintf(fp, "\tTYPE\t\tMAP\n");
    fprintf(fp, "\tMAP_ACTION\tOpenAppGroup\n");
    fprintf(fp, "}\n");

    fclose(fp);
    g_free(file_path);
    g_free(group_xdg);
    g_free(criteria_name);
    return TRUE;
}