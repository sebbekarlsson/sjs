#include <js/builtins/array.h>
#include <js/js_eval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *builtin_array_map(void *ptr, list_T *args, map_T *stack) {
  if (ptr == 0)
    printf("ptr is 0\n");
  JSAST *arr = ptr;

  JSAST *arg1 = args->items[0];

  JSAST *call = init_js_ast(JS_AST_CALL);
  call->left = arg1;
  JSAST *new_arr = init_js_ast_result(JS_AST_ARRAY);

  for (uint32_t i = 0; i < arr->children->size; i++) {
    list_clear(call->args);
    JSAST *child = (JSAST *)arr->children->items[i];
    JSAST *evaluated = js_eval(child, stack);
    list_push(call->args, evaluated);

    JSAST *result = js_eval_call(call, stack);
    // JSAST* result = map_get_value(stack, STACK_ADDR_RETURN);

    if (result) {
      list_push(new_arr->children, result);
    }
  }

  stack_pop(stack, STACK_ADDR_RETURN);
  // map_unset(stack, STACK_ADDR_RETURN);
  // map_set(stack, STACK_ADDR_RETURN, new_arr);

  return new_arr;
}

JSAST *init_js_builtin_array_prototype(JSAST *child) {
  if (child == 0)
    child = init_js_ast(JS_AST_ARRAY);
  JSAST *prototype = init_js_ast(JS_AST_OBJECT);
  prototype->prototype_child = child;

  JSAST *length_ast = init_js_ast(JS_AST_NUMBER);
  length_ast->value_int_size_ptr = &child->children->size;
  map_set(prototype->keyvalue, "length", length_ast);

  JSAST *map_func = init_js_ast(JS_AST_FUNCTION);
  JSAST *id = init_js_ast(JS_AST_ID);
  js_ast_set_value_str(id, "map_func");
  list_push(map_func->args, id);
  map_func->fptr = builtin_array_map;
  js_ast_set_value_str(map_func, "map");
  map_set(prototype->keyvalue, "map", map_func);

  return prototype;
}

JSAST *init_js_builtin_array() {
  JSAST *arr = init_js_ast(JS_AST_OBJECT);
  JSAST *prototype = init_js_builtin_array_prototype(0);
  map_set(arr->keyvalue, "prototype", prototype);

  return arr;
}
