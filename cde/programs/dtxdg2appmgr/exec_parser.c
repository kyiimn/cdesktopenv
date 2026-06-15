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
#include <string.h>

#include "exec_parser.h"

static gboolean
is_deprecated_field_code(gchar code)
{
    return code == 'n' || code == 'N' || code == 'v' || code == 'm';
}

gchar *
dtxdg2appmgr_exec_to_cde_string(const gchar *exec_line,
                                 const gchar *name,
                                 const gchar *desktop_path,
                                 const gchar *icon_name)
{
    GString *result;
    const gchar *p;
    gchar quote_char;

    if (exec_line == NULL)
        return g_strdup("");

    result = g_string_sized_new(strlen(exec_line) + 32);
    p = exec_line;
    quote_char = '\0';

    while (*p != '\0') {
        if (quote_char != '\0') {
            if (*p == quote_char) {
                quote_char = '\0';
                g_string_append_c(result, *p);
                p++;
            } else if (*p == '%') {
                p++;
                if (*p == '\0') {
                    break;
                }
                if (*p == '%') {
                    g_string_append_c(result, '%');
                    p++;
                } else if (*p == 'f' || *p == 'F' ||
                           *p == 'u' || *p == 'U') {
                    g_string_append(result, "%Arg_1%");
                    p++;
                } else if (*p == 'i') {
                    if (icon_name != NULL && icon_name[0] != '\0') {
                        g_string_append(result, "--icon ");
                        g_string_append(result, icon_name);
                    }
                    p++;
                } else if (*p == 'c') {
                    if (name != NULL)
                        g_string_append(result, name);
                    p++;
                } else if (*p == 'k') {
                    if (desktop_path != NULL)
                        g_string_append(result, desktop_path);
                    p++;
                } else if (*p == 'd' || *p == 'D') {
                    p++;
                } else if (is_deprecated_field_code(*p)) {
                    p++;
                } else {
                    p++;
                }
            } else {
                g_string_append_c(result, *p);
                p++;
            }
        } else {
            if (*p == '"' || *p == '\'') {
                quote_char = *p;
                g_string_append_c(result, *p);
                p++;
            } else if (*p == '%') {
                p++;
                if (*p == '\0') {
                    break;
                }
                if (*p == '%') {
                    g_string_append_c(result, '%');
                    p++;
                } else if (*p == 'f' || *p == 'F' ||
                           *p == 'u' || *p == 'U') {
                    g_string_append(result, "%Arg_1%");
                    p++;
                } else if (*p == 'i') {
                    if (icon_name != NULL && icon_name[0] != '\0') {
                        g_string_append(result, "--icon ");
                        g_string_append(result, icon_name);
                    }
                    p++;
                } else if (*p == 'c') {
                    if (name != NULL)
                        g_string_append(result, name);
                    p++;
                } else if (*p == 'k') {
                    if (desktop_path != NULL)
                        g_string_append(result, desktop_path);
                    p++;
                } else if (*p == 'd' || *p == 'D') {
                    p++;
                } else if (is_deprecated_field_code(*p)) {
                    p++;
                } else {
                    p++;
                }
            } else {
                g_string_append_c(result, *p);
                p++;
            }
        }
    }

    return g_string_free(result, FALSE);
}

void
dtxdg2appmgr_exec_context_free(dtxdg2appmgr_ExecContext *ctx)
{
    if (ctx == NULL)
        return;

    g_free(ctx->file_path);
    g_strfreev(ctx->file_paths);
    g_free(ctx->url);
    g_strfreev(ctx->urls);
    g_free(ctx->directory);
    g_free(ctx->wm_class);
    g_free(ctx->name);
    g_free(ctx->desktop_path);
    g_free(ctx);
}