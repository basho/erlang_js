# This is a stub Makefile that invokes GNU make which will read the GNUmakefile
# instead of this file. This provides compatability on systems where GNU make is
# not the system 'make' (eg. most non-linux UNIXes).

REBAR ?= $(shell which rebar 2>/dev/null || which ./rebar)

all:
	$(REBAR) compile

verbose:
	$(REBAR) compile verbose=1

check: test
test: all
	$(REBAR) eunit

docs:
	$(REBAR) doc

clean:
	$(REBAR) clean
