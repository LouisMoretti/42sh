# Name of binary
check_PROGRAMS = test_parser

# Source file
test_parser_SOURCES = test_parser.c

# Preprocessing Flags
test_parser_CPPFLAGS = -I%D%

# Compilation Flags
test_parser_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_parser_LDFLAGS = -lcriterion

test_parser_LDADD =  \
	   $(top_srcdir)/src/parser/libparser.a

check-local:
	./$(check_PROGRAMS)
