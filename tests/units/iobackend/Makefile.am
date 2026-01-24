# Name of binary
check_PROGRAMS = test_iobackend

# Source file
test_iobackend_SOURCES  =  test_iobackend.c

# Preprocessing Flags
test_iobackend_CPPFLAGS =  -I$(top_srcdir)/src

# Compilation Flags
test_iobackend_CFLAGS   =  -std=c99 -Werror -Wall -Wextra -Wvla

test_iobackend_LDFLAGS  =  -lcriterion

if ENABLE_COVERAGE
test_iobackend_CFLAGS  +=  --coverage
test_iobackend_LDFLAGS +=  --coverage
CFLAGS                 +=  -g -O0
endif

if ENABLE_DEBUG
test_iobackend_CFLAGS  +=  -fsanitize=address
test_iobackend_LDFLAGS +=  -fsanitize=address
CFLAGS                 +=  -g -O0
endif

test_iobackend_LDADD =  \
	   ../../../src/config/libconfig.a \
	   ../../../src/iobackend/libiobackend.a

check-local:
	@(./$(check_PROGRAMS) || true)
