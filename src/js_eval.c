#include <js/js_eval.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static unsigned int is_true(JSAST *ast) {
  return ((ast->is_true) || ast->value_int);
}

void stack_pop(map_T *stack, const char *key) {
  JSAST *ast = (JSAST *)map_get_value(stack, key);
  if (ast == 0)
    return;
  map_unset(stack, key);
  js_ast_maybe_free(ast);
}

JSAST *js_eval_string_concat(JSAST *left, JSAST *right) {
  JSAST *new_string = init_js_ast(JS_AST_STRING);

  char *strleft = js_ast_str_value(left);
  char *strright = js_ast_str_value(right);

  js_str_append(&new_string->value_str, strleft);
  js_str_append(&new_string->value_str, strright);

  free(strleft);
  free(strright);

  return new_string;
}

#define MATH_OP_F(name, ast, op, stack)                                        \
  {                                                                            \
    JSAST *left = js_eval(ast->left, stack);                                   \
    JSAST *right = js_eval(ast->right, stack);                                 \
    if (left->type == JS_AST_STRING || right->type == JS_AST_STRING)           \
      return js_eval_string_concat(left, right);                               \
    float x = left->value_num;                                                 \
    float y = right->value_num;                                                \
    JSAST *name = init_js_ast_result(JS_AST_NUMBER);                           \
    name->value_num = x op y;                                                  \
    name->value_int = (int)name->value_num;                                    \
    return name;                                                               \
  }

#define MATH_OP_F_1(name, ast, op, y, stack)                                   \
  {                                                                            \
    JSAST *left = js_eval(ast, stack);                                         \
    float x = left->value_num;                                                 \
    JSAST *name = init_js_ast_result(JS_AST_NUMBER);                           \
    name->value_num = x op y;                                                  \
    name->value_int = (int)name->value_num;                                    \
    left->value_num = name->value_num;                                         \
    left->value_int = name->value_int;                                         \
    return name;                                                               \
  }

#define MATH_OP_I(name, ast, op, stack)                                        \
  {                                                                            \
    JSAST *left = js_eval(ast->left, stack);                                   \
    JSAST *right = js_eval(ast->right, stack);                                 \
    int x = left->value_int;                                                   \
    int y = right->value_int;                                                  \
    JSAST *name = init_js_ast_result(JS_AST_NUMBER);                           \
    name->value_int = x op y;                                                  \
    name->value_num = (float)name->value_int;                                  \
    return name;                                                               \
  }

JSAST *js_eval(JSAST *ast, map_T *stack) {
  switch (ast->type) {
  case JS_AST_COMPOUND:
    return js_eval_compound(ast, stack);
    break;
  case JS_AST_STATEMENT:
    return js_eval_statement(ast, stack);
    break;
  case JS_AST_RETURN:
    return js_eval_statement(ast, stack);
    break;
  case JS_AST_CALL:
    return js_eval_call(ast, stack);
    break;
  case JS_AST_STRING:
    return js_eval_string(ast, stack);
    break;
  case JS_AST_NUMBER:
    return js_eval_number(ast, stack);
    break;
  case JS_AST_ASSIGNMENT:
    return js_eval_assignment(ast, stack);
    break;
  case JS_AST_DEFINITION:
    return js_eval_definition(ast, stack);
    break;
  case JS_AST_FUNCTION:
    return js_eval_function(ast, stack);
    break;
  case JS_AST_BINOP:
    return js_eval_binop(ast, stack);
    break;
  case JS_AST_UNOP:
    return js_eval_unop(ast, stack);
    break;
  case JS_AST_ID:
    return js_eval_id(ast, stack);
    break;
  case JS_AST_IF:
    return js_eval_if(ast, stack);
    break;
  case JS_AST_WHILE:
    return js_eval_while(ast, stack);
  case JS_AST_FOR:
    return js_eval_for(ast, stack);
    break;
  case JS_AST_PROPERTY_ACCESS:
    return js_eval_property_access(ast, stack);
    break;
  default: { return ast; }
  }

  return ast;
}
JSAST *js_maybe_eval(JSAST *ast, map_T *stack) {
  if (ast == 0)
    return init_js_ast_result(JS_AST_UNDEFINED);
  return js_eval(ast, stack);
}

JSAST *js_eval_compound(JSAST *ast, map_T *stack) {
  for (uint32_t i = 0; i < ast->children->size; i++) {
    JSAST *child = (JSAST *)ast->children->items[i];
    if (child == 0)
      continue;

    js_eval(child, stack);
  }

  return ast;
}
JSAST *js_eval_statement(JSAST *ast, map_T *stack) {
  if (ast->right) {
    JSAST *ret = js_eval(ast->right, stack);

    if (ast->type == JS_AST_RETURN && ret != 0) {
      stack_pop(stack, STACK_ADDR_RETURN);
      map_set(stack, STACK_ADDR_RETURN, (JSAST *)ret);
    }
  }

  return ast;
}

JSAST *js_call_function(JSAST *self, JSAST *ast, JSFunction *fptr,
                        list_T *call_args, list_T *expected_args,
                        map_T *stack) {
  stack_pop(stack, STACK_ADDR_RETURN);

  list_T *evaluated_args = init_list(sizeof(JSAST *));

  for (uint32_t i = 0; i < expected_args->size; i++) {
    if (i >= call_args->size)
      break;

    JSAST *farg = (JSAST *)expected_args->items[i];
    JSAST *carg = (JSAST *)call_args->items[i];
    JSAST *evaluated = js_eval(carg, stack);

    list_push(evaluated_args, evaluated);
    map_set(stack, farg->value_str, evaluated);
  }

  JSAST *result = 0;

  if (fptr != 0) {
    result = (JSAST *)fptr(self, evaluated_args, stack);
    list_free_shallow(evaluated_args);
  } else if (ast && ast->body) {
    result = js_eval(ast->body, stack);
  }

  JSAST *ret = (JSAST *)map_get_value(stack, STACK_ADDR_RETURN);

  return ret ? ret : result ? result : init_js_ast_result(JS_AST_UNDEFINED);
}

JSAST *js_eval_call(JSAST *ast, map_T *stack) {
  if (ast->left == 0) {
    printf("Error: undefined\n");
  }

  JSAST *left = js_eval(ast->left, stack);

  JSAST *self = left->accessed ? left->accessed : left;

  if (left->fptr != 0) {
    return js_call_function(self, 0, left->fptr, ast->args, left->args, stack);
  }

  if (left->type == JS_AST_FUNCTION) {
    return js_call_function(self, left, 0, ast->args, left->args, stack);
  }

  return init_js_ast_result(JS_AST_UNDEFINED);
}
JSAST *js_eval_string(JSAST *ast, map_T *stack) { return ast; }
JSAST *js_eval_number(JSAST *ast, map_T *stack) {
  if (ast == 0)
    return ast;
  if (ast->value_int_size_ptr != 0) {
    ast->value_int = (int)*ast->value_int_size_ptr;
    ast->value_num = (float)*ast->value_int_size_ptr;
  }

  return ast;
}
JSAST *js_eval_function(JSAST *ast, map_T *stack) {
  if (ast->fptr != 0)
    return ast;

  if (ast->value_str == 0)
    return ast;
  /*  for (uint32_t i = 0; i < ast->args->size; i++) {
      JSAST* arg = (JSAST*)ast->args->items[i];
      if (arg == 0) continue;
      if (arg->value_str == 0) continue;
      map_bucket_T* b = map_get(stack, arg->value_str);

      if (b == 0) {
        map_set(stack, arg->value_str, init_js_ast_result(JS_AST_UNDEFINED));
      }
    }
    for (uint32_t i = 0; i < ast->args->size; i++) {
      ast->args->items[i] = js_eval(ast->args->items[i], stack);
    }*/

  map_set(stack, ast->value_str, ast);

  // if (ast->body != 0) {
  //  return js_eval(ast->body, stack);
  //}

  return ast;
}

JSAST *js_eval_if(JSAST *ast, map_T *stack) {
  unsigned int is_expr_true = 0;

  if (ast->expr == 0) {
    is_expr_true = 1;
  } else {
    JSAST *expr = js_eval(ast->expr, stack);
    is_expr_true = is_true(expr);
  }

  if (is_expr_true && ast->body) {
    return js_eval(ast->body, stack);
  } else if (ast->right) {
    return js_eval_if(ast->right, stack);
  }

  return ast;
}

JSAST *js_eval_while(JSAST *ast, map_T *stack) {
  while (is_true(js_eval(ast->expr, stack)) && ast->body != 0) {
    js_eval(ast->body, stack);
  }

  return ast;
}

JSAST *js_eval_for(JSAST *ast, map_T *stack) {
  JSAST *statement1 = ast->args->size > 0 ? (JSAST *)ast->args->items[0] : 0;
  JSAST *statement2 = ast->args->size > 1 ? (JSAST *)ast->args->items[1] : 0;
  JSAST *statement3 = ast->args->size > 2 ? (JSAST *)ast->args->items[2] : 0;

  for (js_maybe_eval(statement1, stack);
       is_true(js_maybe_eval(statement2, stack));
       js_maybe_eval(statement3, stack)) {
    js_eval(ast->body, stack);
  }

  return ast;
}

JSAST *js_eval_definition(JSAST *ast, map_T *stack) {
  if (ast->right && ast->right->type == JS_AST_FUNCTION) {
    ast->right->value_str =
        ast->left && ast->left->value_str ? strdup(ast->left->value_str) : 0;
  }

  JSAST *value = js_eval(ast->right, stack);

  JSAST *existing = (JSAST *)map_get_value(stack, ast->left->value_str);

  stack_pop(stack, ast->left->value_str);
  map_set(stack, ast->left->value_str, value);
  return value;
}
JSAST *js_eval_assignment(JSAST *ast, map_T *stack) {
  JSAST *left = js_eval(ast->left, stack);
  if (left->type == JS_AST_ID)
    return ast; // never want to modify ID.

  JSAST *right = js_eval(ast->right, stack);

  if (left->value_str != 0 && right->value_str != 0) {
    free(left->value_str);
    left->value_str = 0;
    left->value_str = strdup(right->value_str);
  }

  switch (ast->token_type) {
  case TOKEN_EQUALS: {
    left->value_int = right->value_int;
    left->value_num = right->value_num;
  } break;
  case TOKEN_MINUS_EQUALS: {
    left->value_int -= right->value_int;
    left->value_num -= right->value_num;
  } break;
  case TOKEN_PLUS_EQUALS: {
    left->value_int += right->value_int;
    left->value_num += right->value_num;
  } break;
  default: {
    printf("Error: %s\n", js_token_type_to_str(ast->token_type));
    exit(1);
  } break;
  }

  return right;
}

JSAST *js_eval_binop(JSAST *ast, map_T *stack) {
  switch (ast->token_type) {
  case TOKEN_DOT:
    return js_eval_dot(ast, stack);
    break;
  case TOKEN_PLUS:
    MATH_OP_F(result, ast, +, stack);
    break;
  case TOKEN_MINUS:
    MATH_OP_F(result, ast, -, stack);
    break;
  case TOKEN_STAR:
    MATH_OP_F(result, ast, *, stack);
    break;
  case TOKEN_DIV:
    MATH_OP_F(result, ast, /, stack);
    break;
  case TOKEN_LT:
    MATH_OP_F(result, ast, <, stack);
    break;
  case TOKEN_GT:
    MATH_OP_F(result, ast, >, stack);
    break;
  case TOKEN_EQUALS_EQUALS:
    MATH_OP_F(result, ast, ==, stack);
    break;
  case TOKEN_EQUALS_EQUALS_EQUALS:
    MATH_OP_F(result, ast, ==, stack);
    break;
  case TOKEN_NOT_EQUALS:
    MATH_OP_F(result, ast, !=, stack);
    break;
  case TOKEN_NOT_EQUALS_EQUALS:
    MATH_OP_F(result, ast, !=, stack);
    break;
  case TOKEN_AND_AND:
    MATH_OP_I(result, ast, &&, stack);
    break;
  case TOKEN_MOD:
    MATH_OP_I(result, ast, %, stack);
    break;
  default: {
    printf("Not sure what to do with `%d`\n", ast->token_type);
    exit(1);
  }
  }

  return ast;
}

static JSAST *get_property_index_ast(JSAST *ast, map_T *stack) {
  if (ast->type == JS_AST_PROPERTY_ACCESS) {
    if (ast->args->size <= 0)
      return 0;
    return (JSAST *)ast->args->items[0];
  } else {
    return ast->right;
  }
}

JSAST *js_eval_dot(JSAST *ast, map_T *stack) {
  JSAST *left = js_eval(ast->left, stack);
  JSAST *right = ast->right;
  if (left->type == JS_AST_UNDEFINED) {
    printf("Error: `%s` is undefined.\n", left->value_str);
    exit(1);
  }

  JSAST *accessor = get_property_index_ast(ast, stack);
  if (accessor == ast->right)
    accessor = 0;
  if (accessor != 0)
    accessor = js_eval(accessor, stack);

  JSAST *result = 0;
  map_T *obj = left->keyvalue;
  char *key = right != 0 ? right->value_str : 0;

  if (ast->type == JS_AST_PROPERTY_ACCESS && accessor != 0) {
    if (accessor->type == JS_AST_NUMBER) {
      int index = accessor->value_int;
      result = index + 1 > left->children->size
                   ? init_js_ast_result(JS_AST_UNDEFINED)
                   : (JSAST *)left->children->items[index];
    } else if (accessor->type == JS_AST_STRING) {
      key = accessor->value_str;
      result = key ? (JSAST *)map_get_value(obj, key) : 0;
    }
  } else {
    result = key ? (JSAST *)map_get_value(obj, key) : 0;

    if (result == 0 && left->prototype != 0) {
      obj = left->prototype->keyvalue;
      result = key ? (JSAST *)map_get_value(obj, key) : 0;

      if (result != 0) {
        result = js_eval(result, stack);
      }
    }
  }

  if (result && key) {
    result->accessed = left;
    map_set(stack, key, result);
  }

  if (ast->right != 0) {
    right = js_eval(ast->right, stack);
    return right;
  }

  return result ? result : init_js_ast_result(JS_AST_UNDEFINED);
}

JSAST *js_eval_property_access(JSAST *ast, map_T *stack) {
  JSAST *left = js_eval(ast->left, stack);

  if (ast->args->size <= 0)
    return init_js_ast(
        JS_AST_UNDEFINED); // might not be what we wanna do though.

  if (left->type == JS_AST_OBJECT)
    return js_eval_dot(ast, stack);

  JSAST *right = ast->right;
  if (left->type == JS_AST_UNDEFINED) {
    printf("Error: `%s` is undefined.\n", left->value_str);
    exit(1);
  }

  list_T *children = js_ast_to_array(left);
  if (children == 0)
    return init_js_ast_result(JS_AST_UNDEFINED);

  JSAST *result = 0;
  JSAST *indexast = js_eval((JSAST *)ast->args->items[0], stack);
  int index = indexast->value_int;
  result = index + 1 > children->size ? init_js_ast_result(JS_AST_UNDEFINED)
                                      : (JSAST *)children->items[index];

  if (ast->right) {
    right = js_eval(ast->right, stack);
  }

  return result ? result : init_js_ast_result(JS_AST_UNDEFINED);
}

JSAST *js_eval_unop_left(JSAST *ast, map_T *stack) {
  if (ast->left == 0) {
    printf("Error: Eval unop, left is 0\n");
  }
  JSAST *value = js_eval(ast->left, stack);

  if (value != 0) {
    switch (ast->token_type) {
    case TOKEN_INCREMENT:
      MATH_OP_F_1(result, value, +, 1, stack);
      break;
    case TOKEN_DECREMENT:
      MATH_OP_F_1(result, value, -, 1, stack);
      break;
    default: {
      printf("Error %s\n", js_token_type_to_str(ast->token_type));
      exit(1);
    }
    }

    return value;
  }

  return ast->left;
}

// TODO: implement
JSAST *js_eval_unop_right(JSAST *ast, map_T *stack) {
  return init_js_ast_result(JS_AST_UNDEFINED);
}

JSAST *js_eval_unop(JSAST *ast, map_T *stack) {
  if (ast->left)
    return js_eval_unop_left(ast, stack);
  if (ast->right)
    return js_eval_unop_right(ast, stack);
  return init_js_ast_result(JS_AST_UNDEFINED);
}

JSAST *js_eval_id(JSAST *ast, map_T *stack) {
  if (ast->value_str == 0) {
    printf("Error: ID has no str value.\n");
    exit(1);
  };
  JSAST *value = (JSAST *)map_get_value(stack, ast->value_str);
  map_bucket_T *b = map_get(stack, ast->value_str);
  if (value == 0 || b == 0) {
    printf("Undefined variable `%s`\n", ast->value_str);
    exit(1);
  }

  return value;
}
