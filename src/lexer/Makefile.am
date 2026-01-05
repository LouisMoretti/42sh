# Library Name
lib_LIBRARIES = liblexer.a

# Source Files
liblexer_a_SOURCES = \
				   lexer.c \
				   lexer.h \
				   token.c \
				   token.h

# Preprocessing Flags
liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
liblexer_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = liblexer.a
