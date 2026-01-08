# Name of binary
check_PROGRAMS = test_iobackend

# Source file
test_iobackend_SOURCES = test_iobackend.c

# Preprocessing Flags
test_iobackend_CPPFLAGS = -I%D%

# Compilation Flags
test_iobackend_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla

test_iobackend_LDFLAGS = -lcriterion

test_iobackend_LDADD =  \
	   $(top_srcdir)/src/iobackend/libiobackend.a

check-local:
	./$(check_PROGRAMS)
