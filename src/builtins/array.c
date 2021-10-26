#include <js/builtins/array.h>
#include <js/js_eval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *builtin_array_map(void *ptr, list_T *args, map_T *stack) {
  JSAST *arr = ptr;

  JSAST *arg1 = args->items[0];

  list_T *children = js_ast_to_array(ptr);
  JSAST *new_arr = init_js_ast(JS_AST_ARRAY);

  if (children == 0)
    return new_arr;

  list_T *call_args = init_list(sizeof(JSAST *));

  for (uint32_t i = 0; i < children->size; i++) {
    list_clear(call_args);
    JSAST *child = (JSAST *)children->items[i];
    JSAST *evaluated = js_eval(child, stack);
    list_push(call_args, evaluated);

    JSAST *result =
        js_call_function(arg1, arg1, 0, call_args, arg1->args, stack);

    if (result) {
      list_push(new_arr->children, result);
    }
  }

  stack_pop(stack, STACK_ADDR_RETURN);

  return new_arr;
}

void *builtin_array_foreach(void *ptr, list_T *args, map_T *stack) {
  JSAST *arr = ptr;

  JSAST *arg1 = args->items[0];

  list_T *children = js_ast_to_array(ptr);

  if (children == 0)
    return init_js_ast_result(JS_AST_UNDEFINED);

  list_T *call_args = init_list(sizeof(JSAST *));

  for (uint32_t i = 0; i < children->size; i++) {
    list_clear(call_args);
    JSAST *child = (JSAST *)children->items[i];
    JSAST *evaluated = js_eval(child, stack);
    list_push(call_args, evaluated);

    JSAST *result =
        js_call_function(arg1, arg1, 0, call_args, arg1->args, stack);
  }

  return init_js_ast_result(JS_AST_UNDEFINED);
}

// functions
#include <array.gpp.h>

void *builtin_array_from(void *ptr, list_T *args, map_T *stack) {
  JSAST *arg1 = args->items[0];

  JSAST *newarr = init_js_ast_blank(JS_AST_ARRAY);
  newarr->children = js_ast_to_array(arg1);
  return newarr;
}

JSAST *js_builtin_array_constructor(JSAST *prototype) {
  js_builtin_array_init_prototype_functions(prototype);
  return prototype;
}

JSAST *init_js_builtin_array_prototype(JSAST *child) {
  if (child == 0)
    child = init_js_ast_result(JS_AST_ARRAY);
  JSAST *prototype = init_js_ast(JS_AST_OBJECT);
  prototype->prototype_child = child;

  if (child->children == 0) {
    printf("array_prototype: child->children == 0\n");
    exit(1);
  }

  JSAST *length_ast = init_js_ast(JS_AST_NUMBER);
  length_ast->value_int_size_ptr = &child->children->size;
  map_set(prototype->keyvalue, "length", length_ast);

  return js_builtin_array_constructor(prototype);
}

JSAST *init_js_builtin_array() {
  JSAST *arr = init_js_ast(JS_AST_OBJECT);

  js_builtin_array_init_static_functions(arr);

  JSAST *prototype = init_js_builtin_array_prototype(0);
  map_set(arr->keyvalue, "prototype", prototype);

  return arr;
}
