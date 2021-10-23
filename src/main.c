#include <js/js_eval.h>
#include <js/js_io.h>
#include <js/js_lexer.h>
#include <js/js_parser.h>
#include <stdint.h>
#include <stdio.h>

void *_builtin_print(list_T *args, map_T *stack) {
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

int main(int argc, char *argv[]) {
  char *contents = get_file_contents(argv[1]);

  JSLexer *lexer = init_js_lexer(contents);
  JSParser *parser = init_js_parser(lexer);

  JSAST *root = js_parser_parse(parser);
  map_T *global_frame = NEW_MAP();

  JSAST *func = init_js_ast(JS_AST_FUNCTION);
  JSAST *id = init_js_ast(JS_AST_ID);
  js_ast_set_value_str(id, "value");
  list_push(func->args, id);
  func->fptr = _builtin_print;
  js_ast_set_value_str(func, "log");

  JSAST *console = init_js_ast(JS_AST_OBJECT);
  js_ast_set_value_str(console, "console");
  map_set(console->keyvalue, "log", func);

  map_set(global_frame, "console", console);

  JSAST *result = js_eval(root, global_frame);

  return 0;
}
