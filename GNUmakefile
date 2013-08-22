all: deps compile

deps:
	$(REBAR) get-deps

compile:
	$(REBAR) compile

verbose:
	$(REBAR) compile verbose=1

clean: c_src_clean
	rm -rf tests_ebin docs
	$(REBAR) clean

c_src:
	cd c_src; $(MAKE)

c_src_clean:
	cd c_src; $(MAKE) clean

distclean: clean
	$(REBAR) delete-deps

test: all
	$(REBAR) eunit

docs: all
	@mkdir -p docs
	@./build_docs.sh

.PHONY: c_src c_src_clean docs build_plt clean_plt

include rebar.mk

APPS = kernel stdlib sasl erts ssl tools os_mon runtime_tools crypto inets \
	xmerl webtool snmp public_key mnesia eunit syntax_tools compiler
COMBO_PLT = $(HOME)/.erlang_js_dialyzer_plt

check_plt: compile
	dialyzer --check_plt --plt $(COMBO_PLT) --apps $(APPS) ebin

build_plt: compile
	dialyzer --build_plt --output_plt $(COMBO_PLT) --apps $(APPS) ebin

dialyzer: compile
	@echo
	@echo Use "'make check_plt'" to check PLT prior to using this target.
	@echo Use "'make build_plt'" to build PLT prior to using this target.
	@echo
	@sleep 1
	dialyzer -Wunmatched_returns -Werror_handling -Wrace_conditions \
		-Wunderspecs --plt $(COMBO_PLT) ebin

cleanplt:
	@echo
	@echo "Are you sure?  It takes about 1/2 hour to re-build."
	@echo Deleting $(COMBO_PLT) in 5 seconds.
	@ech
	sleep 5
	rm $(COMBO_PLT)
