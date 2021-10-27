#include <js/builtins/array.h>
#include <js/builtins/string.h>
#include <js/js.h>
#include <js/js_eval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

JSAST *init_js_builtin_string_prototype(JSAST *child) {
  if (child == 0)
    child = init_js_ast(JS_AST_STRING);
  JSAST *prototype = init_js_ast(JS_AST_OBJECT);
  prototype->prototype_child = child;

  JSAST *length_ast = init_js_ast(JS_AST_NUMBER);
  length_ast->value_int_size_ptr = &child->string_length;
  map_set(prototype->keyvalue, "length", length_ast);

  return js_builtin_array_constructor(prototype);
}

JSAST *init_js_builtin_string() {
  JSAST *arr = init_js_ast(JS_AST_OBJECT);
  JSAST *prototype = init_js_builtin_string_prototype(0);
  map_set(arr->keyvalue, "prototype", prototype);

  return arr;
}
