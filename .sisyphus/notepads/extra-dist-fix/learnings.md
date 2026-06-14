# EXTRA_DIST Fix Learnings

## Key Insight
In CDE's autotools build system, files referenced as dependencies in custom build rules (right side of `:`) must be listed in `EXTRA_DIST` or some other distribution variable to be included in `make dist` tarballs. If they're not in `_SOURCES`, `_HEADERS`, `EXTRA_DIST`, `dist_*_DATA`, or `dist_*_SCRIPTS`, they won't be distributed.

## Pattern
- `.cpp` files → tradcpp source inputs (GENCPP preprocessing)
- `.src` files → tradcpp source inputs (GENCPP preprocessing) for config/script generation
- `.yy`/`.y` files → YACC/Bison grammars
- `.ll` files → LEX/Flex scanners
- `.bil`/`.bip` files → dtappbuilder interface/project files
- `.msg` files → gencat message catalog sources
- `.x` files → rpcgen RPC definitions
- `.sed` files → sed scripts used in build rules
- `.data` files → data inputs for code generation tools

## Files Modified (30 total)
### .cpp fixes (2 files):
- programs/dtfile/Makefile.am
- programs/dthelp/parser/helptag/Makefile.am

### .src fixes (21 files):
- programs/types/Makefile.am (30 .src files)
- programs/dtlogin/config/Makefile.am (18 .src files)
- programs/dtsession/Makefile.am (6 .src files)
- programs/dtsession/config/Makefile.am (1 .src file)
- programs/dtstyle/Makefile.am (3 .src files)
- programs/dtwm/Makefile.am (2 .src files)
- programs/dtterm/Makefile.am (3 files: .src + utility scripts)
- programs/dthello/Makefile.am (1 .src file)
- programs/dtspcd/Makefile.am (1 .src file)
- programs/dtsearchpath/Makefile.am (2 .src files)
- programs/dtimsstart/Makefile.am (1 .src file)
- programs/dtprintegrate/Makefile.am (3 .src files)
- programs/dtopen/Makefile.am (1 .src file)
- programs/dtappintegrate/Makefile.am (1 .src file)
- programs/dtscreen/Makefile.am (1 .src file)
- programs/dtprintinfo/Makefile.am (1 .msg.src file)
- programs/dtconfig/sun/Makefile.am (2 .src files)
- programs/dtksh/examples/Makefile.am (18 .src files)
- programs/dtappbuilder/src/ab/Makefile.am (41 .bil/.bip/.src files)
- programs/dtinfo/tools/misc/Makefile.am (1 .src file)
- programs/ttsnoop/Makefile.am (23 .bil/.bip/.C.src/.src files)

### lib/ fixes (5 files):
- lib/DtMmdb/StyleSheet/Makefile.am (.yy/.ll files)
- lib/DtMmdb/compression/Makefile.am (.ll files)
- lib/DtMmdb/schema/Makefile.am (.yy/.ll files)
- lib/DtTerm/Term/Makefile.am (.data file)
- lib/csa/Makefile.am (.y/.x/.sed/wrap files)

### Additional fixes (4 files):
- programs/dtcm/server/Makefile.am (parser.y)
- programs/dtdocbook/infolib/Makefile.am (.yy/.ll files)
- programs/dtappbuilder/src/abmf/Makefile.am (.msg file)
- programs/dtappbuilder/src/libAButil/Makefile.am (.msg file)
- programs/dtappbuilder/src/libABobjXm/Makefile.am (.msg file)
- programs/dtappbuilder/src/libABil/Makefile.am (.msg file)