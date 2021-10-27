#include <js/builtins/process.h>
#include <js/js.h>
#include <js/js_eval.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *builtin_process_cwd(void *ptr, list_T *args, map_T *stack) {
  JSExecution *execution = (JSExecution *)((JSAST *)ptr)->any_ptr;
  JSAST *string_ast = init_js_ast_result(JS_AST_STRING);
  js_ast_set_value_str(string_ast, execution->cwd);
  return string_ast;
}

void js_builtin_process_init_prototype_functions(JSAST *prototype) { return; }

JSAST *js_builtin_process_constructor(JSAST *prototype) {
  js_builtin_process_init_prototype_functions(prototype);
  return prototype;
}

JSAST *init_js_builtin_process_prototype(JSAST *child) {
  // if (child == 0)
  //  child = init_js_ast(JS_AST_ARRAY);
  JSAST *prototype = init_js_ast(JS_AST_OBJECT);
  // prototype->prototype_child = child;

  return js_builtin_process_constructor(prototype);
}

// static functions
#include <process.gpp.h>

JSAST *init_js_builtin_process(JSExecution *execution) {
  JSAST *obj = init_js_ast(JS_AST_OBJECT);
  obj->any_ptr = execution;

  js_builtin_process_init_static_functions(obj);

  JSAST *prototype = init_js_builtin_process_prototype(0);
  map_set(obj->keyvalue, "prototype", prototype);

  return obj;
}
