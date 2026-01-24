# Library Name
lib_LIBRARIES = libexpansion.a

# Source Files
libexpansion_a_SOURCES = \
				   expansion.c \
				   expansion.h

# Preprocessing Flags
libexpansion_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libexpansion_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = libexpansion.a

if ENABLE_COVERAGE
libexpansion_a_CFLAGS  +=  --coverage
CFLAGS                 +=  -g -O0
endif

if ENABLE_DEBUG
libexpansion_a_CFLAGS  +=  -fsanitize=address
CFLAGS                 +=  -g -O0
endif
