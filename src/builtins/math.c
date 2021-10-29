#include <js/builtins/math.h>
#include <js/js.h>
#include <js/js_eval.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *builtin_math_cos(void *ptr, list_T *args, map_T *stack,
                       JSExecution *execution) {
  JSAST *arr = ptr;
  JSAST *arg1 = args->items[0];
  JSAST *result = init_js_ast_result(JS_AST_NUMBER);

  result->value_num = cos((float)(arg1->value_num));

  return result;
}

// functions
#include <math.gpp.h>

JSAST *js_builtin_math_constructor(JSAST *prototype) {
  //  js_builtin_math_init_prototype_functions(prototype);
  return prototype;
}

JSAST *init_js_builtin_math() {
  JSAST *arr = init_js_ast(JS_AST_OBJECT);

  js_builtin_math_init_static_functions(arr);

  return arr;
}
