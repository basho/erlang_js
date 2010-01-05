# This Makefile builds the dependencies (libjs and libnspr) needed by
# spidermonkey_drv.so

js: ../libjs.a

../libnspr4.a: build/nspr_release/dist/lib/libnspr4.a
	@cp build/nspr_release/dist/lib/libnspr4.a ..

../libjs.a: ../libnspr4.a js/src/libjs.a
	@cp build/js/src/libjs.a ..

build/nspr_release/dist/lib/libnspr4.a: build/mozilla build/nspr_release
	@cd build/nspr_release;../mozilla/nsprpub/configure --disable-debug --enable-optimize;make

build/mozilla:
	@mkdir -p $(@)
	@cvs -q -d :pserver:anonymous@cvs-mirror.mozilla.org:/cvsroot co -r NSPR_4_8_RTM mozilla/nsprpub
	@mv mozilla build

build/nspr_release:
	@mkdir -p $(@)

js/src/libjs.a: build/js
	@cd build/js/src;make BUILD_OPT=1 JS_DIST=../../nspr_release/dist JS_THREADSAFE=1 XCFLAGS="-DHAVE_VA_COPY -DVA_COPY=va_copy" -f Makefile.ref
	@cp build/js/src/*_OPT.OBJ/libjs.a build/js/src
	@mkdir -p include/js;
	@cp build/js/src/*.h include/js
	@cp build/js/src/*.tbl include/js
	@cp build/js/src/*_OPT.OBJ/*.h include/js

build/js:
	@tar -C build -xzf js-1.8.0-rc1.tar.gz

dist:
	@rm -rf *.o *.a $(OUTPUTFILES)
	@rm -rf config.status config.log autom4te.cache
	@rm -f *flymake*
	@rm -rf include
	@rm -rf build

jsclean:
	@rm -rf build
	@rm -f *flymake*
	@rm -f ../*.a
	@rm -rf include
