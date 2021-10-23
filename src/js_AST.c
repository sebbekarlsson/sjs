#include <js/js_AST.h>
#include <js/macros.h>

JSAST* init_js_ast(JSASTType type) {

  JSAST* ast = NEW(JSAST);
  ast->type = type;
  ast->children = init_list(sizeof(JSAST*));
  ast->args = init_list(sizeof(JSAST*));
  ast->keyvalue = NEW_MAP();

  return ast;
}
