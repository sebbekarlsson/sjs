#include <js/builtins/utils.h>

JSAST *js_create_function_from_ptr(JSFunction ptr, const char *name, int argc,
                                   char *argv[]) {
  JSAST *func = init_js_ast(JS_AST_FUNCTION);

  for (int i = 0; i < argc; i++) {
    JSAST *id = init_js_ast(JS_AST_ID);
    js_ast_set_value_str(id, argv[i]);
    list_push(func->args, id);
  }
  func->fptr = ptr;
  js_ast_set_value_str(func, name);
  return func;
}
