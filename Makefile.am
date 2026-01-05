SUBDIRS=src

AUTOTOOLTRASH = aclocal.m4 ar-lib compile config.log config.status configure configure~ depcomp install-sh install-sh~ missing

all:
	make -C src/

check:

clean:
	$(RM) src/42sh
	$(RM) $(AUTOTOOLTRASH)
	$(RM) -r autom4te.cache
	find . -name "Makefile" -exec rm {} \;
	find . -name "Makefile.in" -exec rm {} \;
