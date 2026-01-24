# Name of binary
check_PROGRAMS = test_expansion

# Source file
test_expansion_SOURCES   =  test_expansion.c

# Preprocessing Flags
test_expansion_CPPFLAGS  =  -I$(top_srcdir)/src

# Compilation Flags
test_expansion_CFLAGS    =  -std=c99 -Werror -Wall -Wextra -Wvla

test_expansion_LDFLAGS   =  -lcriterion

if ENABLE_COVERAGE
test_expansion_CFLAGS   +=  --coverage
test_expansion_LDFLAGS  +=  --coverage
CFLAGS                  +=  -g -O0
endif

if ENABLE_DEBUG
test_expansion_CFLAGS   +=  -fsanitize=address
test_expansion_LDFLAGS  +=  -fsanitize=address
CFLAGS                  +=  -g -O0
endif

test_expansion_LDADD =  \
	   ../../../src/expansion/libexpansion.a \
	   ../../../src/parser/libparser.a \
	   ../../../src/config/libconfig.a \
	   ../../../src/utils/hash_map/libhashmap.a

check-local:
	@(./$(check_PROGRAMS) || true)
