MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = dtudcexch.msg

# dtudcexch depends on dtudcfonted internal libraries (liboakfuty.a, libfal.a)
# which are not built by the autotools system, so this program cannot be compiled.
# Distribute source files only.

EXTRA_DIST += udcexc.c udcexp.c udcimp.c getbdffn.c excutil.c \
              selectx.c exportbdf.c importbdf.c excutil.h