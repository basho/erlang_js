# This Makefile builds the dependencies (libjs and libnspr) needed by
# spidermonkey_drv.so

TAR		?= tar
GUNZIP		?= gunzip
#SMONKEY_VER	:= 1.8.0-rc1
NSPR_VER	:= 4.8

SYSTEM_DIR := $(CURDIR)/system
LIB_DIR    := $(SYSTEM_DIR)/lib
INC_DIR    := $(SYSTEM_DIR)/include

JS_DIR     := $(CURDIR)/js
NSPR_DIR   := $(CURDIR)/nsprpub

# NSPR_SIXTYFOUR is defined in erlang_js/rebar.config

js: $(LIB_DIR)/libjs_static.a

$(LIB_DIR)/libjs_static.a: $(LIB_DIR)/libnspr4.a
	$(GUNZIP) -c jaegermonkey.tar.gz | $(TAR) xf -
	@cd $(JS_DIR)/src;ac_cv_visibility_pragma=no ./configure --enable-threadsafe --enable-static \
                                      --disable-tests --with-nspr-cflags="-I$(INC_DIR)/nspr" \
                                      --with-nspr-libs="$(LIB_DIR)/libnspr4.a"
	@$(MAKE) -C $(JS_DIR)/src
	@mkdir $(INC_DIR)/js
	@cp $(JS_DIR)/src/*.h $(INC_DIR)/js
	@cp $(JS_DIR)/src/*.tbl $(INC_DIR)/js
	@cp $(JS_DIR)/src/libjs_static.a $(LIB_DIR)
	@rm -rf $(JS_DIR)/src/dist

$(LIB_DIR)/libnspr4.a:
	$(GUNZIP) -c nsprpub-$(NSPR_VER).tar.gz | $(TAR) xf -
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
