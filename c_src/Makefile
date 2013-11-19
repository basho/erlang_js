# This Makefile builds the dependencies (libjs and libnspr) needed by
# spidermonkey_drv.so

UNAME	:= $(shell uname -s)
TAR		?= tar
GUNZIP		?= gunzip
SMONKEY_VER	:= mozjs17.0.0
NSPR_VER	:= 4.9.3

ifeq ($(UNAME),SunOS)
	PATCH	?= gpatch
else
	PATCH	?= patch
endif

SYSTEM_DIR := $(CURDIR)/system
LIB_DIR    := $(SYSTEM_DIR)/lib
INC_DIR    := $(SYSTEM_DIR)/include

JS_DIR_BASE := $(CURDIR)/js
JS_DIR      := $(JS_DIR_BASE)/js
NSPR_DIR    := $(CURDIR)/nsprpub

# NSPR_SIXTYFOUR is defined in erlang_js/rebar.config

js: $(LIB_DIR)/libjs.a

$(LIB_DIR)/libjs.a: $(LIB_DIR)/libnspr4.a
	$(GUNZIP) -c $(SMONKEY_VER).tar.gz | $(TAR) xf -
	mv $(SMONKEY_VER) js
#	@for I in patches/js-*.patch; do \
#		($(PATCH) -p1 < $${I} || echo "Skipping patch"); \
#	done
	(cd $(JS_DIR)/src/ && ./configure --enable-threadsafe --with-nspr-cflags='-I$(INC_DIR)/nspr' --with-nspr-libs="-L$(LIB_DIR) -lplds4 -lplc4 -lnspr4 -lpthread -ldl -lm -lz -ldl")

	@$(MAKE) -C $(JS_DIR)/src BUILD_OPT=1 JS_DIST=$(SYSTEM_DIR) \
		JS_THREADSAFE=1 \
		XCFLAGS="-DHAVE_VA_COPY -DVA_COPY=va_copy $(CFLAGS)" \
		XLDFLAGS="$(LDFLAGS) -lplds4 -lplc4 -lnspr4 -lpthread -ldl -lm -lz -ldl -lstdc++"
	@mkdir $(INC_DIR)/js
	@cp $(JS_DIR)/src/*.h $(INC_DIR)/js
	@cp -r $(JS_DIR)/src/gc $(INC_DIR)/js
	@cp -r $(JS_DIR)/src/ds $(INC_DIR)/js
	@cp -r $(JS_DIR)/public/ $(INC_DIR)/js/js/
	@cp -r $(JS_DIR_BASE)/mfbt $(INC_DIR)/js/mozilla
	@cp $(JS_DIR)/src/*.tbl $(INC_DIR)/js
#	@cp $(JS_DIR)/src/*_OPT.OBJ/*.h $(INC_DIR)/js
	@cp $(JS_DIR)/src/libjs_static.a $(LIB_DIR)/libjs.a

$(LIB_DIR)/libnspr4.a:
	$(GUNZIP) -c nspr-$(NSPR_VER).tar.gz | $(TAR) xf -
	mv nspr-$(NSPR_VER)/mozilla/nsprpub .
	rm -rf nspr-$(NSPR_VER)
#	@for I in patches/nspr-*.patch; do \
#		($(PATCH) -p1 < $${I} || echo "Skipping patch"); \
#	done
	(cd $(NSPR_DIR) && \
	 ./configure --disable-debug --enable-optimize \
                     --prefix=$(SYSTEM_DIR) $(NSPR_SIXTYFOUR) && \
         $(MAKE) all install)

clean:
	@rm -rf $(SYSTEM_DIR) $(NSPR_DIR) $(JS_DIR_BASE)
	@rm -rf *flymake*

nspryoink: deps
	@cd deps ; \
	@cvs -q -d :pserver:anonymous@cvs-mirror.mozilla.org:/cvsroot \
		co -r NSPR_4_8_RTM -d nsprpub mozilla/nsprpub

.EXPORT_ALL_VARIABLES:
