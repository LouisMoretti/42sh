# Library Name
lib_LIBRARIES = libexecution.a

# Source Files
libexecution_a_SOURCES = \
						 execution_loop.c \
						 execution_loop.h \
						 echo.c \
						 boolean.c \
						 cd.c \
						 exit.c \
						 dot.c \
						 builtin.h \
						 pipe.c \
						 pipe.h \
						 redirection.c \
						 redirection.h \
						 execution.c \
						 execution.h

# Preprocessing Flags
libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libexecution_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = libexecution.a

if ENABLE_COVERAGE
libexecution_a_CFLAGS  +=  --coverage
CFLAGS                 +=  -g -O0
endif

if ENABLE_DEBUG
libexecution_a_CFLAGS  +=  -fsanitize=address
CFLAGS                 +=  -g -O0
endif
