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

.PHONY: c_src c_src_clean docs build_plt clean_plt

include tools.mk
