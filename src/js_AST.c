#include <js/builtins/array.h>
#include <js/builtins/string.h>
#include <js/js_AST.h>
#include <js/js_string.h>
#include <js/macros.h>
#include <stdio.h>
#include <string.h>

JSAST *js_ast_constructor(JSAST *ast, JSASTType type) {
  ast->type = type;
  ast->keyvalue = NEW_MAP();
  ast->is_result = 0;
  ast->prototype = 0;
  ast->fptr = 0;
  ast->value_int_ptr = 0;
  ast->value_int_size_ptr = 0;
  switch (type) {
  case JS_AST_ARRAY: {
    ast->prototype = init_js_builtin_array_prototype(ast);
  } break;
  case JS_AST_STRING: {
    ast->prototype = init_js_builtin_string_prototype(ast);
  } break;
  default: {
    // noop
  } break;
  }

  return ast;
}

JSAST *init_js_ast(JSASTType type) {
  JSAST *ast = NEW(JSAST);
  ast->args = init_list(sizeof(JSAST *));
  ast->children = init_list(sizeof(JSAST *));
  ast = js_ast_constructor(ast, type);
  return ast;
}

JSAST *init_js_ast_blank(JSASTType type) {
  return js_ast_constructor(NEW(JSAST), type);
}

JSAST *init_js_ast_result(JSASTType type) {
  JSAST *ast = init_js_ast(type);
  ast->is_result = 1;
  return ast;
}

static list_T* js_copy_ast_list(list_T* list) {
  if (list == 0) return 0;
  list_T* newlist = init_list(list->item_size);

  for (size_t i = 0; i < list->size; i++) {
    JSAST* child = (JSAST*)list->items[i];
    if (child == 0) continue;

    list_push(newlist, js_ast_copy(child));
  }

  return newlist;
}

static map_T* js_copy_ast_map(map_T* src) {
  map_T* copy = NEW_MAP();

  char** keys = 0;
  size_t length = 0;
  map_get_keys(src, &keys, &length);

  for (size_t i = 0; i < length; i++) {
    char* key = keys[i];
    if (key == 0) continue;
    JSAST* item = (JSAST*)map_get_value(src, key);
    if (item == 0) continue;
    map_set(copy, key, item);
  }

  return copy;
}

JSAST* js_ast_copy(JSAST* src) {
  if (src == 0) return 0;
  JSAST* copy = init_js_ast(src->type);

  copy->left = js_ast_copy(src->left);
  copy->right = js_ast_copy(src->right);
  copy->body = js_ast_copy(src->body);
  copy->expr = js_ast_copy(src->expr);
  copy->prototype = js_ast_copy(src->prototype);

  copy->value_num = src->value_num;
  copy->value_int = src->value_int;
  copy->value_int_ptr = src->value_int_ptr;
  copy->value_int_size_ptr = src->value_int_size_ptr;
  copy->string_length = src->string_length;
  copy->value_str_ptr = src->value_str_ptr;
  copy->any_ptr = src->any_ptr;
  copy->value_str = src->value_str ? strdup(src->value_str) : 0;
  copy->children = js_copy_ast_list(src->children);
  copy->args = js_copy_ast_list(src->args);
  copy->keyvalue = js_copy_ast_map(src->keyvalue);
  copy->fptr = src->fptr;
  copy->is_result = src->is_result;
  copy->is_true = src->is_true;
  copy->exported = src->exported;
  copy->token_type = src->token_type;

  return copy;
}

void js_ast_set_value_str(JSAST *ast, char *value_str) {
  if (value_str == 0)
    return;
  if (ast->value_str != 0)
    free(ast->value_str);
  ast->value_str = strdup(value_str);
  ast->string_length = strlen(value_str);
}

char *js_ast_str_value(JSAST *ast) {
  if (ast == 0)
    return strdup("");

  if (ast->type != JS_AST_NUMBER) {
    if (ast->value_str != 0 && strlen(ast->value_str) > 0)
      return strdup(ast->value_str);
  }

  char buff[128];
  sprintf(buff, "%1.2f", ast->value_num);
  return strdup(buff);
}

static void _free_ast_list(list_T *list) {
  if (list == 0)
    return;
  if (list->items != 0) {
    for (uint32_t i = 0; i < list->size; i++) {
      JSAST *ast = (JSAST *)list->items[i];
      js_ast_free(ast);
    }
    free(list->items);
  }

  free(list);
}

void js_ast_free(JSAST *ast) {
  if (ast == 0)
    return;
  if (ast->value_str != 0) {
    free(ast->value_str);
    ast->value_str = 0;
  }

  if (ast->body != 0) {
    js_ast_free(ast->body);
    ast->body = 0;
  }
  if (ast->left != 0) {
    js_ast_free(ast->left);
    ast->left = 0;
  }
  if (ast->right != 0) {
    js_ast_free(ast->right);
    ast->right = 0;
  }

  if (ast->expr != 0) {
    js_ast_free(ast->expr);
    ast->expr = 0;
  }

  if (ast->prototype != 0) {
    js_ast_free(ast->prototype);
    ast->prototype = 0;
  }

  map_free(ast->keyvalue);

  _free_ast_list(ast->args);
  _free_ast_list(ast->children);
}

char *js_ast_array_to_string(JSAST *ast) {
  char *str = 0;
  js_str_append(&str, "[");
  for (uint32_t i = 0; i < ast->children->size; i++) {
    JSAST *child = (JSAST *)ast->children->items[i];
    if (child == 0)
      continue;
    char *childstr = js_ast_to_string(child);

    if (childstr) {
      js_str_append(&str, childstr);
      free(childstr);
    }

    if (i < ast->children->size - 1) {
      js_str_append(&str, ", ");
    }
  }
  js_str_append(&str, "]");

  return str;
}
char *js_ast_object_to_string(JSAST *ast) {
  char *str = 0;
  char **keys = 0;
  unsigned int length = 0;

  map_get_keys(ast->keyvalue, &keys, &length);

  js_str_append(&str, "{");

  for (unsigned int i = 0; i < length; i++) {
    const char *k = (const char *)keys[i];
    JSAST *child = (JSAST *)map_get_value(ast->keyvalue, k);
    if (child == 0)
      continue;

    char *childstr = js_ast_to_string(child);

    js_str_append(&str, k);
    js_str_append(&str, ": ");
    if (childstr) {
      js_str_append(&str, childstr);
      free(childstr);
    }

    if (i < length - 1) {
      js_str_append(&str, ", ");
    }
  }

  js_str_append(&str, "}");

  return str;
}
char *js_ast_function_to_string(JSAST *ast) {
  char *str = 0;
  js_str_append(&str, "ƒ ");
  char *name = js_ast_str_value(ast);
  if (name) {
    js_str_append(&str, name);
    free(name);
  }
  js_str_append(&str, "(");
  for (uint32_t i = 0; i < ast->args->size; i++) {
    JSAST *child = (JSAST *)ast->args->items[i];
    if (child == 0)
      continue;
    char *childstr = js_ast_to_string(child);

    if (childstr) {
      js_str_append(&str, childstr);
      free(childstr);
    }

    if (i < ast->args->size - 1) {
      js_str_append(&str, ",");
    }
  }
  js_str_append(&str, ")");
  return str;
}

char *js_ast_to_string(JSAST *ast) {
  switch (ast->type) {
  case JS_AST_ARRAY:
    return js_ast_array_to_string(ast);
    break;
  case JS_AST_OBJECT:
    return js_ast_object_to_string(ast);
    break;
  case JS_AST_FUNCTION:
    return js_ast_function_to_string(ast);
    break;
  default: { return js_ast_str_value(ast); }
  }

  return js_ast_str_value(ast);
}

void js_ast_maybe_free(JSAST *ast) {
  if (ast == 0)
    return;
  if (ast->is_result == 0)
    return;
  js_ast_free(ast);
}

list_T *js_ast_to_array(JSAST *ast) {
  if (ast == 0)
    return 0;

  if (ast->type == JS_AST_STRING && ast->value_str != 0) {
    list_T *newlist = init_list(sizeof(JSAST *));
    for (size_t i = 0; i < ast->string_length; i++) {
      JSAST *stringast = init_js_ast_result(JS_AST_STRING);
      char *strval = char_to_str(ast->value_str[i]);
      js_ast_set_value_str(stringast, strval);
      free(strval);
      list_push(newlist, stringast);
    }

    return newlist;
  }

  if (ast->children)
    return ast->children;

  return 0;
}

JSIterator js_ast_iterate(JSAST *ast) {
  return js_iterator(ast->children->items, &ast->children->size);
}

list_T *js_ast_get_values(JSAST *ast) {
  list_T *new_arr = init_list(sizeof(JSAST *));

  unsigned int length = 0;
  char **keys = 0;
  map_get_keys(ast->keyvalue, &keys, &length);

  for (size_t i = 0; i < length; i++) {
    char *key = keys[i];
    if (key == 0)
      continue;

    JSAST *ast_v = (JSAST *)map_get_value(ast->keyvalue, key);
    if (ast_v == 0)
      continue;

    list_push(new_arr, ast_v);
  }

  return new_arr;
}

list_T *js_ast_get_keys(JSAST *ast) {
  list_T *new_arr = init_list(sizeof(char *));

  unsigned int length = 0;
  char **keys = 0;
  map_get_keys(ast->keyvalue, &keys, &length);

  for (size_t i = 0; i < length; i++) {
    char *key = keys[i];
    if (key == 0)
      continue;

    list_push(new_arr, key);
  }

  return new_arr;
}

list_T *js_ast_get_keys_asts(JSAST *ast) {
  list_T *new_arr = init_list(sizeof(JSAST *));

  unsigned int length = 0;
  char **keys = 0;
  map_get_keys(ast->keyvalue, &keys, &length);

  for (size_t i = 0; i < length; i++) {
    char *key = keys[i];
    if (key == 0)
      continue;

    JSAST *strast = init_js_ast_result(JS_AST_STRING);
    js_ast_set_value_str(strast, key);
    list_push(new_arr, strast);
  }

  return new_arr;
}
