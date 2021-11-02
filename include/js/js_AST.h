#ifndef JS_ENGINE_AST_H
#define JS_ENGINE_AST_H
#include <AST_TYPES.gpp.h>
#include <hashmap/map.h>
#include <js/js_iterator.h>
#include <js/js_list.h>
#include <js/js_token.h>
#include <stdint.h>

struct JS_EXECUTION_STRUCT;

typedef void *(*JSFunction(void *ptr, list_T *args, map_T *stack,
                           struct JS_EXECUTION_STRUCT *execution));

typedef struct JS_AST_STRUCT {
  JSASTType type;
  struct JS_AST_STRUCT *left;
  struct JS_AST_STRUCT *right;
  struct JS_AST_STRUCT *body;
  struct JS_AST_STRUCT *expr;
  struct JS_AST_STRUCT *prototype;
  struct JS_AST_STRUCT *prototype_child;
  struct JS_AST_STRUCT *accessed;
  struct JS_AST_STRUCT *constructor;
  float value_num;
  int value_int;
  uint32_t *value_int_ptr;
  size_t *value_int_size_ptr;
  size_t string_length;
  char **value_str_ptr;
  void *any_ptr;
  char *value_str;
  list_T *children;
  list_T *args;
  map_T *keyvalue;
  JSTokenType token_type;
  JSFunction *fptr;
  unsigned int is_result;
  unsigned int is_true;
  unsigned int exported;
  unsigned int marked;
  int stack_index;
} JSAST;

typedef struct JS_AST_TUPLE_STRUCT {
  JSAST *x;
  JSAST *y;
} JSASTTuple;

JSAST *init_js_ast(JSASTType type);

JSAST *js_ast_copy(JSAST *src);

JSAST *init_js_ast_blank(JSASTType type);

JSAST *init_js_ast_result(JSASTType type);

list_T *js_ast_to_array(JSAST *ast);

void js_ast_set_value_str(JSAST *ast, char *value_str);

void js_ast_free(JSAST *ast);

void js_ast_maybe_free(JSAST *ast);

char *js_ast_str_value(JSAST *ast);

char *js_ast_to_string(JSAST *ast);

JSAST *js_ast_most_right(JSAST *ast);

JSAST *js_ast_get_constructor(JSAST *ast);

JSAST *js_ast_get_prototype(JSAST *ast);

void js_ast_set_prototype(JSAST *ast, JSAST *prototype);

list_T *js_ast_get_values(JSAST *ast);
list_T *js_ast_get_keys(JSAST *ast);
list_T *js_ast_get_keys_asts(JSAST *ast);

JSIterator js_ast_iterate(JSAST *ast);

int js_ast_get_int(JSAST *ast);

void js_ast_set_int(JSAST *ast, int value);
void js_ast_set_float(JSAST *ast, float value);

#endif
