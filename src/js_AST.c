#include <js/js_AST.h>
#include <js/macros.h>
#include <string.h>
#include <stdio.h>

JSAST* init_js_ast(JSASTType type) {

  JSAST* ast = NEW(JSAST);
  ast->type = type;
  ast->children = init_list(sizeof(JSAST*));
  ast->args = init_list(sizeof(JSAST*));
  ast->keyvalue = NEW_MAP();
  ast->value_str = strdup("");

  return ast;
}

void js_ast_set_value_str(JSAST* ast, char* value_str) {
  if (ast->value_str != 0) free(ast->value_str);
  ast->value_str = strdup(value_str);
}


char* js_ast_str_value(JSAST* ast) {
  if (ast == 0) return strdup("");

  if (ast->type != JS_AST_NUMBER) {
    if (ast->value_str != 0 && strlen(ast->value_str) > 0) return strdup(ast->value_str);
  }

  char buff[128];
  sprintf(buff, "%1.2f", ast->value_num);
  return strdup(buff);
}
