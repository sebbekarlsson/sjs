#include <js/builtins/console/console.h>
#include <js/builtins/utils.h>
#include <stdint.h>

static void *builtin_console_log(list_T *args, map_T *stack) {
  for (uint32_t i = 0; i < args->size; i++) {
    JSAST *arg = (JSAST *)args->items[i];
    char *str = js_ast_str_value(arg);

    if (str != 0) {
      printf("%s\n", str);
      free(str);
    }
  }

  return init_js_ast(JS_AST_UNDEFINED);
}

JSAST *init_js_builtin_console() {
  JSAST *console = init_js_ast(JS_AST_OBJECT);
  js_ast_set_value_str(console, "console");

  JSAST *func = init_js_ast(JS_AST_FUNCTION);

  JSAST *id = init_js_ast(JS_AST_ID);
  js_ast_set_value_str(id, "value");
  list_push(func->args, id);
  func->fptr = builtin_console_log;
  js_ast_set_value_str(func, "log");

  map_set(console->keyvalue, "log", func);

  return console;
}
