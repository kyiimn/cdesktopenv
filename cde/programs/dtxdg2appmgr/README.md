# dtxdg2appmgr

Convert XDG `.desktop` application entries into CDE Application Manager
actions, group stubs, and XPM icon sets.

## Overview

`dtxdg2appmgr` walks the XDG data directories looking for `.desktop` files,
parses the ones that should be exposed to the user, and emits three kinds of
artifacts that the CDE Application Manager picks up:

- A CDE `.dt` action definition for each application
- An empty, executable Application Manager stub file inside a per-group
  directory
- A trio of `.pm` XPM icons (16x16, 32x32, 48x48) for the application

To avoid colliding with the built-in CDE application groups (such as
`Desktop_Tools` or `Desktop_Apps`), every XDG-derived group has an `_XDG`
suffix appended. The mapping is deterministic: the first XDG `Categories=`
entry is sanitized and the suffix is added.

The net effect is that any XDG-compliant application installed on the system
shows up in the CDE front panel and Application Manager with no per-app
configuration. On a typical workstation dozens of `.desktop` files become
dozens of CDE actions and a handful of `_XDG` groups, all generated in a
single run.

## Build Requirements

- GLib 2.0 and GIO 2.0, both version 2.40 or later (detected at configure
  time by `pkg-config`)
- ImageMagick `convert` and/or netpbm `pngtopnm` / `pnmscale` / `ppmtoxpm`
  for icon conversion
- A CDE build environment (autotools)
- The standard CDE build dependencies (X11/Motif, etc.)

The `Makefile.am` for the program is guarded by the `HAVE_GLIB2` autoconf
conditional. If GLib is missing, the program is silently skipped and
`make` produces nothing for this directory.

To skip the program explicitly, configure with
`--without-xdg-integration`. This sets the conditional to false regardless
of whether GLib happens to be installed.

## Build

```
./autogen.sh
./configure
make
sudo make install
```

After install, the binary lives at
`$(CDE_INSTALLATION_TOP)/bin/dtxdg2appmgr` (default `/usr/dt/bin/`).

## Usage

```
dtxdg2appmgr [OPTIONS]
```

| Short | Long              | Argument | Description                                       |
|-------|-------------------|----------|---------------------------------------------------|
| `-v`  | `--verbose`       |          | Verbose progress output                          |
| `-r`  | `--retain`        |          | Retain temporary files (passed to converters)    |
| `-o`  | `--output-dir`    | `DIR`    | CDE appconfig output directory                    |
| `-i`  | `--icon-dir`      | `DIR`    | Icon output directory                             |
| `-c`  | `--cache-file`    | `FILE`   | Cache file path                                   |
| `-f`  | `--force`         |          | Force rebuild, ignoring cache                     |
|       | `--dry-run`       |          | Show what would be done without writing any files |

### Defaults

- `output-dir`: `$HOME/.dt/types/` (per-user) when `$HOME` is set,
  otherwise `$CDE_CONFIGURATION_TOP/appconfig/types/C` (system-wide,
  default `/etc/dt/appconfig/types/C`). The appmanager stub path is
  computed as `<output-dir>/$DTUSERSESSION/`, falling back to `C` when the
  variable is unset or empty.
- `icon-dir`: `$HOME/.dt/icons/` per user, otherwise
  `$CDE_INSTALLATION_TOP/appconfig/icons/C` (default
  `/usr/dt/appconfig/icons/C`).
- `cache-file`: `$CDE_LOGFILES_TOP/xdg-cache.db` (default
  `/var/dt/xdg-cache.db`).

The process exits 0 on success and 1 on any per-entry error or fatal
setup failure. `--dry-run` always exits 0 and never writes files. `SIGINT`
triggers a graceful stop: the loop exits, the cache is flushed, and the
process returns 1.

## Output Structure

For a `.desktop` file with id `firefox.desktop` mapped to group `Network`,
the following files are written:

| Artifact       | Path                                            |
|----------------|-------------------------------------------------|
| Action file    | `<output-dir>/Firefox.dt`                       |
| Stub           | `<output-dir>/<session>/Network_XDG/Firefox`    |
| Icons          | `<icon-dir>/xdg-Firefox.{t,m,l}.pm`             |
| Group action   | `<output-dir>/Network_XDG.dt`                   |

The stub file is created mode 0755 and contains no content. Its existence
is what the CDE Application Manager uses to discover the action.

Icon sizes use the standard CDE letter suffixes:

- `t` = 16x16 (tiny)
- `m` = 32x32 (medium)
- `l` = 48x48 (large)

If the `.desktop` file has no resolvable `Icon=` entry, the action is still
written and registered, just without an icon block in the `.dt` file.

## XDG Integration

### Scanned directories

The program reads, in order:

1. `$HOME/.local/share/applications/` (always scanned, regardless of
   `XDG_DATA_DIRS`)
2. Every directory listed in `$XDG_DATA_DIRS`, with `applications/`
   appended
3. If `XDG_DATA_DIRS` is unset or empty, falls back to
   `/usr/local/share:/usr/share` (with `applications/` appended to each)

### Filtering

A `.desktop` file is included only if all of the following hold:

- `Type=Application` (the default when the key is missing)
- `NoDisplay=false` (the default)
- `Hidden=false` (the default)

Other keys (`TryExec`, `StartupWMClass`, `Terminal`, `Exec`, `Comment`,
`GenericName`, `Categories`) are read but never used to filter.

### Locale support

`Name` and `Comment` are looked up in the user's language by stripping
the encoding and country suffix from `$LANG` (for example `en_US.UTF-8`
becomes `en`). When the localized key is missing or `$LANG` is unset/`C`,
the unlocalized key is used. `GenericName` follows the same rules.

## CDE Integration

### Caching

The cache is a GLib key file at the path given by `--cache-file`. Each
section names a `.desktop` file (its absolute path) and carries two
keys: `dt_path` (the path of the generated `.dt` file) and `mtime` (the
mtime of the source `.desktop` file at the time of generation).

A `.desktop` file is considered up to date and skipped when its current
mtime matches the cached mtime. The cache is rewritten on every non-dry-run
run that processes at least one entry.

`--force` clears the cache skip behavior and processes every entry
regardless of mtime. It does not delete stale entries; if a `.desktop`
file is removed from the system, the corresponding `.dt` file lingers
until manually cleaned up.

### Xsession

`dtxdg2appmgr` is invoked from the CDE `Xsession` script after
`dtappgather` runs, so a fresh login populates the Application Manager
with the current set of XDG applications. It runs with no arguments,
which uses all defaults. A per-user installation is non-destructive: the
default `output-dir` is `$HOME/.dt/types/`, which is read by CDE in
addition to the system tree.

### ReloadApps action

The CDE `ReloadApps` action (triggered from the front panel menu) calls
`dtxdg2appmgr -r` so the user can refresh the application set without
logging out. The `-r` (retain) flag keeps any temporary files the
icon converter created, which is useful for debugging the conversion
pipeline.

### Group directory layout

Application Manager stubs are written into a per-session subdirectory
selected by `$DTUSERSESSION` (defaulting to `C`). The path
`<output-dir>/<session>/<Group>_XDG/` is created on demand. The
accompanying `<Group>_XDG.dt` action file uses the `DATA_ATTRIBUTES` /
`DATA_CRITERIA` pattern that the CDE file manager uses to recognize a
group of files and dispatch the `OpenAppGroup` action.

## Algorithm

The conversion pipeline is:

1. **Scan.** Walk each XDG data directory looking for files matching
   `*.desktop` (hidden files are skipped).
2. **Parse.** Use GLib's `GKeyFile` in desktop mode to read each file.
   Extract `Type`, `NoDisplay`, `Hidden`, `Exec`, `Icon`, `Terminal`,
   `TryExec`, `StartupWMClass`, `Categories`, `Name`, `GenericName`, and
   `Comment`. Resolve the localized `Name[lang]` / `Comment[lang]` /
   `GenericName[lang]` variants against the user's stripped `$LANG`.
3. **Filter.** Drop entries where `Type != Application`, `NoDisplay=true`,
   or `Hidden=true`. Record the mtime of the source file.
4. **Cache check.** If `--force` is not set and the cache has an entry
   for this file with a matching mtime, skip it.
5. **Category mapping.** Take the first entry of `Categories=` (split on
   `;`). Strip an `X-` prefix, then strip one of the prefixes
   `GTK`, `Motif`, `GNOME`, `Qt` if present. Sanitize what remains
   (non-alphanumeric characters become `_`, runs of `_` collapse, leading
   and trailing `_` are stripped). Append `_XDG` to the result. If the
   category is empty or the result is empty, use `Other_XDG`.
6. **Action name.** Sanitize the application's `Name` to a valid CDE
   action name (same rules as category sanitization). If a file with
   that name already exists in the group's stub directory, append
   `_<sanitized-id>` to disambiguate. The desktop file id (the basename
   of the file with `.desktop` stripped) is used as the disambiguator.
7. **Icon resolution.** Walk the icon theme directories and `/usr/share/pixmaps`
   looking for the icon in `.png`, `.svg`, `.xpm`, `.jpg`, or `.jpeg`
   form. See "Icon Search" below.
8. **Icon conversion.** For each of the three target sizes (16, 32, 48),
   invoke `convert` (ImageMagick) to write an XPM. If that fails and the
   source is a PNG, fall back to
   `pngtopnm | pnmscale -xysize W H | ppmtoxpm`. The XPM file is renamed
   to `.pm`. If any of the three sizes fails, all created files for the
   entry are rolled back.
9. **Exec translation.** Field codes in `Exec=` (`%f`, `%F`, `%u`, `%U`,
   `%d`, `%D`, `%n`, `%N`, `%i`, `%c`, `%k`, `%v`, `%m`) are translated
   to a CDE-friendly form. Single-URL codes collapse to just the
   argument; `%F`/`%U` keep their multiplicity. `%i` (icon) is replaced
   with `-icon <icon-name>` if an icon was resolved. `Terminal=true`
   wraps the result in a `dtterm -e` invocation.
10. **Emit.** Write the `<Action>.dt` file with `LABEL`, `ICON`,
    `TYPE=COMMAND`, `WINDOW_TYPE=NO_STDIO`, `EXEC_STRING`, and (when
    present) `DESCRIPTION` fields. Create the appmanager stub file.
    Record the group for the later group `.dt` write.
11. **Group actions.** After all entries are processed, write a
    `<Group>_XDG.dt` file for each unique group seen. The file defines a
    `DATA_ATTRIBUTES` block, a `DATA_CRITERIA` block matching
    `*/appmanager/*/<Group>_XDG/`, and an `Open` action of type `MAP`
    pointing at `OpenAppGroup`.
12. **Save cache.** Persist the updated cache unless `--dry-run` was
    given.

### Icon search

For a non-absolute `Icon=` value, the search order is:

1. `$HOME/.local/share/icons/<name>`, then
   `$HOME/.local/share/icons/<size>/<name>`, then
   `$HOME/.local/share/icons/<size>/apps/<name>` (sizes 48x48, 32x32, 16x16)
2. For each XDG data directory: `<data>/icons/<name>` and the same
   size/layout variants as above
3. `$HOME/.icons/<name>` and its size/layout variants
4. `/usr/share/pixmaps/<name>` and its size/layout variants

Each lookup tries the bare name, then the name with each supported
extension (`.png`, `.svg`, `.xpm`, `.jpg`, `.jpeg`). For each size, the
layout tries `<dir>/<size>/<name>`, `<dir>/<size>/apps/<name>`, and
`<dir>/<size>/<name>.<ext>`, `<dir>/<size>/apps/<name>.<ext>`. If the
icon name is an absolute path, it is used as-is and the format is
inferred from the extension.

## XPM Conversion

The XPM converter writes three sizes for each icon, picked from
`size_table` in `xpm_converter.c`:

| Size | Suffix |
|------|--------|
| 16   | `t`    |
| 32   | `m`    |
| 48   | `l`    |

For each size, the converter tries ImageMagick first:

```
convert -background none -resize <W>x<H> <src> <dest>.xpm
```

If `convert` is missing or returns non-zero and the source is a PNG,
the netpbm fallback is used:

```
pngtopnm <src> | pnmscale -xysize <W> <H> | ppmtoxpm > <dest>.xpm
```

The resulting `.xpm` file is renamed to `.pm` (the CDE icon file
extension). If any of the three sizes fails to convert, the
already-created `.pm` files for that entry are removed and the entry
is reported as an error, but processing of other entries continues.

## Examples

### Preview what would be generated

```
dtxdg2appmgr --dry-run -v
```

Lists every data directory, every `.desktop` file that would be
considered, the resolved group, and the target paths for the `.dt` file,
the appmanager stub, and the icon set. Nothing is written.

### Generate to a system-wide location

```
sudo dtxdg2appmgr \
  -o /var/dt/appconfig/appmanager/C \
  -i /usr/dt/appconfig/icons/C \
  -c /var/dt/xdg-cache.db
```

Suitable for a system-wide install where multiple users should see the
same set of XDG applications. The output directory must be writable by
the invoking user.

### Force a full rebuild

```
dtxdg2appmgr -f -v
```

Ignores the mtime cache and regenerates every action file, stub, and
icon. Useful after upgrading many `.desktop` files at once or after
changing the action name sanitization rules.

### Per-user install

Run with no arguments. The defaults place the `.dt` files in
`$HOME/.dt/types/` and the icons in `$HOME/.dt/icons/`, which CDE picks
up automatically. The cache lives at `/var/dt/xdg-cache.db` only if
writable; otherwise the program warns and continues without caching.

### CDE login integration

`dtxdg2appmgr` is invoked from `Xsession` after `dtappgather`. This is
automatic; no operator action is required. To verify the hook is
working, check the `Xsession` script for a line such as
`dtxdg2appmgr`.

### Refresh from the front panel

The CDE `ReloadApps` front panel action runs `dtxdg2appmgr -r` to
rebuild the application set on demand. Again, this is automatic once
CDE is installed and configured.

### Debug icon conversion

```
dtxdg2appmgr -f -v -r 2>&1 | tee /tmp/dtxdg.log
```

The `-r` flag retains the intermediate `.xpm` files the converter
creates, so an icon that ends up looking wrong in CDE can be inspected
before the rename to `.pm`.

## Credits

The category mapping and XDG-to-CDE translation logic is adapted from
`contrib/desktop2dt/desktop2dt` by Isaac Dunham, 2013, MIT license. The
program is rewritten in C, uses GLib for parsing and path handling, and
emits a richer set of artifacts (action files, group actions, three
icon sizes).

## See Also

- `Xsession` (in `cde/programs/dtsession/`) for the login-time invocation
- `dt.dt` (in the CDE appconfig) for the `ReloadApps` action that
  triggers a refresh
- `contrib/desktop2dt/desktop2dt` for the original implementation
- The CDE Application Manager documentation for the format of the
  generated `.dt` files
