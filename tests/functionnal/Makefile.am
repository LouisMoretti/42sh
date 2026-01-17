check-local:
	@pytest -rA || true
	@if [ "${OUTPUT_FILE}" != "" ]; then python win_rate.py; fi

generate-test-report:
	@if [ ! test -d ../../output ]; then mkdir ../../output; fi
	@pytest --junitprefix=42sh --junitxml=../../output/${TESTSRESULTS} || true
