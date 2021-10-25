#ifndef JS_ENGINE_BUILTINS_OBJECT_H
#define JS_ENGINE_BUILTINS_OBJECT_H
#include <js/js_AST.h>
#include <js/js_list.h>
JSAST* init_js_builtin_object_prototype(JSAST* child);
JSAST *init_js_builtin_object();
#endif
