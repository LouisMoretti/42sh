check-local:
	@if [ "${OUTPUT_FILE}" != "" ]; then if [ ! -d ../../output ]; then mkdir ../../output; fi fi
	@if [ "${OUTPUT_FILE}" != "" ]; then pytest --junitprefix=42sh \
		--junitxml=../../output/${TESTSRESULTS} || true; python win_rate.py 2>/dev/null || true; \
		else pytest || true; fi

generate-test-report:
	@if [ ! test -d ../../output ]; then mkdir ../../output; fi
	@pytest --junitprefix=42sh --junitxml=../../output/${TESTSRESULTS} || true
