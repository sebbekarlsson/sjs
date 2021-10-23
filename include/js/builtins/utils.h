#ifndef JS_ENGINE_BUILTINS_CONSOLE_H
#define JS_ENGINE_BUILTINS_CONSOLE_H
#include <js/js_AST.h>
#include <stdint.h>
JSAST* js_create_function_from_ptr(JSFunction ptr, const char* name, int argc, char* argv[]);
#endif
