#ifndef JS_ENGINE_AST_H
#define JS_ENGINE_AST_H
#include <AST_TYPES.gpp.h>
#include <hashmap/map.h>
#include <js/js_list.h>
#include <js/js_token.h>
#include <stdint.h>

typedef void *(*JSFunction(list_T *args, map_T *stack));

typedef struct JS_AST_STRUCT {
  JSASTType type;
  struct JS_AST_STRUCT *left;
  struct JS_AST_STRUCT *right;
  struct JS_AST_STRUCT *body;
  struct JS_AST_STRUCT *expr;
  struct JS_AST_STRUCT *prototype;
  struct JS_AST_STRUCT *prototype_child;
  float value_num;
  int value_int;
  uint32_t *value_int_ptr;
  size_t *value_int_size_ptr;
  char *value_str;
  list_T *children;
  list_T *args;
  map_T *keyvalue;
  JSTokenType token_type;
  JSFunction *fptr;
  unsigned int is_result;
  unsigned int is_true;
} JSAST;

typedef struct JS_AST_TUPLE_STRUCT {
  JSAST *x;
  JSAST *y;
} JSASTTuple;

JSAST *init_js_ast(JSASTType type);

JSAST *init_js_ast_result(JSASTType type);

void js_ast_set_value_str(JSAST *ast, char *value_str);

void js_ast_free(JSAST *ast);

void js_ast_maybe_free(JSAST *ast);

char *js_ast_str_value(JSAST *ast);

char *js_ast_to_string(JSAST *ast);
#endif
