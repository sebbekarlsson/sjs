#include <js/builtins/object.h>
#include <js/js_eval.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *builtin_object_keys(void *ptr, list_T *args, map_T *stack) {

  JSAST *arg1 = args->items[0];
  JSAST *newarr = init_js_ast_result(JS_AST_ARRAY);
  list_T *keys = js_ast_get_keys_asts(arg1);
  list_concat(newarr->children, keys);
  list_free_shallow(keys);
  return newarr;
}

void *builtin_object_values(void *ptr, list_T *args, map_T *stack) {
  JSAST *arg1 = args->items[0];
  JSAST *newarr = init_js_ast_result(JS_AST_ARRAY);
  list_T *values = js_ast_get_values(arg1);
  list_concat(newarr->children, values);
  list_free_shallow(values);
  return newarr;
}

void js_builtin_object_init_prototype_functions(JSAST *prototype) { return; }

JSAST *js_builtin_object_constructor(JSAST *prototype) {
  js_builtin_object_init_prototype_functions(prototype);
  return prototype;
}

JSAST *init_js_builtin_object_prototype(JSAST *child) {
  if (child == 0)
    child = init_js_ast(JS_AST_ARRAY);
  JSAST *prototype = init_js_ast(JS_AST_OBJECT);
  prototype->prototype_child = child;

  return js_builtin_object_constructor(prototype);
}

// static functions
#include <object.gpp.h>

JSAST *init_js_builtin_object() {
  JSAST *obj = init_js_ast(JS_AST_OBJECT);

  js_builtin_object_init_static_functions(obj);

  JSAST *prototype = init_js_builtin_object_prototype(0);
  map_set(obj->keyvalue, "prototype", prototype);

  return obj;
}
