# Library Name
lib_LIBRARIES = libparser.a

# Source Files
libparser_a_SOURCES = \
				   parser.c \
				   parser.h \
				   init_ast.c \
				   free_ast.c \
				   ast.h \
				   pretty_print.c \
				   pretty_print.h

# Preprocessing Flags
libparser_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libparser_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = libparser.a

if ENABLE_COVERAGE
libparser_a_CFLAGS  +=  --coverage
CFLAGS              +=  -g -O0
endif

if ENABLE_DEBUG
libparser_a_CFLAGS  +=  -fsanitize=address
CFLAGS              +=  -g -O0
endif
