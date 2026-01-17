# Name of binary
check_PROGRAMS = test_expansion

# Source file
test_expansion_SOURCES = test_expansion.c

# Preprocessing Flags
test_expansion_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
test_expansion_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_expansion_LDFLAGS = -lcriterion

if ENABLE_COVERAGE
test_expansion_CFLAGS  +=  --coverage -O0
test_expansion_LDFLAGS +=  --coverage
endif

test_expansion_LDADD =  \
	   ../../../src/expansion/libexpansion.a

check-local:
	@(./$(check_PROGRAMS) || true)
