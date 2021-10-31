#ifndef JS_ENGINE_BUILTINS_FUNCTION_PARSE_INT_H
#define JS_ENGINE_BUILTINS_FUNCTION_PARSE_INT_H
#include <js/js.h>
#include <js/js_list.h>
void *builtin_function_parse_int(void *ptr, list_T *args, map_T *stack,
                                 JSExecution *execution);
#endif
