# This Makefile builds the dependencies (libjs and libnspr) needed by
# spidermonkey_drv.so

UNAME	:= $(shell uname -s)
TAR		?= tar
GUNZIP		?= gunzip
SMONKEY_VER	:= 1.8.0-rc1
NSPR_VER	:= 4.8

ifeq ($(UNAME),SunOS)
	PATCH	?= gpatch
else
	PATCH	?= patch
endif

SYSTEM_DIR := $(CURDIR)/system
LIB_DIR    := $(SYSTEM_DIR)/lib
INC_DIR    := $(SYSTEM_DIR)/include

JS_DIR     := $(CURDIR)/js
NSPR_DIR   := $(CURDIR)/nsprpub

# NSPR_SIXTYFOUR is defined in erlang_js/rebar.config

js: $(LIB_DIR)/libjs.a

$(LIB_DIR)/libjs.a: $(LIB_DIR)/libnspr4.a
	$(GUNZIP) -c js-$(SMONKEY_VER).tar.gz | $(TAR) xf -
	@for I in patches/js-*.patch; do \
		($(PATCH) -p1 < $${I} || echo "Skipping patch"); \
	done
	@$(MAKE) -C $(JS_DIR)/src BUILD_OPT=1 JS_DIST=$(SYSTEM_DIR) \
		JS_THREADSAFE=1 \
		XCFLAGS="-DHAVE_VA_COPY -DVA_COPY=va_copy $(CFLAGS)" \
		XLDFLAGS="$(LDFLAGS)" \
		-f Makefile.ref
	@mkdir $(INC_DIR)/js
	@cp $(JS_DIR)/src/*.h $(INC_DIR)/js
	@cp $(JS_DIR)/src/*.tbl $(INC_DIR)/js
	@cp $(JS_DIR)/src/*_OPT.OBJ/*.h $(INC_DIR)/js
	@cp $(JS_DIR)/src/*_OPT.OBJ/libjs.a $(LIB_DIR)

$(LIB_DIR)/libnspr4.a:
	$(GUNZIP) -c nsprpub-$(NSPR_VER).tar.gz | $(TAR) xf -
	@for I in patches/nspr-*.patch; do \
		($(PATCH) -p1 < $${I} || echo "Skipping patch"); \
	done
	(cd $(NSPR_DIR) && \
	 ./configure --disable-debug --enable-optimize \
                     --prefix=$(SYSTEM_DIR) $(NSPR_SIXTYFOUR) && \
         $(MAKE) all install)

clean:
	@rm -rf $(SYSTEM_DIR) $(NSPR_DIR) $(JS_DIR)
	@rm -rf *flymake*

nspryoink: deps
	@cd deps ; \
	@cvs -q -d :pserver:anonymous@cvs-mirror.mozilla.org:/cvsroot \
		co -r NSPR_4_8_RTM -d nsprpub mozilla/nsprpub

.EXPORT_ALL_VARIABLES:
