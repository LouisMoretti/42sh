# Library Name
lib_LIBRARIES = libhashmap.a

# Source Files
libhashmap_a_SOURCES = \
				   hash_map.c \
				   hash.c \
				   hash_map.h

# Preprocessing Flags
libhashmap_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libhashmap_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = libhashmap.a
