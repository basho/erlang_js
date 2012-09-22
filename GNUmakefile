
all:
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

test: all
	$(REBAR) eunit

docs: all
	@mkdir -p docs
	@./build_docs.sh

.PHONY: c_src c_src_clean docs

include rebar.mk
