#include <js/builtins/array.h>

JSAST *init_js_builtin_array_prototype(JSAST *child) {
  if (child == 0)
    child = init_js_ast(JS_AST_ARRAY);
  JSAST *prototype = init_js_ast(JS_AST_OBJECT);
  prototype->prototype_child = child;

  JSAST *length_ast = init_js_ast(JS_AST_NUMBER);
  length_ast->value_int_size_ptr = &child->children->size;
  map_set(prototype->keyvalue, "length", length_ast);

  return prototype;
}

JSAST *init_js_builtin_array() {
  JSAST *arr = init_js_ast(JS_AST_OBJECT);
  JSAST *prototype = init_js_builtin_array_prototype(0);
  map_set(arr->keyvalue, "prototype", prototype);
  return arr;
}
