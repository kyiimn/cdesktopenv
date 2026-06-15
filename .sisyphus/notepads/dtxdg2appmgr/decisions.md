# Decisions - dtxdg2appmgr

## 2026-06-15

- Language: C + GLib (not C++, not Python, not ksh)
- `_XDG` suffix on ALL XDG group names (not XDG_ prefix, not conditional)
- Icon sizes: 3 sizes (.t.pm/.m.pm/.l.pm) per CDE standard
- SVG handled by ImageMagick's librsvg delegate (no separate dependency)
- Algorithm ported from desktop2dt shell script (not shell-out, not independent)
- Cache: mtime-based incremental updates (INI format via GKeyFile)
- Output: `/var/dt/appconfig/appmanager/$DTUSERSESSION/<Category>_XDG/<AppName>`