#ifndef JS_ENGINE_EVAL_H
#define JS_ENGINE_EVAL_H
#include <js/js_AST.h>

JSAST* js_eval(JSAST* ast, map_T* stack);
JSAST* js_eval_compound(JSAST* ast, map_T* stack);
JSAST* js_eval_statement(JSAST* ast, map_T* stack);
JSAST* js_eval_call(JSAST* ast, map_T* stack);
JSAST* js_eval_string(JSAST* ast, map_T* stack);
JSAST* js_eval_number(JSAST* ast, map_T* stack);
JSAST* js_eval_assign(JSAST* ast, map_T* stack);
JSAST* js_eval_function(JSAST* ast, map_T* stack);
JSAST* js_eval_if(JSAST* ast, map_T* stack);
JSAST* js_eval_binop(JSAST* ast, map_T* stack);
JSAST* js_eval_dot(JSAST* ast, map_T* stack);
JSAST* js_eval_unop(JSAST* ast, map_T* stack);
JSAST* js_eval_id(JSAST* ast, map_T* stack);
JSAST* js_eval_definition(JSAST* ast, map_T* stack);
JSAST* js_eval_assignment(JSAST* ast, map_T* stack);
#endif
