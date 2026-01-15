SUBDIRS=src tests

export BIN_PATH OUTPUT_FILE COVERAGE 

MAKEFLAGS=--no-print-directory
AUTOTOOLTRASH = aclocal.m4 ar-lib compile config.log config.status configure configure~ depcomp install-sh install-sh~ missing

clean: clean-recursive
	(find . -name "__pycache__" -exec rm -rf {} \; 2>/dev/null || true)
	(find . -name ".pytest_cache" -exec rm -rf {} \; 2>/dev/null || true)
	(find . -name ".deps" -exec rm -rf {} \; 2>/dev/null || true)

clean-conf:
	find . -name "Makefile.in" -exec rm {} \;
	$(RM) $(AUTOTOOLTRASH)
	$(RM) -r autom4te.cache
	find . -name "Makefile" -exec rm {} \;

