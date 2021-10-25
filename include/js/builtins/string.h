#ifndef JS_ENGINE_BUILTINS_STRING_H
#define JS_ENGINE_BUILTINS_STRING_H
#include <js/js_AST.h>
#include <js/js_list.h>
JSAST* init_js_builtin_string_prototype(JSAST* child);
JSAST *init_js_builtin_string();
#endif
