#ifndef JS_ENGINE_BUILTINS_ARRAY_H
#define JS_ENGINE_BUILTINS_ARRAY_H
#include <js/js_AST.h>
#include <js/js_list.h>
#include <js/js.h>
JSAST* init_js_builtin_array_prototype(JSAST* child);
JSAST* js_builtin_array_constructor(JSAST* prototype);
void *builtin_array_map(void* ptr, list_T *args, map_T *stack, JSExecution* execution);
void *builtin_array_from(void* ptr, list_T *args, map_T *stack, JSExecution* execution);
JSAST *init_js_builtin_array();
#endif
