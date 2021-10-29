#ifndef JS_ENGINE_BUILTINS_MATH_H
#define JS_ENGINE_BUILTINS_MATH_H
#include <js/js_AST.h>
#include <js/js_list.h>
#include <js/js.h>
JSAST* js_builtin_math_constructor(JSAST* prototype);
void *builtin_math_cos(void* ptr, list_T *args, map_T *stack, JSExecution* execution);
JSAST *init_js_builtin_math();
#endif
