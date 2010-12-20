/* author Kevin Smith <ksmith@basho.com>
   copyright 2009-2010 Basho Technologies

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <erl_driver.h>

#include "jaegermonkey.h"

/* The class of the global object. */
static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS};

char *copy_string(const char *source) {
  int size = strlen(source) + 1;
  char *retval = driver_alloc(size);
  memset(retval, 0, size);
  strncpy(retval, source, size - 1);
  return retval;
}

char *copy_jsstring(JSString *source) {
  char *buf = JS_GetStringBytes(source);
  return copy_string(buf);
}

inline void begin_request(jaegermonkey_vm *vm) {
  JS_SetContextThread(vm->context);
  JS_BeginRequest(vm->context);
}

inline void end_request(jaegermonkey_vm *vm) {
  JS_EndRequest(vm->context);
  JS_ClearContextThread(vm->context);
}

void on_error(JSContext *context, const char *message, JSErrorReport *report) {
  if (report->flags & JSREPORT_EXCEPTION) {
    jaegermonkey_error *sm_error = (jaegermonkey_error *) driver_alloc(sizeof(jaegermonkey_error));
    if (message != NULL) {
      sm_error->msg = copy_string(message);
    }
    else {
      sm_error->msg = copy_string("undefined error");
    }
    sm_error->lineno = report->lineno;
    if (report->linebuf != NULL) {
      sm_error->offending_source = copy_string(report->linebuf);
    }
    else {
      sm_error->offending_source = copy_string("unknown");
    }
    JS_SetContextPrivate(context, sm_error);
  }
}

void write_timestamp(FILE *fd) {
  struct tm *tmp;
  time_t t;

  t = time(NULL);
  tmp = localtime(&t); /* or gmtime, if you want GMT^H^H^HUTC */
  fprintf(fd, "%02d/%02d/%04d (%02d:%02d:%02d): ",
	  tmp->tm_mon+1, tmp->tm_mday, tmp->tm_year+1900,
	  tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}

JSBool js_log (JSContext *cx, uintN argc, jsval *vp) {
  if (argc != 2) {
    JS_SET_RVAL(cx, vp, JSVAL_FALSE);
  }
  else {
    jsval *argv = JS_ARGV(cx, vp);
    jsval jsfilename = argv[0];
    jsval jsoutput = argv[1];
    char *filename = JS_GetStringBytes(JS_ValueToString(cx, jsfilename));
    char *output = JS_GetStringBytes(JS_ValueToString(cx, jsoutput));
    FILE *fd = fopen(filename, "a+");
    if (fd != NULL) {
      write_timestamp(fd);
      fwrite(output, 1, strlen(output), fd);
      fwrite("\n", 1, strlen("\n"), fd);
      fclose(fd);
      JS_SET_RVAL(cx, vp, JSVAL_TRUE);
    }
    else {
      JS_SET_RVAL(cx, vp, JSVAL_FALSE);
    }
  }
  return JS_TRUE;
}

void sm_configure_locale() {
  JS_SetCStringsAreUTF8();
}

jaegermonkey_vm *sm_initialize(long thread_stack, long heap_size) {
  jaegermonkey_vm *vm = (jaegermonkey_vm*) driver_alloc(sizeof(jaegermonkey_vm));
  int gc_size = (int) heap_size * 0.25;
  vm->runtime = JS_NewRuntime(MAX_GC_SIZE);
  JS_SetGCParameter(vm->runtime, JSGC_MAX_BYTES, heap_size);
  JS_SetGCParameter(vm->runtime, JSGC_MAX_MALLOC_BYTES, gc_size);
  vm->context = JS_NewContext(vm->runtime, 8192);
  JS_SetScriptStackQuota(vm->context, thread_stack);
  begin_request(vm);
  JS_SetOptions(vm->context, JSOPTION_VAROBJFIX);
  JS_SetOptions(vm->context, JSOPTION_STRICT);
  JS_SetOptions(vm->context, JSOPTION_COMPILE_N_GO);
  JS_SetOptions(vm->context, JSVERSION_LATEST);
  vm->global = JS_NewGlobalObject(vm->context, &global_class);
  JS_InitStandardClasses(vm->context, vm->global);
  JS_SetErrorReporter(vm->context, on_error);
  JS_DefineFunction(vm->context, JS_GetGlobalObject(vm->context), "ejsLog", js_log, 2, 0);
  end_request(vm);
  vm->invoke_count = 0;
  return vm;
}

void sm_stop(jaegermonkey_vm *vm) {
  JS_SetContextThread(vm->context);
  JS_DestroyContext(vm->context);
  JS_DestroyRuntime(vm->runtime);
  driver_free(vm);
}

void sm_shutdown() {
  JS_ShutDown();
}

char *escape_quotes(char *text) {
  int bufsize = strlen(text) * 2;
  char *buf = (char *) driver_alloc(bufsize);
  memset(buf, 0, bufsize);
  int i = 0;
  int x = 0;
  int escaped = 0;
  for (i = 0; i < strlen(text); i++) {
    if (text[i] == '"') {
      if(!escaped) {
	memcpy(&buf[x], (char *) "\\\"", 2);
	x += 2;
      }
      else {
	memcpy(&buf[x], &text[i], 1);
	x++;
      }
    }
    else {
      if(text[i] =='\\') {
	escaped = 1;
      }
      else {
	escaped = 0;
      }
      memcpy(&buf[x], &text[i], 1);
      x++;
    }
  }
  char *retval = (char *) driver_alloc(strlen(buf) + 1);
  memset(retval, 0, strlen(buf) + 1);
  strncpy(retval, buf, strlen(buf));
  driver_free(buf);
  return retval;
}

char *error_to_json(const jaegermonkey_error *error) {
  char *escaped_source = escape_quotes(error->offending_source);
  int size = (strlen(escaped_source) + strlen(error->msg) + 128) * sizeof(char);
  char *retval = (char *) driver_alloc(size);

  snprintf(retval, size, "{\"error\": {\"lineno\": %d, \"message\": \"%s\", \"source\": \"%s\"}}", error->lineno,
	   error->msg, escaped_source);
  driver_free(escaped_source);
  return retval;
}

void free_error(jaegermonkey_error *error) {
  driver_free(error->offending_source);
  driver_free(error->msg);
  driver_free(error);
}

char *sm_eval(jaegermonkey_vm *vm, const char *filename, const char *code, int handle_retval) {
  char *retval = NULL;
  JSScript *script;
  jsval result;

  begin_request(vm);
  script = JS_CompileScript(vm->context,
			    vm->global,
			    code, strlen(code),
			    filename, 1);
  jaegermonkey_error *error = (jaegermonkey_error *) JS_GetContextPrivate(vm->context);
  if (error == NULL) {
    JS_ClearPendingException(vm->context);
    JS_ExecuteScript(vm->context, vm->global, script, &result);
    vm->invoke_count++;
    error = (jaegermonkey_error *) JS_GetContextPrivate(vm->context);
    if (error == NULL) {
      if (handle_retval) {
	if (JSVAL_IS_STRING(result)) {
	  JSString *str = JS_ValueToString(vm->context, result);
	  retval = copy_jsstring(str);
	}
	else if(strcmp(JS_GetStringBytes(JS_ValueToString(vm->context, result)), "undefined") == 0) {
	  retval = copy_string("{\"error\": \"Expression returned undefined\", \"lineno\": 0, \"source\": \"unknown\"}");
	}
	else {
	  retval = copy_string("{\"error\": \"non-JSON return value\", \"lineno\": 0, \"source\": \"unknown\"}");
	}
      }
      JS_DestroyScript(vm->context, script);
    }
    else {
      retval = error_to_json(error);
      free_error(error);
      JS_SetContextPrivate(vm->context, NULL);
    }
  }
  else {
    retval = error_to_json(error);
    free_error(error);
    JS_SetContextPrivate(vm->context, NULL);
  }
  if (vm->invoke_count > 200) {
    JS_GC(vm->context);
    vm->invoke_count = 0;
  }
  else {
    JS_MaybeGC(vm->context);
  }

  end_request(vm);
  return retval;
}
