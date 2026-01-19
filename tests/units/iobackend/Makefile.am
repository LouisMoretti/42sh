# Name of binary
check_PROGRAMS = test_iobackend

# Source file
test_iobackend_SOURCES = test_iobackend.c

# Preprocessing Flags
test_iobackend_CPPFLAGS = -I$(top_srcdir)/src

# Compilation Flags
test_iobackend_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_iobackend_LDFLAGS = -lcriterion

if ENABLE_COVERAGE
test_iobackend_CFLAGS  +=  --coverage -O0
test_iobackend_LDFLAGS +=  --coverage
endif

test_iobackend_LDADD =  \
	   ../../../src/config/libconfig.a \
	   ../../../src/iobackend/libiobackend.a

check-local:
	@(./$(check_PROGRAMS) || true)
