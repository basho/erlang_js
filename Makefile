all: compile

tests: compile
	@cd tests;erl -make
	@erl -noshell -boot start_sasl -pa ebin -s erlang_js -eval 'test_suite:test().' -s init stop
	@rm -f ebin/test_* ebin/*_tests.erl

priv/spidermonkey_drv.so:
	@cd priv/src;./configure;make
ebin:
	@mkdir ebin

clean:
	@rm -rf ebin doc
	@cd priv/src;make clean

dist: compile
	@cd priv/src;make dist


distclean: clean
	@cd priv/src;make jsclean
	rm -f priv/src/Makefile
doc:
	@erl -noshell -run edoc_run application 'erlang_js' '"."' '[]'