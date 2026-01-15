SUBDIRS=

check-local:
	@if [ "${COVERAGE}" = "yes" ]; then make -C functionnal/ check; fi
	@make -C units/ check

check-cov: CFLAGS=-std=c99 -Werror -Wall -Wextra -Wvla --coverage -fPIC
check-cov: LDLIBS=-lcriterion -lgcov
check-cov: LDFLAGS=--coverage
check-cov:
