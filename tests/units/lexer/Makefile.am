# Name of binary
check_PROGRAMS = test_lexer

# Source file
test_lexer_SOURCES = test_lexer.c

# Preprocessing Flags
test_lexer_CPPFLAGS =  -I$(top_srcdir)/src

# Compilation Flags
test_lexer_CFLAGS  =   -std=c99 -Werror -Wall -Wextra -Wvla
CFLAGS             =   -g

test_lexer_LDFLAGS =   -lcriterion

if ENABLE_COVERAGE
test_lexer_CFLAGS  +=  --coverage
test_lexer_LDFLAGS +=  --coverage
endif

if ENABLE_DEBUG
test_lexer_CFLAGS  +=  -fsanitize=address
test_lexer_LDFLAGS +=  -fsanitize=address
endif

test_lexer_LDADD =  \
	   ../../../src/config/libconfig.a \
	   ../../../src/iobackend/libiobackend.a \
	   ../../../src/lexer/liblexer.a

check-local:
	@(./$(check_PROGRAMS) || true)
