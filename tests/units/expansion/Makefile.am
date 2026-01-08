# Name of binary
check_PROGRAMS = test_expansion

# Source file
test_expansion_SOURCES = test_expansion.c

# Preprocessing Flags
test_expansion_CPPFLAGS = -I%D%

# Compilation Flags
test_expansion_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_expansion_LDFLAGS = -lcriterion

test_expansion_LDADD =  \
	   $(top_srcdir)/src/expansion/libexpansion.a

check-local:
	./$(check_PROGRAMS)
