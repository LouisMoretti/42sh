SUBDIRS=functionnal

all:
	make -C ../src/



ifeq ($(COVERAGE), yes)
SUBDIRS=functionnal \
		units
check-local: CFLAGS=-std=c99 -Werror -Wall -Wextra -Wvla -fvisibility=hidden --coverage -fPIC
check-local: LDLIBS=-lcriterion -lgcov
check-local: LDFLAGS=--coverage
check-local: all
	echo "coucou oui"
else
check-local:
	echo "coucou non"
endif
