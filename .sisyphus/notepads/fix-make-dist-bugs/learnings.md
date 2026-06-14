# Learnings: Fix make dist distribution bugs

## CDE Autotools Distribution Patterns

### Files referenced via `#include "subdir/file"` but missing from EXTRA_DIST
When C source files use `#include "bitmaps/xxx.xbm"` or `#include "pixmaps/xxx.pm"`, those data files must be in `EXTRA_DIST` in the corresponding Makefile.am, or they'll be missing from the `make dist` tarball.

### Files found and fixed:
1. **dtappbuilder/src/ab/Makefile.am**: Added 22 `bitmaps/*.xbm` files to EXTRA_DIST
2. **dtscreen/Makefile.am**: Added `lifeicon.bit` and `xlogo.bit` to EXTRA_DIST  
3. **dtinfo/dtinfo/src/Other/Makefile.am**: Added `detached_bw.xbm`, `detached_bw.xpm`, `graphic_unavailable.xbm`, `graphic_unavailable.xpm` to EXTRA_DIST

### Other missing files found and fixed:
4. **dtlogin/config/Makefile.am**: Added `_common.ksh.src` to EXTRA_DIST (referenced via `#include "_common.ksh.src"` in Xsession.src and Xsetup.src)
5. **dtterm/Makefile.am**: Added `mkfallbk` shell script to EXTRA_DIST (used as build tool `./mkfallbk`)

### Skipped per task instructions:
- **dtudcfonted/pixmaps/**: Has `#include "pixmaps/arrow.pm"` etc. but task says to skip

### Known remaining issue (not in scope):
- **dtksh/ksh93/**: Entire vendored ksh93 source tree (1152 files) not distributed by `make dist`. This causes dtksh build to fail from tarball. This is a pre-existing issue requiring a different fix approach (dist-hook or vendor directory distribution).