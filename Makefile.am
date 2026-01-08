SUBDIRS=src tests

AUTOTOOLTRASH = aclocal.m4 ar-lib compile config.log config.status configure configure~ depcomp install-sh install-sh~ missing

clean: clean-recursive
	$(RM) $(AUTOTOOLTRASH)
	$(RM) -r autom4te.cache
	find . -name "__pycache__" -exec rm -r {} \;
	find . -name "Makefile.in" -exec rm {} \;
	find . -name "Makefile" -exec rm {} \;
