#include <js/js_AST.h>
#include <js/macros.h>
#include <stdio.h>
#include <string.h>

JSAST *init_js_ast(JSASTType type) {

  JSAST *ast = NEW(JSAST);
  ast->type = type;
  ast->children = init_list(sizeof(JSAST *));
  ast->args = init_list(sizeof(JSAST *));
  ast->keyvalue = NEW_MAP();
  ast->is_result = 0;

  return ast;
}

JSAST *init_js_ast_result(JSASTType type) {
  JSAST *ast = init_js_ast(type);
  ast->is_result = 1;
  return ast;
}

void js_ast_set_value_str(JSAST *ast, char *value_str) {
  if (ast->value_str != 0)
    free(ast->value_str);
  ast->value_str = strdup(value_str);
}

char *js_ast_str_value(JSAST *ast) {
  if (ast == 0)
    return strdup("");

  if (ast->type != JS_AST_NUMBER) {
    if (ast->value_str != 0 && strlen(ast->value_str) > 0)
      return strdup(ast->value_str);
  }

  char buff[128];
  sprintf(buff, "%1.2f", ast->value_num);
  return strdup(buff);
}

static void _free_ast_list(list_T *list) {
  if (list == 0)
    return;
  if (list->items != 0) {
    for (uint32_t i = 0; i < list->size; i++) {
      JSAST *ast = (JSAST *)list->items[i];
      js_ast_free(ast);
    }
    free(list->items);
  }

  free(list);
}

void js_ast_free(JSAST *ast) {
  if (ast == 0)
    return;
  if (ast->value_str != 0) {
    free(ast->value_str);
    ast->value_str = 0;
  }

  if (ast->body != 0) {
    js_ast_free(ast->body);
    ast->body = 0;
  }
  if (ast->left != 0) {
    js_ast_free(ast->left);
    ast->left = 0;
  }
  if (ast->right != 0) {
    js_ast_free(ast->right);
    ast->right = 0;
  }

  map_free(ast->keyvalue);

  _free_ast_list(ast->args);
  _free_ast_list(ast->children);
}

void js_ast_maybe_free(JSAST *ast) {
  if (ast == 0)
    return;
  if (ast->is_result == 0)
    return;
  js_ast_free(ast);
}
