#include <js/builtins/array.h>
#include <js/builtins/console/console.h>
#include <js/builtins/error.h>
#include <js/builtins/functions/functions.h>
#include <js/builtins/math.h>
#include <js/builtins/object.h>
#include <js/builtins/process.h>
#include <js/builtins/string.h>
#include <js/js.h>
#include <js/js_frame.h>
#include <stdint.h>
#include <stdio.h>

const char *NAMES[8] = {"console", "Array",     "String",     "Object",
                        "process", "__dirname", "__filename", "module"};

const int NAMES_LENGTH = 8;

void js_frame_free(map_T *frame, JSExecution *execution) {
  if (frame == 0)
    return;

  char **keys = 0;
  unsigned int length = 0;

  map_get_keys(frame, &keys, &length);

  for (size_t i = 0; i < length; i++) {
    char *k = keys[i];
    if (k == 0)
      continue;
    JSAST *child = (JSAST *)map_get_value(frame, k);
    if (child == 0)
      continue;

    js_gc_ast(execution->gc, child);
  }

  map_free(frame);
}

#define GC_MARK(item)                                                          \
  {                                                                            \
    if (execution->gc == 0) {                                                  \
      printf("ERROR (%s): Garbage collector is NULL.\n", __func__);            \
      exit(1);                                                                 \
    }                                                                          \
    js_gc_ast(execution->gc, item);                                            \
  }

map_T *setup_js_frame(JSExecution *execution) {
  map_T *frame = NEW_MAP();

  JSAST *process = init_js_builtin_process(execution);
  GC_MARK(process);
  map_set(frame, "process", process);

  JSAST *dirnameast = init_js_ast(JS_AST_STRING);
  GC_MARK(dirnameast);
  dirnameast->value_str_ptr = &execution->__dirname;

  JSAST *filenameast = init_js_ast(JS_AST_STRING);
  GC_MARK(filenameast);
  filenameast->value_str_ptr = &execution->__filename;

  map_set(frame, "__dirname", dirnameast);
  map_set(frame, "__filename", filenameast);

  JSAST *module = init_js_ast(JS_AST_OBJECT);
  GC_MARK(module);
  JSAST *exports = init_js_ast(JS_AST_OBJECT);
  GC_MARK(exports);
  map_set(module->keyvalue, "exports", exports);
  map_set(frame, "module", module);

  JSAST *console = init_js_builtin_console(execution);
  GC_MARK(console);
  map_set(frame, "console", console);

  JSAST *arr = init_js_builtin_array();
  GC_MARK(arr);
  map_set(frame, "Array", arr);

  JSAST *str = init_js_builtin_string();
  GC_MARK(str);
  map_set(frame, "String", str);

  JSAST *obj = init_js_builtin_object();
  GC_MARK(obj);
  map_set(frame, "Object", obj);

  JSAST *math = init_js_builtin_math(execution);
  GC_MARK(math);
  map_set(frame, "Math", math);

  JSAST *error = init_js_builtin_error();
  GC_MARK(error);
  map_set(frame, "Error", error);

  js_builtin_init_functions(frame);

  // TODO: implement Boolean
  // TODO: implement Boolean
  // TODO: implement Function
  // TODO: implement Symbol
  // TODO: implement Number
  // TODO: implement BigInt
  // TODO: implement Date
  // TODO: implement RegExp
  // TODO: implement JSON
  // TODO: implement Infinity
  // TODO: implement NaN
  // TODO: implement undefined
  // TODO: implement globalThis

  // TODO: implement Int8Array
  // TODO: implement Uint8Array
  // TODO: implement Uint8ClampedArray
  // TODO: implement Int16Array
  // TODO: implement Uint16Array
  // TODO: implement Int32Array
  // TODO: implement Uint32Array
  // TODO: implement Float32Array
  // TODO: implement Float64Array
  // TODO: implement BigInt64Array
  // TODO: implement BigUint64Array

  // TODO: implement Map
  // TODO: implement Set
  // TODO: implement WeakMap
  // TODO: implement WeakSet

  // TODO: implement ArrayBuffer
  // TODO: implement SharedArrayBuffer
  // TODO: implement Atomics
  // TODO: implement DataView

  // TODO: implement Promise
  // TODO: implement Generator
  // TODO: implement GeneratorFunction
  // TODO: implement AsyncFunction
  // TODO: implement AsyncGenerator
  // TODO: implement AsyncGeneratorFunction

  // TODO: implement Reflect
  // TODO: implement Proxy

  // TODO: implement Intl
  // TODO: implement Intl.Collator
  // TODO: implement Intl.DateTimeFormat
  // TODO: implement Intl.ListFormat
  // TODO: implement Intl.NumberFormat
  // TODO: implement Intl.PluralRules
  // TODO: implement Intl.RelativeTimeFormat
  // TODO: implement Intl.Locale

  // TODO: implement Error
  // TODO: implement AggregateError
  // TODO: implement  EvalError
  // TODO: implement InternalError
  // TODO: implement RangeError
  // TODO: implement ReferenceError
  // TODO: implement SyntaxError
  // TODO: implement TypeError
  // TODO: implement URIError

  return frame;
}
