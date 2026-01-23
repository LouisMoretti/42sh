# Sub directories
SUBDIRS = parser \
		  config \
		  lexer \
		  execution \
		  expansion \
		  iobackend \
		  utils/hash_map			

# Name of binary
bin_PROGRAMS = 42sh

# Source file
42sh_SOURCES = 42sh.c

# Preprocessing Flags
42sh_CPPFLAGS = -I%D%

# Compilation Flags
42sh_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
CFLAGS      = -g

42sh_LDADD =  \
       parser/libparser.a \
	   lexer/liblexer.a \
	   config/libconfig.a \
	   execution/libexecution.a \
	   expansion/libexpansion.a \
	   iobackend/libiobackend.a \
	   utils/hash_map/libhashmap.a

if ENABLE_COVERAGE
42sh_CFLAGS  +=  --coverage
42sh_LDFLAGS  =  --coverage
endif

if ENABLE_DEBUG
42sh_CFLAGS  +=  -fsanitize=address
42sh_LDFLAGS  =  -fsanitize=address
endif
