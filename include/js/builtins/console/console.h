#ifndef JS_ENGINE_BUILTINS_CONSOLE_H
#define JS_ENGINE_BUILTINS_CONSOLE_H
#include <js/js.h>
#include <js/js_AST.h>
JSAST *init_js_builtin_console(JSExecution *execution);
void *builtin_console_log(void *ptr, list_T *args, map_T *stack,
                          JSExecution *execution);
#endif
