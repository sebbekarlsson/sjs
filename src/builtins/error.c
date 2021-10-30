#include <js/builtins/error.h>
#include <js/js_eval.h>

JSAST *js_builtin_error_public_constructor(void *ptr, list_T *args,
                                           map_T *stack,
                                           JSExecution *execution) {
  JSAST *arg1 = args->size ? (JSAST *)args->items[0] : 0;
  if (arg1 == 0)
    return js_builtin_error_constructor(ptr, init_js_ast_result(JS_AST_STRING));

  JSAST *evaluated_arg = js_eval(arg1, stack, execution);

  JSAST *prot = js_builtin_error_constructor(ptr, evaluated_arg);
  return ptr;
}

// functions
#include <error.gpp.h>

JSAST *js_builtin_error_constructor(JSAST *self, JSAST *message_ast) {
  JSAST *_this = self;
  JSAST *prototype = init_js_ast(JS_AST_OBJECT);
  js_builtin_error_init_prototype_functions(prototype);

  if (self != 0) {
    prototype->prototype_child = self;
    map_set(self->keyvalue, "message", message_ast);
    map_set(self->keyvalue, "prototype", prototype);
    return self;
  } else if (self == 0) {
    JSAST *func = init_js_ast(JS_AST_FUNCTION);
    map_set(func->keyvalue, "prototype", prototype);
    js_ast_set_value_str(func, "Error");
    func->prototype = prototype;
    return func;
  }

  return prototype;
}

JSAST *init_js_builtin_error() { return js_builtin_error_constructor(0, 0); }
