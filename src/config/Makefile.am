# Library Name
lib_LIBRARIES = libconfig.a

# Source Files
libconfig_a_SOURCES = \
				   config.c \
				   config.h

# Preprocessing Flags
libconfig_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libconfig_a_CFLAGS =  -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = libconfig.a

if ENABLE_COVERAGE
libconfig_a_CFLAGS  +=  --coverage
CFLAGS              +=  -g -O0
endif

if ENABLE_DEBUG
libconfig_a_CFLAGS  +=  -fsanitize=address
CFLAGS              +=  -g -O0
endif
