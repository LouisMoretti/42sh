SUBDIRS=src tests

MAKEFLAGS     =  --no-print-directory
AUTOTOOLTRASH =  aclocal.m4 ar-lib compile config.log config.status configure configure~ depcomp install-sh install-sh~ missing
TESTSRESULTS  =  results.xml
COVERAGEFILE  =  coverage

export MAKEFLAGS BIN_PATH OUTPUT_FILE COVERAGE TESTSRESULTS 

generate-test-report: all
	@make -C tests/functionnal generate-test-report $(MAKEFLAGS)

generate-coverage: check
	@echo Generate coverage report...
	@gcovr -r src/ --html --html-details -o $(COVERAGEFILE).html 2>/dev/null || echo "'gcovr'" is not installed or failed
	@mkdir output/ &>/dev/null || true
	@mv $(COVERAGEFILE).* output/ &>/dev/null || true

clean: clean-recursive clean-reports
	@(find . -name "__pycache__" -exec rm -rf {} \; 2>/dev/null || true)
	@(find . -name ".pytest_cache" -exec rm -rf {} \; 2>/dev/null || true)
	@(find . -name ".deps" -exec rm -rf {} \; 2>/dev/null || true)
	@(find . -name "*.gcno" -exec rm -f {} \; 2>/dev/null || true)
	@(find . -name "*.gcda" -exec rm -f {} \; 2>/dev/null || true)

clean-reports:
	@$(RM) -r output/

clean-conf:
	@find . -name "Makefile.in" -exec rm {} \; || true
	@$(RM) $(AUTOTOOLTRASH)
	@$(RM) -r autom4te.cache
	@find . -name "Makefile" -exec rm {} \; || true
