# Name of binary
check_PROGRAMS = test_parser

# Source file
test_parser_SOURCES = test_parser.c

# Preprocessing Flags
test_parser_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
test_parser_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_parser_LDFLAGS = -lcriterion

if ENABLE_COVERAGE
test_parser_CFLAGS  +=  --coverage -O0
test_parser_LDFLAGS +=  --coverage
endif

test_parser_LDADD =  \
	   ../../../src/parser/libparser.a

check-local:
	@(./$(check_PROGRAMS) || true)
