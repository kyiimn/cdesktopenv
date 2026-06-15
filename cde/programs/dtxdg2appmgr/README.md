# dtxdg2appmgr

Convert XDG `.desktop` application entries into CDE Application Manager actions.

## Overview

`dtxdg2appmgr` scans XDG data directories for `.desktop` files, parses their
key/value pairs, resolves icons, and generates CDE `.dt` action files and
Application Manager stubs. This allows XDG-compliant applications to appear
in the CDE Application Manager without manual configuration.

## Build Requirements

- GLib 2.0 (detected by configure via `pkg-config`)
- CDE build environment (autotools)

## Build

This program is only built when GLib 2.0 is detected at configure time:

```
./configure
make
```

If GLib is not found, `dtxdg2appmgr` is silently skipped.

## Usage

```
dtxdg2appmgr [OPTIONS]

Options:
  --output-dir DIR    CDE appconfig output directory
  --cache-file FILE   Cache file path
  --verbose           Verbose output
  --force             Force regeneration
  --dry-run           Show what would be done
```