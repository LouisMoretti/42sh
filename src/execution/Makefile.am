# Library Name
lib_LIBRARIES = libexecution.a

# Source Files
libexecution_a_SOURCES = \
						 echo.c \
						 boolean.c \
						 builtin.h \
						 pipe.c \
						 pipe.h \
						 execution.c \
						 execution.h

# Preprocessing Flags
libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libexecution_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = libexecution.a

if ENABLE_COVERAGE
libexecution_a_CFLAGS  +=  --coverage -O0
endif
