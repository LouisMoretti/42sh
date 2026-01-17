# Name of binary
check_PROGRAMS = test_expansion

# Source file
test_expansion_SOURCES = test_expansion.c

# Preprocessing Flags
test_expansion_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
test_expansion_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_expansion_LDFLAGS = -lcriterion

test_expansion_LDADD =  \
	   ../../../src/utils/hash_map/libhashmap.a \
	   ../../../src/config/libconfig.a \
	   ../../../src/expansion/libexpansion.a 

check-local:
	(./$(check_PROGRAMS) || true)
