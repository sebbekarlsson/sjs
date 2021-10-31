#include <js/builtins/functions/parse_int.h>
#include <js/builtins/macros.h>
#include <js/macros.h>

void *builtin_function_parse_int(void *ptr, list_T *args, map_T *stack,
                                 JSExecution *execution) {
  EXPECT_MIN_ARGS(1);
  JSAST *value = GET_ARG(0);
  JSAST *radix = GET_ARG(1);

  char *str = value->value_str;
  int radix_i = OR(js_ast_get_int(radix), 10);
  int result_i = str ? (int)strtol(str, NULL, radix_i) : 0;

  JSAST *result_ast = init_js_ast_result(JS_AST_NUMBER);
  js_ast_set_int(result_ast, result_i);

  return result_ast;
}
