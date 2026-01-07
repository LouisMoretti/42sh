SUBDIRS=functionnal

all:
	make -C ../src/

build-env:
	python -m venv env && . env/bin/activate && pip install -r requirements.txt && deactivate
	
check-local: build-env
	
check-cov: CFLAGS=-std=c99 -Werror -Wall -Wextra -Wvla -fvisibility=hidden --coverage -fPIC
check-cov: LDLIBS=-lcriterion -lgcov
check-cov: LDFLAGS=--coverage
check-cov:
