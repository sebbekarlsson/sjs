#ifndef JS_ENGINE_BUILTINS_PROCESS_H
#define JS_ENGINE_BUILTINS_PROCESS_H
#include <js/js_AST.h>
#include <js/js.h>
#include <js/js_list.h>
JSAST* init_js_builtin_process_prototype(JSAST* child);
JSAST *init_js_builtin_process(JSExecution* execution);
#endif
