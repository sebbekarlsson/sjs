#ifndef JS_ENGINE_BUILTINS_ERROR_H
#define JS_ENGINE_BUILTINS_ERROR_H
#include <js/js_AST.h>
#include <js/js_list.h>
#include <js/js.h>
JSAST* js_builtin_error_constructor(JSAST* self, JSAST* message_ast);
JSAST *init_js_builtin_error();
#endif
