SUBDIRS=

check-local:
	@make -C functionnal/ check $(MAKEFLAGS)
	@if [ "${COVERAGE}" = "yes" ]; then \
		make -C units/ check $(MAKEFLAGS); \
	fi

clean-local:
	@make -C units/ clean $(MAKEFLAGS)
