#include <js/builtins/array.h>
#include <js/builtins/console/console.h>
#include <js/js_frame.h>
#include <stdint.h>
#include <stdio.h>

map_T *setup_js_frame() {
  map_T *frame = NEW_MAP();

  JSAST *console = init_js_builtin_console();
  map_set(frame, "console", console);

  JSAST *arr = init_js_builtin_array();
  map_set(frame, "Array", arr);

  // TODO: implement String
  // TODO: implement Object
  // TODO: implement Boolean
  // TODO: implement Math
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
