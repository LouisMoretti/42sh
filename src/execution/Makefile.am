# Library Name
lib_LIBRARIES = libexecution.a

# Source Files
libexecution_a_SOURCES = \
				   execution.c \
				   execution.h

# Preprocessing Flags
libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
libexecution_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

# Libraries to not install
noinst_LIBRARIES = libexecution.a
