# Library Name
lib_LIBRARIES = liblexer.a

# Source Files
liblexer_a_SOURCES = \
				   lexer.c \
				   lexer.h

# Preprocessing Flags
liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
liblexer_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = liblexer.a

if ENABLE_COVERAGE
liblexer_a_CFLAGS  +=  --coverage
CFLAGS             +=  -g -O0
endif

if ENABLE_DEBUG
liblexer_a_CFLAGS  +=  -fsanitize=address
CFLAGS             +=  -g -O0
endif
