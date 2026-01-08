# Name of binary
check_PROGRAMS = test_lexer

# Source file
test_lexer_SOURCES = test_lexer.c

# Preprocessing Flags
test_lexer_CPPFLAGS = -I%D%

# Compilation Flags
test_lexer_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_lexer_LDFLAGS = -lcriterion

test_lexer_LDADD =  \
	   $(top_srcdir)/src/lexer/liblexer.a

check-local:
	./$(check_PROGRAMS)
