# Learnings - dtxdg2appmgr-fixes

## 2026-06-15 Initial Context

- Build command: `cd cde/programs/dtxdg2appmgr && make clean && make`
- All source files in `cde/programs/dtxdg2appmgr/`
- LGPL v2 license headers required on all files
- `#include <cde_config.h>` with `#ifdef HAVE_CONFIG_H` guard required
- All public symbols use `dtxdg2appmgr_` prefix
- `_XDG` suffix always applied to all group names (no conditional)
- CDE group .dt format reference: `cde/programs/types/TeX.dt.src`
  - DATA_ATTRIBUTES uses `*Appgroup` suffix (e.g., `TeXAppgroup`)
  - DATA_CRITERIA uses `*Criteria1` suffix
  - ACTION Open → MAP_ACTION OpenAppGroup
  - ACTION Print → MAP_ACTION PrintAppGroup
  - ICON field uses group-specific icon (e.g., `TeXGroup`)
  - PATH_PATTERN: `*/appmanager/*/<GroupName>`
- Xsession.src line 303: variable `dtstart_xdg2appmgr` is set but `StartFirst dtstart_xdg2appmgr` is NOT called
- Xsession.src `StartFirst()` function starts at line 456
- Current group .dt is MISSING: ACTION Print, OPEN_X_APPGROUP DATA_ATTRIBUTES, proper Appgroup naming
- Shell injection in xpm_converter.c lines 62,96: `g_spawn_command_line_sync` passes raw paths to `/bin/sh -c`
## 2026-06-15: Shell injection fix in xpm_converter.c

### Pattern: g_spawn_command_line_sync() is dangerous
- `g_spawn_command_line_sync()` passes the command string through `/bin/sh -c`
- Any user-controlled data in the command string (file paths, names) enables shell injection
- A `.desktop` file with `Icon=/tmp/x;rm -rf ~;echo.png` would execute `rm -rf ~`

### Correct pattern: g_spawn_sync() with argv array
- For direct program invocation (ImageMagick `convert`): use `g_spawn_sync()` with a `gchar *argv[]` array. Each argument is a separate array element — no shell interpretation.
- For pipelines (netpbm): still need `/bin/sh -c`, but use `g_shell_quote()` on all user-controlled paths before interpolating them into the command string. This ensures shell metacharacters are escaped.

### Key APIs used
- `g_spawn_sync()` — safe argv-based process spawn
- `g_shell_quote()` — quotes a string for safe shell interpolation (wraps in single quotes, escapes embedded single quotes)
- `G_SPAWN_SEARCH_PATH` — lets `g_spawn_sync` find the executable in PATH
- `G_SPAWN_STDOUT_TO_DEV_NULL` — suppress stdout for ImageMagick (prevents buffer allocation)

### Build note
- LSP/clangd shows errors for GLib types (`gchar`, `gboolean`, etc.) because clangd doesn't find the GLib headers. The actual gcc build succeeds with 0 warnings since autotools provides correct include paths.

## 2026-06-15: Cache-failure consistency + memory leak fix in dtxdg2appmgr.c

### Memory leak pattern: g_strjoinv
- `g_strjoinv()` returns a newly-allocated string that must be freed with `g_free()`.
- Inline `g_strjoinv()` calls in function arguments are leaks because the return value is never captured.
- Fix pattern: assign to a local, use it, then `g_free()`. For the empty-string fallback when categories is NULL, use `g_strdup("")` to keep the type signature uniform (both branches yield a freeable `gchar *`).

### Cache-failure consistency pattern
- When a multi-step operation has a "success" semantic, gate side effects (cache updates, "processed" counters) on a boolean flag flipped by every error path.
- The `stub_ok` flag pattern: initialize TRUE, set FALSE on any failure branch, gate the post-block side effects on `&& stub_ok`.
- This is cleaner than `goto` for short blocks and avoids needing a separate cleanup label.

### Section comment convention in dtxdg2appmgr.c
- The file uses `/* 8a. ... */`, `/* 8b. ... */`, etc. as algorithmic-step section markers.
- When editing such a block, preserve the original marker comment exactly (e.g., keep `/* 8k. Update cache */` rather than rewriting to describe the new behavior).

### Build verification
- `make clean && make` in `cde/programs/dtxdg2appmgr/` produces no warnings, exits 0, links the `dtxdg2appmgr` binary (~155 KB).

### 2026-06-15: signal() → sigaction() for dtxdg2appmgr

#### Why sigaction, not signal()
- `signal()` behavior is implementation-defined and historically inconsistent across UNIX variants (BSD vs. SysV). After the call, the handler can be reset to `SIG_DFL` on first delivery on some systems.
- `sigaction()` is the POSIX-standard replacement with guaranteed semantics: handler persists, no system call restarts unless `SA_RESTART` is requested, and the signal mask is explicit.

#### Pattern: graceful shutdown for SIGINT/SIGTERM/SIGHUP
- Group all three shutdown signals onto one handler that sets a `volatile sig_atomic_t interrupted` flag.
- The main loop checks `interrupted` on each iteration and exits cleanly, allowing the cache to flush.
- `sa_flags = 0` (no `SA_RESTART`) is deliberate: we WANT interrupted syscalls to fail with `EINTR` so the loop wakes up promptly instead of blocking forever in a long-running system call.

#### Pattern: SIGPIPE ignore for spawned children
- Image conversion uses `g_spawn_sync()` to invoke ImageMagick / netpbm. If the child dies early or closes its stdout pipe, the parent would otherwise receive SIGPIPE and die.
- `sigaction(SIGPIPE, &sa_ign, NULL)` with `SIG_IGN` prevents this — `write()` returns `EPIPE` instead and the caller can handle it.

#### CDE convention
- `struct sigaction sa` reused for multiple signals of the same handler — cleaner than separate `struct sigaction` per signal.
- Wrap setup in a bare `{ ... }` block to scope the two `struct sigaction` locals (one for handler, one for SIG_IGN).

#### Build
- `make clean && make` exits 0, 0 warnings.
- The existing `sigint_handler` (renaming would be churn) and `interrupted` variable are kept — the handler is generic enough to serve SIGINT, SIGTERM, and SIGHUP identically.

## 2026-06-15: Removed GTK-specific %i transformation from exec_parser.c

- `%i` in XDG Exec lines is a GTK convention that adds `--icon <name>` to the command line
- CDE uses the ICON field in .dt files directly, so `%i` should produce no output
- Both occurrences (quote context ~line 74, non-quote context ~line 113) were replaced with silent consumption
- The `icon_name` parameter is still passed to `dtxdg2appmgr_exec_to_cde_string()` but is now unused by the `%i` handler — could be removed in a future cleanup
- Build: `cd cde/programs/dtxdg2appmgr && make clean && make` — zero warnings

## 2026-06-15: Recursive directory scanning in path_scanner.c

- `g_dir_open()` + `g_dir_read_name()` is the GLib pattern for directory iteration
- `g_file_test()` with `G_FILE_TEST_IS_DIR` and `G_FILE_TEST_IS_SYMLINK` for filesystem checks
- `g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL)` creates a string set with auto-free
- `g_hash_table_add()` (GLib 2.40+) adds to the set; returns gboolean
- `realpath(path, NULL)` is a GNU extension that mallocs the result buffer — works on Linux, needs `<stdlib.h>`
- Symlink-to-directory check: must check `G_FILE_TEST_IS_SYMLINK` separately from `G_FILE_TEST_IS_DIR` because symlinks that point to directories would pass both tests
- Pattern: skip symlinks before canonical path resolution to avoid following them at all
- clangd shows false-positive errors for `<glib.h>` — actual gcc build works fine with `-I/usr/include/glib-2.0` etc.

### 2026-06-15: Group .dt format alignment with CDE TeX.dt reference

#### CDE group .dt naming convention (final)
- `DATA_ATTRIBUTES <Group>Appgroup` (e.g., `TeXAppgroup`, `Application_XDGAppgroup`)
- `DATA_CRITERIA <Group>AppgroupCriteria1` (note: `AppgroupCriteria1`, not just `Criteria1`)
- `DATA_ATTRIBUTES_NAME` must match the `Appgroup` suffixed name
- `ICON <Group>Group` (group-specific icon, not the generic `Dtapps`)
- `DESCRIPTION` ends with a period (`<Group> Applications.`) — match the CDE reference, not the prior ` (XDG)` suffix which was a non-CDE marker
- `PATH_PATTERN` keeps the directory portion (`*/appmanager/*/`) but uses the bare group name (no `_XDG`) for the leaf — the leaf matches the stub directory CDE already creates

#### ACTION Open / ACTION Print pairs
- Both ACTION blocks share `ARG_TYPE = <Group>Appgroup` (must match `DATA_ATTRIBUTES` name)
- Both are `TYPE = MAP`
- `OpenAppGroup` for ACTION Open, `PrintAppGroup` for ACTION Print (CDE system actions — never rename)
- Adding the second ACTION block means a separating blank line between the `}` of ACTION Open and `ACTION Print` (the prior code had a single `fprintf(fp, "}\n")` and no trailing blank — added `}\n\n` for Open, then `ACTION Print\n`)

#### Memory management pattern
- Each `g_strdup_printf` allocation needs `g_free` on every early-return error path
- The function has 2 early returns (mkdir, fopen) plus the success path — all 3 must free all 4 owned strings (group_xdg, attrs_name, criteria_name, icon_name)
- Cleanest: declare all `g_free()` calls in matching order at each exit. Compiler doesn't enforce this, but a missed `g_free` on a new string would leak every error path — high-value lint

#### Build
- `make clean && make` exits 0, 0 warnings
- Verified output format against `cde/programs/types/TeX.dt` — generated `Application_XDG.dt` matches TeX.dt structure field-for-field (modulo group name)

## 2026-06-15: Final Verification Wave Results

### F1 Plan Compliance — 8/8 items verified in code
- xpm_converter.c:72,113 — `g_spawn_sync` with argv[] (no `g_spawn_command_line_sync`)
- Xsession.src:506 — `StartFirst dtstart_xdg2appmgr` confirmed
- dtxdg2appmgr.c:370,393 — `stub_ok` boolean gates cache update
- dtxdg2appmgr.c:280 — `g_free(cats_joined)` after `g_strjoinv` (no leak)
- dtxdg2appmgr.c:152-167 — `sigaction` for SIGINT/SIGTERM/SIGHUP + SIG_IGN for SIGPIPE
- desktop_parser.c:103 — `G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS`
- desktop_parser.c:200-201 — `Terminal=true` filter (`g_ascii_strcasecmp(... "true")`)
- path_scanner.c:87 — `scan_directory_recursive` with realpath (line 117) + GHashTable visited
- dt_writer.c:223-228 — ACTION Print → MAP_ACTION PrintAppGroup
- dt_writer.c:165-167 — Appgroup/Group naming convention
- exec_parser.c:74-76, 113-115 — `%i` consumed silently (no --icon output)

### F2 Code Quality — clean
- `make clean && make` → 0 warnings, 0 errors
- 0 occurrences of banned APIs (g_spawn_command_line_sync, signal(SIGINT, G_KEY_FILE_NONE)
- 31 dtxdg2appmgr_ symbols exported (well above 10+)
- glib-2.0 + gio-2.0 dynamically linked

### F3 Real Manual QA — defense verified
- Malicious Icon= path passed as literal filename to `convert`/`pngtopnm`, not shell-interpreted
- `/tmp/PWNED` was NOT created after running with `Icon=/tmp/x;touch /tmp/PWNED;echo .png`
- Terminal=true .desktop files filtered out (only 2/3 entries processed)
- Group .dt format contains ACTION Print with MAP_ACTION PrintAppGroup
- 0 double `_XDG` files (only one suffix per group name)
- Cache-failure consistency verified: read-only stub dir → entry NOT cached → retried next run

### F4 Scope Fidelity — exact
- 7 files modified, matching plan exactly
- 3 commits (59af7d342, 39014bf72, f7e9ce62b)
- No other CDE files touched

## 2026-06-16: Fixed default directory resolution for user-local operation

### Bug
`options.c` set non-NULL defaults for `output_dir`, `icon_output_dir`, and `cache_file` (hardcoded system paths like `/usr/dt/appconfig/appmanager`). This meant `get_dt_output_dir()` and `get_icon_output_dir()` in `dtxdg2appmgr.c` never reached their `$HOME/.dt/...` fallback logic.

### Fix
1. Changed `options.c` to assign NULL when no CLI flag is given (GOption leaves `output_dir`, `icon_output_dir`, `cache_file` as NULL)
2. Added `get_cache_file()` in `dtxdg2appmgr.c` with three-tier fallback: explicit → `$HOME/.dt/xdg-cache.db` → `/var/dt/xdg-cache.db`
3. Changed `get_appmanager_dir()` to take `dt_output_dir` string instead of `opts` struct (since `opts->output_dir` can now be NULL)
4. Updated main() to resolve all paths upfront via `get_*()` functions, then use resolved values throughout
5. Added `ensure_dir(cache_dir)` for the cache file's parent directory
6. Updated usage message to show `$HOME/.dt/...` defaults instead of system paths

### Key insight
`g_free(NULL)` is safe in GLib, so no special handling needed in `options_free()` for NULL path fields.

### Build
`cd cde/programs/dtxdg2appmgr && make clean && make` — compiles with 0 warnings, 0 errors.

### Test
`./dtxdg2appmgr --dry-run -v` — correctly resolves to `$HOME/.dt/types`, `$HOME/.dt/icons`, `$HOME/.dt/xdg-cache.db` without flags.
`./dtxdg2appmgr --dry-run -v -o /usr/dt/appconfig/appmanager` — correctly uses explicit system path.
