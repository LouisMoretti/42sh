SUBDIRS=functionnal

check-local:
	
check-cov: CFLAGS=-std=c99 -Werror -Wall -Wextra -Wvla --coverage -fPIC
check-cov: LDLIBS=-lcriterion -lgcov
check-cov: LDFLAGS=--coverage
check-cov:
