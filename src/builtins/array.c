#include <js/builtins/array.h>
#include <js/js_eval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *builtin_array_map(void *ptr, list_T *args, map_T *stack) {
  JSAST *arr = ptr;

  JSAST *arg1 = args->items[0];

  list_T *call_args = init_list(sizeof(JSAST *));
  JSAST *new_arr = init_js_ast(JS_AST_ARRAY);

  for (uint32_t i = 0; i < arr->children->size; i++) {
    list_clear(call_args);
    JSAST *child = (JSAST *)arr->children->items[i];
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

JSAST *js_builtin_array_constructor(JSAST *prototype) {
  JSAST *map_func = init_js_ast(JS_AST_FUNCTION);
  JSAST *id = init_js_ast(JS_AST_ID);
  js_ast_set_value_str(id, "map_func");
  list_push(map_func->args, id);
  map_func->fptr = builtin_array_map;
  js_ast_set_value_str(map_func, "map");
  map_set(prototype->keyvalue, "map", map_func);
  return prototype;
}

JSAST *init_js_builtin_array_prototype(JSAST *child) {
  if (child == 0)
    child = init_js_ast(JS_AST_ARRAY);
  JSAST *prototype = init_js_ast(JS_AST_OBJECT);
  prototype->prototype_child = child;

  JSAST *length_ast = init_js_ast(JS_AST_NUMBER);
  length_ast->value_int_size_ptr = &child->children->size;
  map_set(prototype->keyvalue, "length", length_ast);

  return js_builtin_array_constructor(prototype);
}

JSAST *init_js_builtin_array() {
  JSAST *arr = init_js_ast(JS_AST_OBJECT);
  JSAST *prototype = init_js_builtin_array_prototype(0);
  map_set(arr->keyvalue, "prototype", prototype);

  return arr;
}
