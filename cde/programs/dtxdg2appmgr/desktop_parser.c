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

#ifdef HAVE_CONFIG_H
#include <cde_config.h>
#endif

#include <sys/stat.h>
#include <glib.h>
#include <string.h>
#include "desktop_parser.h"

static gchar *
normalize_lang(const gchar *lang)
{
    if (lang == NULL || lang[0] == '\0')
        return NULL;

    const gchar *dot = strchr(lang, '.');
    const gchar *underscore = strchr(lang, '_');
    gchar *normalized;

    if (underscore != NULL) {
        gsize len = (gsize)(underscore - lang);
        normalized = g_strndup(lang, len);
    } else if (dot != NULL) {
        gsize len = (gsize)(dot - lang);
        normalized = g_strndup(lang, len);
    } else {
        normalized = g_strdup(lang);
    }

    if (normalized[0] == '\0') {
        g_free(normalized);
        return NULL;
    }

    return normalized;
}

static gchar *
get_locale_string(GKeyFile *kf, const gchar *key, const gchar *lang)
{
    GError *error = NULL;
    gchar *value = NULL;

    if (lang != NULL && lang[0] != '\0') {
        value = g_key_file_get_locale_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                             key, lang, &error);
        if (error != NULL) {
            g_error_free(error);
            error = NULL;
            value = NULL;
        }
    }

    if (value == NULL) {
        value = g_key_file_get_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                      key, &error);
        if (error != NULL) {
            g_error_free(error);
            return NULL;
        }
    }

    return value;
}

dtxdg2appmgr_DesktopEntry *
dtxdg2appmgr_desktop_parse(const gchar *path, const gchar *lang)
{
    GKeyFile *kf;
    GError *error = NULL;
    dtxdg2appmgr_DesktopEntry *entry;
    gchar *categories_str;
    gchar *normalized_lang;
    struct stat st;

    if (path == NULL)
        return NULL;

    kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, path, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error)) {
        g_error_free(error);
        g_key_file_free(kf);
        return NULL;
    }

    entry = g_new0(dtxdg2appmgr_DesktopEntry, 1);

    entry->path = g_strdup(path);
    entry->id = g_path_get_basename(path);

    normalized_lang = normalize_lang(lang);

    entry->name = get_locale_string(kf, G_KEY_FILE_DESKTOP_KEY_NAME, normalized_lang);
    entry->generic_name = get_locale_string(kf, G_KEY_FILE_DESKTOP_KEY_GENERIC_NAME, normalized_lang);
    entry->comment = get_locale_string(kf, G_KEY_FILE_DESKTOP_KEY_COMMENT, normalized_lang);

    g_free(normalized_lang);

    entry->exec = g_key_file_get_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                        G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);
    entry->icon = g_key_file_get_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                        G_KEY_FILE_DESKTOP_KEY_ICON, NULL);
    entry->type = g_key_file_get_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                        G_KEY_FILE_DESKTOP_KEY_TYPE, NULL);
    if (entry->type == NULL)
        entry->type = g_strdup("Application");

    entry->terminal = g_key_file_get_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                            G_KEY_FILE_DESKTOP_KEY_TERMINAL, NULL);
    if (entry->terminal == NULL)
        entry->terminal = g_strdup("false");

    entry->no_display = g_key_file_get_boolean(kf, G_KEY_FILE_DESKTOP_GROUP,
                                                G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY, NULL);
    entry->hidden = g_key_file_get_boolean(kf, G_KEY_FILE_DESKTOP_GROUP,
                                           G_KEY_FILE_DESKTOP_KEY_HIDDEN, NULL);

    entry->startup_wm_class = g_key_file_get_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                                     "StartupWMClass", NULL);

    entry->try_exec = g_key_file_get_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                            G_KEY_FILE_DESKTOP_KEY_TRY_EXEC, NULL);

    categories_str = g_key_file_get_string(kf, G_KEY_FILE_DESKTOP_GROUP,
                                           G_KEY_FILE_DESKTOP_KEY_CATEGORIES, NULL);
    if (categories_str != NULL) {
        entry->categories = g_strsplit(categories_str, ";", -1);
        g_free(categories_str);
    } else {
        entry->categories = g_new0(gchar *, 1);
    }

    if (stat(path, &st) == 0)
        entry->mtime = st.st_mtime;

    g_key_file_free(kf);
    return entry;
}

void
dtxdg2appmgr_desktop_entry_free(dtxdg2appmgr_DesktopEntry *entry)
{
    if (entry == NULL)
        return;

    g_free(entry->id);
    g_free(entry->path);
    g_free(entry->name);
    g_free(entry->generic_name);
    g_free(entry->comment);
    g_free(entry->exec);
    g_free(entry->icon);
    g_free(entry->type);
    g_free(entry->terminal);
    g_free(entry->startup_wm_class);
    g_free(entry->try_exec);
    g_strfreev(entry->categories);
    g_free(entry);
}

gboolean
dtxdg2appmgr_desktop_should_include(const dtxdg2appmgr_DesktopEntry *entry)
{
    if (entry == NULL)
        return FALSE;

    if (g_strcmp0(entry->type, "Application") != 0)
        return FALSE;

    if (entry->no_display)
        return FALSE;

    if (entry->hidden)
        return FALSE;

    /* Skip applications that require a terminal emulator */
    if (g_ascii_strcasecmp(entry->terminal, "true") == 0)
        return FALSE;

    return TRUE;
}