# Init 42sh Project
AC_INIT([42sh], [1.0], [arnaud.bellicha@epita.fr, clement.bisiaux@epita.fr, louis.moretti@epita.fr, yassine.tlich@epita.fr])

# Setup Automake
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

# Pretty display of Makefile rules
AM_SILENT_RULES([yes])

# Enable ar for Makefile
AM_PROG_AR

# Check if ranlib is available
AC_PROG_RANLIB

# Check if a C compiler is available
AC_PROG_CC

AX_COMPILER_FLAGS([], [], [], [-std=c99 -pedantic -Werror -Wall -Wextra -Wvla])

AC_ARG_ENABLE([coverage],
  AS_HELP_STRING([--enable-coverage],
                 [enable code coverage])
)
AS_IF([test "$enable_coverage" = yes], [
       AC_DEFINE([ENABLE_COVERAGE], [1], [Set 'ENABLE_COVERAGE' variable to enable coverage])
       ])

AM_CONDITIONAL([ENABLE_COVERAGE], [test "$enable_coverage" = yes])
AM_EXTRA_RECURSIVE_TARGETS([clean-coverage distclean-coverage])

# List Makefiles in subdirectories
AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 src/parser/Makefile
                 src/lexer/Makefile
                 src/execution/Makefile
                 src/expansion/Makefile
                 src/iobackend/Makefile
                 src/config/Makefile
                 src/utils/hash_map/Makefile
                 tests/Makefile
                 tests/functionnal/Makefile
                 tests/units/Makefile
                 tests/units/iobackend/Makefile
                 tests/units/lexer/Makefile
                 tests/units/parser/Makefile
                 tests/units/expansion/Makefile
                 tests/units/execution/Makefile
                 ])

AC_OUTPUT
