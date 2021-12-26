#ifndef JS_ENGINE_FRONTEND_JS_H
#define JS_ENGINE_FRONTEND_JS_H
#include <js/js_AST.h>
char* js_f_js_entry(const char* filename);

struct JS_EXECUTION_STRUCT;

char* js_f_js(JSAST* ast, map_T* stack, list_T* stacklist, struct JS_EXECUTION_STRUCT* execution);
#endif
