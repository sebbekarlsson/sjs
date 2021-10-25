#ifndef JS_ENGINE_EVAL_H
#define JS_ENGINE_EVAL_H
#include <js/js_AST.h>

#define STACK_ADDR_RETURN "RETURN"

JSAST *js_eval(JSAST *ast, map_T *stack);
JSAST *js_maybe_eval(JSAST *ast, map_T *stack);
JSAST *js_eval_compound(JSAST *ast, map_T *stack);
JSAST *js_eval_statement(JSAST *ast, map_T *stack);
JSAST *js_eval_call(JSAST *ast, map_T *stack);
JSAST *js_eval_string(JSAST *ast, map_T *stack);
JSAST *js_eval_number(JSAST *ast, map_T *stack);
JSAST *js_eval_assign(JSAST *ast, map_T *stack);
JSAST *js_eval_function(JSAST *ast, map_T *stack);
JSAST *js_eval_if(JSAST *ast, map_T *stack);
JSAST *js_eval_while(JSAST *ast, map_T *stack);
JSAST *js_eval_for(JSAST *ast, map_T *stack);
JSAST *js_eval_property_access(JSAST *ast, map_T *stack);
JSAST *js_eval_binop(JSAST *ast, map_T *stack);
JSAST *js_eval_dot(JSAST *ast, map_T *stack);
JSAST *js_eval_unop(JSAST *ast, map_T *stack);
JSAST *js_eval_id(JSAST *ast, map_T *stack);
JSAST *js_eval_definition(JSAST *ast, map_T *stack);
JSAST *js_eval_assignment(JSAST *ast, map_T *stack);

JSAST *js_call_function(JSAST *self, JSAST *ast, JSFunction *fptr,
                        list_T *call_args, list_T *expected_args, map_T *stack);

void stack_pop(map_T *stack, const char *key);
#endif
