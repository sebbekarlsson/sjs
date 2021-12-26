#ifndef JS_ENGINE_EVAL_H
#define JS_ENGINE_EVAL_H
#include <js/js_AST.h>

struct JS_EXECUTION_STRUCT;

#define STACK_ADDR_RETURN "___RETURN___"
#define STACK_ADDR_CONSTRUCT_FLAG "___NEW___"
#define STACK_ADDR_CONSTRUCT_CALLER "___NEW_CALLER___"
#define STACK_ADDR_THIS "this"
#define STACK_ADDR_EXCEPTION "___EXCEPTION___"

JSAST *js_eval(JSAST *ast, map_T *stack, struct JS_EXECUTION_STRUCT *execution);
JSAST *js_maybe_eval(JSAST *ast, map_T *stack,
                     struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_compound(JSAST *ast, map_T *stack,
                        struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_statement(JSAST *ast, map_T *stack,
                         struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_call(JSAST *ast, map_T *stack,
                    struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_string(JSAST *ast, map_T *stack,
                      struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_number(JSAST *ast, map_T *stack,
                      struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_assign(JSAST *ast, map_T *stack,
                      struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_function(JSAST *ast, map_T *stack,
                        struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_class(JSAST *ast, map_T *stack,
                     struct JS_EXECUTION_STRUCT *execution);

JSAST *js_eval_import(JSAST *ast, map_T *stack,
                      struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_if(JSAST *ast, map_T *stack,
                  struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_while(JSAST *ast, map_T *stack,
                     struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_for(JSAST *ast, map_T *stack,
                   struct JS_EXECUTION_STRUCT *execution);

JSAST *js_eval_construct(JSAST *ast, map_T *stack,
                         struct JS_EXECUTION_STRUCT *execution);

JSAST *js_eval_try(JSAST *ast, map_T *stack,
                   struct JS_EXECUTION_STRUCT *execution);

JSAST *js_eval_catch(JSAST *ast, map_T *stack,
                     struct JS_EXECUTION_STRUCT *execution);

JSAST *js_eval_throw(JSAST *ast, map_T *stack,
                     struct JS_EXECUTION_STRUCT *execution);

JSAST *js_eval_property_access(JSAST *ast, map_T *stack,
                               struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_binop(JSAST *ast, map_T *stack,
                     struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_dot(JSAST *ast, map_T *stack,
                   struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_unop(JSAST *ast, map_T *stack,
                    struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_id(JSAST *ast, map_T *stack,
                  struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_definition(JSAST *ast, map_T *stack,
                          struct JS_EXECUTION_STRUCT *execution);
JSAST *js_eval_assignment(JSAST *ast, map_T *stack,
                          struct JS_EXECUTION_STRUCT *execution);

JSAST *js_call_function(JSAST *self, JSAST *ast, JSFunction *fptr,
                        list_T *call_args, list_T *expected_args, map_T *stack,
                        struct JS_EXECUTION_STRUCT *execution);

void stack_pop(map_T *stack, const char *key);

void js_export_symbol(char *name, JSAST *symbol, map_T *stack,
                      struct JS_EXECUTION_STRUCT *execution);

unsigned int js_is_dry(map_T *stack, struct JS_EXECUTION_STRUCT *execution);

unsigned int js_is_true(JSAST *ast);

unsigned int js_is_empty(JSAST *ast);
#endif
