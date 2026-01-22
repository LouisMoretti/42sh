# Library Name
lib_LIBRARIES = libiobackend.a

# Source Files
libiobackend_a_SOURCES = \
				   iobackend.c \
				   iobackend.h

# Preprocessing Flags
libiobackend_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libiobackend_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
CFLAGS                = -g

# Libraries to not install
noinst_LIBRARIES = libiobackend.a

if ENABLE_COVERAGE
libiobackend_a_CFLAGS  +=  --coverage
endif

if ENABLE_DEBUG
libiobackend_a_CFLAGS  +=  -fsanitize=address
endif
