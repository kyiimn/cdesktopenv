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

#include <string.h>
#include <glib.h>
#include "category_mapper.h"

static gchar *
sanitize_with_underscore(const gchar *input)
{
    gchar *result;
    gchar *p;
    glong src, dst;

    if (input == NULL || *input == '\0')
        return g_strdup("");

    result = g_new(gchar, strlen(input) + 1);

    for (p = result; *input; input++) {
        if (g_ascii_isalnum(*input) || *input == '_')
            *p++ = *input;
        else
            *p++ = '_';
    }
    *p = '\0';

    /* collapse consecutive underscores */
    src = dst = 0;
    while (result[src]) {
        result[dst++] = result[src];
        if (result[src] == '_') {
            while (result[src + 1] == '_')
                src++;
        }
        src++;
    }
    result[dst] = '\0';

    /* strip leading underscores */
    p = result;
    while (*p == '_') p++;
    if (p != result) {
        memmove(result, p, strlen(p) + 1);
    }

    /* strip trailing underscores */
    {
        glong len = (glong)strlen(result);
        while (len > 0 && result[len - 1] == '_') {
            result[len - 1] = '\0';
            len--;
        }
    }

    return result;
}

gchar *
dtxdg2appmgr_category_to_group(const gchar *category)
{
    gchar *sanitized;
    gchar *result;

    if (category == NULL || *category == '\0')
        return g_strdup("Other_XDG");

    sanitized = sanitize_with_underscore(category);

    if (*sanitized == '\0') {
        g_free(sanitized);
        return g_strdup("Other_XDG");
    }

    /* ALWAYS append _XDG suffix */
    result = g_strconcat(sanitized, "_XDG", NULL);
    g_free(sanitized);

    return result;
}

gchar *
dtxdg2appmgr_get_primary_category(const gchar *categories)
{
    gchar **parts;
    gchar *primary;
    gchar *stripped;
    gchar *p;

    if (categories == NULL || *categories == '\0')
        return g_strdup("Other");

    parts = g_strsplit(categories, ";", -1);

    if (parts[0] == NULL || *parts[0] == '\0') {
        g_strfreev(parts);
        return g_strdup("Other");
    }

    primary = g_strdup(parts[0]);
    g_strfreev(parts);

    /* strip X- prefix (e.g. X-GTK-Settings -> Settings) */
    if (g_str_has_prefix(primary, "X-")) {
        gchar *tmp = g_strdup(primary + 2);
        g_free(primary);
        primary = tmp;
    }

    /* strip GTK, Motif, GNOME, Qt prefixes per desktop2dt algorithm */
    const gchar *prefixes[] = { "GTK", "Motif", "GNOME", "Qt", NULL };
    for (const gchar **prefix = prefixes; *prefix; prefix++) {
        if (g_str_has_prefix(primary, *prefix)) {
            gchar *tmp = g_strdup(primary + strlen(*prefix));
            g_free(primary);
            primary = tmp;
            break;
        }
    }

    /* strip leading underscores left after prefix removal */
    p = primary;
    while (*p == '_') p++;
    if (p != primary) {
        stripped = g_strdup(p);
        g_free(primary);
        primary = stripped;
    }

    if (*primary == '\0') {
        g_free(primary);
        return g_strdup("Other");
    }

    return primary;
}

gboolean
dtxdg2appmgr_is_collision(const gchar *name, const gchar *group_dir)
{
    gchar *path;
    gboolean exists;

    if (name == NULL || group_dir == NULL)
        return FALSE;

    if (*name == '\0' || *group_dir == '\0')
        return FALSE;

    path = g_build_filename(group_dir, name, NULL);
    exists = g_file_test(path, G_FILE_TEST_EXISTS);
    g_free(path);

    return exists;
}

gchar *
dtxdg2appmgr_sanitize_action_name(const gchar *name)
{
    gchar *result;

    if (name == NULL || *name == '\0')
        return g_strdup("");

    result = sanitize_with_underscore(name);

    if (*result == '\0') {
        g_free(result);
        return g_strdup("");
    }

    return result;
}