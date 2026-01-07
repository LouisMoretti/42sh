SUBDIRS=src tests

AUTOTOOLTRASH = aclocal.m4 ar-lib compile config.log config.status configure configure~ depcomp install-sh install-sh~ missing

check-local:
	make -C tests/ check

clean:
	$(RM) src/42sh
	$(RM) $(AUTOTOOLTRASH)
	$(RM) -r autom4te.cache
	find . -name "Makefile" -exec rm {} \;
	find . -name "Makefile.in" -exec rm {} \;
