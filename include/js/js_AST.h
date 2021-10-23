#ifndef JS_ENGINE_AST_H
#define JS_ENGINE_AST_H
#include <AST_TYPES.gpp.h>
#include <js/js_list.h>
#include <js/js_token.h>
#include <hashmap/map.h>
typedef struct JS_AST_STRUCT {
  JSASTType type;
  struct JS_AST_STRUCT* left;
  struct JS_AST_STRUCT* right;
  float value_num;
  char* value_str;
  list_T* children;
  list_T* args;
  map_T* keyvalue;
  JSTokenType token_type;

} JSAST;

JSAST* init_js_ast(JSASTType type);
#endif
