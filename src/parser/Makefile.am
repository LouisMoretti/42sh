# Library Name
lib_LIBRARIES = libparser.a

# Source Files
libparser_a_SOURCES = \
				   parser.c \
				   parser.h \
				   ast.c \
				   ast.h

# Preprocessing Flags
libparser_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libparser_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = libparser.a
