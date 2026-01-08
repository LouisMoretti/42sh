SUBDIRS=src tests

AUTOTOOLTRASH = aclocal.m4 ar-lib compile config.log config.status configure configure~ depcomp install-sh install-sh~ missing

clean: clean-recursive
	(find . -name "__pycache__" -exec rm -rf {} \; || true)
	(find . -name ".pytest_cache" -exec rm -rf {} \; || true)
	(find . -name ".deps" -exec rm -rf {} \; || true)
	find . -name "Makefile.in" -exec rm {} \;
	$(RM) $(AUTOTOOLTRASH)
	$(RM) -r autom4te.cache
	find . -name "Makefile" -exec rm {} \;
