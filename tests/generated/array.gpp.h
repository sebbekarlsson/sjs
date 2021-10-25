void js_builtin_array_init_static_functions(JSAST *obj) {
  
{

JSAST *func = init_js_ast(JS_AST_FUNCTION);



JSAST *id = init_js_ast(JS_AST_ID);

js_ast_set_value_str(id, "value");

list_push(func->args, id);

func->fptr = (JSFunction*)builtin_array_from;

js_ast_set_value_str(func, "from");

map_set(obj->keyvalue, "from", func);

}


}


void js_builtin_array_init_prototype_functions(JSAST *prototype) {
 
{

JSAST *func = init_js_ast(JS_AST_FUNCTION);



JSAST *id = init_js_ast(JS_AST_ID);

js_ast_set_value_str(id, "map_func");

list_push(func->args, id);

func->fptr = (JSFunction*)builtin_array_map;

js_ast_set_value_str(func, "map");

map_set(prototype->keyvalue, "map", func);

}



{

JSAST *func = init_js_ast(JS_AST_FUNCTION);



JSAST *id = init_js_ast(JS_AST_ID);

js_ast_set_value_str(id, "foreach_func");

list_push(func->args, id);

func->fptr = (JSFunction*)builtin_array_foreach;

js_ast_set_value_str(func, "forEach");

map_set(prototype->keyvalue, "forEach", func);

}


}

