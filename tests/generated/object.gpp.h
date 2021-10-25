void js_builtin_object_init_static_functions(JSAST *obj) {
  
{

JSAST *func = init_js_ast(JS_AST_FUNCTION);



JSAST *id = init_js_ast(JS_AST_ID);

js_ast_set_value_str(id, "value");

list_push(func->args, id);

func->fptr = (JSFunction*)builtin_object_keys;

js_ast_set_value_str(func, "keys");

map_set(obj->keyvalue, "keys", func);

}



{

JSAST *func = init_js_ast(JS_AST_FUNCTION);



JSAST *id = init_js_ast(JS_AST_ID);

js_ast_set_value_str(id, "value");

list_push(func->args, id);

func->fptr = (JSFunction*)builtin_object_values;

js_ast_set_value_str(func, "values");

map_set(obj->keyvalue, "values", func);

}


}

