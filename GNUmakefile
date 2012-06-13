
all:
	$(REBAR) compile

verbose:
	$(REBAR) compile verbose=1

clean: c_src_clean
	rm -rf tests_ebin docs
	$(REBAR) clean

c_src:
	cd c_src; $(MAKE) js

c_src_clean:
	cd c_src; $(MAKE) clean

test: all
	@mkdir -p tests_ebin
	@cd tests;erl -make
	@erl -noshell -boot start_sasl -pa ebin -pa tests_ebin -s erlang_js -eval 'test_suite:test().' -s init stop
	@rm -f ebin/test_* ebin/*_tests.erl

docs: all
	@mkdir -p docs
	@./build_docs.sh

.PHONY: c_src c_src_clean docs

include rebar.mk
