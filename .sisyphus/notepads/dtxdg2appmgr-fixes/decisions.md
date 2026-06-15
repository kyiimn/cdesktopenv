# Decisions - dtxdg2appmgr-fixes

## 2026-06-15 Initial Decisions

- Shell injection fix: Use `g_spawn_sync` with argv array for ImageMagick, `g_shell_quote` for netpbm pipeline
- Xsession fix: Add `StartFirst dtstart_xdg2appmgr` after line 505 (after `StartFirst dtstart_appgather`)
- Cache fix: Add `continue` after stub-write failure to skip cache update
- Terminal filter: Add `g_ascii_strcasecmp(entry->terminal, "true") == 0` check in `desktop_should_include`
- Recursive scan: Convert `g_dir_open`+`g_dir_read_name` loop to recursive function with symlink loop protection
- Group .dt: Add ACTION Print + OPEN_X_APPGROUP DATA_ATTRIBUTES block following TeX.dt pattern
- %i removal: Replace `%i` with empty string instead of `--icon <name>`
- Signal handling: Replace `signal()` with `sigaction()`, add SIGTERM/SIGHUP handlers, ignore SIGPIPE
- G_KEY_FILE flags: Change `G_KEY_FILE_NONE` to `G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS`
- Memory leak: Free `g_strjoinv` result in main loop (line 259)