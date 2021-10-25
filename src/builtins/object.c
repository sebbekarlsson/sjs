#include <js/builtins/object.h>
#include <js/js_eval.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *builtin_object_keys(void *ptr, list_T *args, map_T *stack) {
  JSAST *arg1 = args->items[0];
  JSAST *new_arr = init_js_ast_result(JS_AST_ARRAY);

  unsigned int length = 0;
  char **keys = 0;
  map_get_keys(arg1->keyvalue, &keys, &length);

  for (size_t i = 0; i < length; i++) {
    char *key = keys[i];
    if (key == 0)
      continue;

    JSAST *ast_str = init_js_ast_result(JS_AST_STRING);
    js_ast_set_value_str(ast_str, key);
    list_push(new_arr->children, ast_str);
  }

  return new_arr;
}

void *builtin_object_values(void *ptr, list_T *args, map_T *stack) {
  JSAST *arg1 = args->items[0];
  JSAST *new_arr = init_js_ast_result(JS_AST_ARRAY);

  unsigned int length = 0;
  char **keys = 0;
  map_get_keys(arg1->keyvalue, &keys, &length);

  for (size_t i = 0; i < length; i++) {
    char *key = keys[i];
    if (key == 0)
      continue;

    JSAST *ast_v = (JSAST *)map_get_value(arg1->keyvalue, key);
    if (ast_v == 0)
      continue;

    list_push(new_arr->children, ast_v);
  }

  return new_arr;
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
