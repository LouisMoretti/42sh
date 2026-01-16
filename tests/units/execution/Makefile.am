# Name of binary
check_PROGRAMS = test_execution

# Source file
test_execution_SOURCES = test_execution.c

# Preprocessing Flags
test_execution_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
test_execution_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_execution_LDFLAGS = -lcriterion

test_execution_LDADD = \
	../../../src/execution/libexecution.a \
	../../../src/expansion/libexpansion.a

check-local:
	(./$(check_PROGRAMS) || true)
