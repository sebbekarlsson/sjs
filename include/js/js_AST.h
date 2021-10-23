#ifndef JS_ENGINE_AST_H
#define JS_ENGINE_AST_H
#include <AST_TYPES.gpp.h>
#include <js/js_list.h>
#include <js/js_token.h>
#include <hashmap/map.h>


typedef void* (*JSFunction(list_T* args, map_T* stack));

typedef struct JS_AST_STRUCT {
  JSASTType type;
  struct JS_AST_STRUCT* left;
  struct JS_AST_STRUCT* right;
  struct JS_AST_STRUCT* body;
  float value_num;
  char* value_str;
  list_T* children;
  list_T* args;
  map_T* keyvalue;
  JSTokenType token_type;
  JSFunction* fptr;

} JSAST;

JSAST* init_js_ast(JSASTType type);

void js_ast_set_value_str(JSAST* ast, char* value_str);

char* js_ast_str_value(JSAST* ast);
#endif
