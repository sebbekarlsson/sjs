#include <js/js_eval.h>
#include <stdint.h>
#include <stdio.h>

static unsigned int is_true(JSAST* ast) {
  return ((ast->is_true) || ast->value_int);
}

#define MATH_OP_F(name, ast, op, stack)                                \
  {\
    JSAST* left = js_eval(ast->left, stack);        \
    JSAST* right = js_eval(ast->right, stack);\
    float x = left->value_num;\
    float y = right->value_num;\
    JSAST* name = init_js_ast_result(JS_AST_NUMBER);\
    name->value_num = x op y;                       \
    name->value_int = name->value_num;\
return name;                                    \
  }


#define MATH_OP_I(name, ast, op, stack)                                \
  {\
    JSAST* left = js_eval(ast->left, stack);        \
    JSAST* right = js_eval(ast->right, stack);\
    int x = left->value_int;\
    int y = right->value_int;\
    JSAST* name = init_js_ast_result(JS_AST_NUMBER);\
    name->value_int = x op y;                \
    name->value_num = (float)name->value_int;\
return name;                                    \
  }


JSAST *js_eval(JSAST *ast, map_T* stack) {
  switch (ast->type) {
    case JS_AST_COMPOUND: return js_eval_compound(ast, stack); break;
    case JS_AST_STATEMENT: return js_eval_statement(ast, stack); break;
    case JS_AST_CALL: return js_eval_call(ast, stack); break;
    case JS_AST_STRING: return js_eval_string(ast, stack); break;
    case JS_AST_NUMBER: return js_eval_number(ast, stack); break;
    case JS_AST_ASSIGNMENT: return js_eval_assignment(ast, stack); break;
    case JS_AST_DEFINITION: return js_eval_definition(ast, stack); break;
    case JS_AST_FUNCTION: return js_eval_function(ast, stack); break;
    case JS_AST_BINOP: return js_eval_binop(ast, stack); break;
    case JS_AST_UNOP: return js_eval_unop(ast, stack); break;
    case JS_AST_ID: return js_eval_id(ast, stack); break;
    case JS_AST_IF: return js_eval_if(ast, stack); break;
    default: { return ast; }
  }

  return ast;
}
JSAST *js_eval_compound(JSAST *ast, map_T* stack) {
  for (uint32_t i = 0; i < ast->children->size; i++) {
    JSAST* child = (JSAST*) ast->children->items[i];
    if (child == 0) continue;

    ast->children->items[i] = (JSAST*)js_eval(child, stack);
  }

  return ast;
}
JSAST *js_eval_statement(JSAST *ast, map_T* stack) {
  if (ast->right) {
    ast->right = js_eval(ast->right, stack);
  }

  return ast;
}
JSAST *js_eval_call(JSAST *ast, map_T* stack) {
  if (ast->left == 0) {
    printf("Error: undefined\n");
  }

  if (ast->left) {
    ast->left = js_eval(ast->left, stack);
  }

  if (ast->left && ast->left->type != JS_AST_FUNCTION) {
    if (ast->left->value_str) {
      printf("`%s` (%d) is not a function.\n", ast->left->value_str, ast->left->type);
      exit(1);
    } else {
      printf("Error: Trying to call something which is not a function.\n");
      exit(1);
    }
  }

  JSAST* result = 0;

  list_T* function_args = ast->left->args;

  list_T* evaluated_args = init_list(sizeof(JSAST*));

  for (uint32_t i = 0; i < function_args->size; i++) {
    if (i >= ast->args->size) break;

    JSAST* farg = (JSAST*) function_args->items[i];
    JSAST* carg = (JSAST*) ast->args->items[i];
    JSAST* evaluated = js_eval(carg, stack);

    list_push(evaluated_args, evaluated);
    map_set(stack, farg->value_str, evaluated);
  }

  if (ast->left->fptr != 0) {
    result = (JSAST*)ast->left->fptr(evaluated_args, stack);
    list_free_shallow(evaluated_args);
  } else if (ast->left->body) {
    result = js_eval(ast->left->body, stack);
  }

  if (ast->right) {
    ast->right = js_eval(ast->right, stack);
  }

  return result ? result : init_js_ast_result(JS_AST_UNDEFINED);
}
JSAST *js_eval_string(JSAST *ast, map_T* stack) { return ast; }
JSAST *js_eval_number(JSAST *ast, map_T* stack) { return ast; }
JSAST *js_eval_function(JSAST *ast, map_T* stack) {
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

  //if (ast->body != 0) {
  //  return js_eval(ast->body, stack);
  //}

  return ast;
}

JSAST* js_eval_if(JSAST* ast, map_T* stack) {
  unsigned int is_expr_true = 0;

  if (ast->expr == 0) {
    is_expr_true = 1;
  } else {
    JSAST* expr = js_eval(ast->expr, stack);
    is_expr_true = is_true(expr);
  }

  if (is_expr_true && ast->body) {
    return js_eval(ast->body, stack);
  } else if (ast->right) {
    return js_eval_if(ast->right, stack);
  }

  return ast;
}

JSAST* js_eval_definition(JSAST* ast, map_T* stack) {
  ast->left = ast->left;
  JSAST* value = js_eval(ast->right, stack);

  JSAST* existing = (JSAST*)map_get_value(stack, ast->left->value_str);

  if (existing)  {
    if (existing->is_result)
    {
      map_unset(stack, ast->left->value_str);
      js_ast_free(existing);
    }
  }

  map_set(stack, ast->left->value_str, value);
  return value;
}
JSAST* js_eval_assignment(JSAST* ast, map_T* stack) {
  return js_eval_definition(ast, stack);
}






JSAST *js_eval_binop(JSAST *ast, map_T* stack) {
  switch (ast->token_type) {
    case TOKEN_DOT: return js_eval_dot(ast, stack); break;
    case TOKEN_PLUS: MATH_OP_F(result, ast, +, stack); break;
    case TOKEN_MINUS: MATH_OP_F(result, ast, -, stack); break;
    case TOKEN_STAR: MATH_OP_F(result, ast, *, stack); break;
    case TOKEN_DIV: MATH_OP_F(result, ast, /, stack); break;
    case TOKEN_LT: MATH_OP_F(result, ast, <, stack); break;
    case TOKEN_GT: MATH_OP_F(result, ast, >, stack); break;
    case TOKEN_EQUALS_EQUALS: MATH_OP_F(result, ast, ==, stack); break;
    case TOKEN_EQUALS_EQUALS_EQUALS: MATH_OP_F(result, ast, ==, stack); break;
    case TOKEN_NOT_EQUALS: MATH_OP_F(result, ast, !=, stack); break;
    case TOKEN_NOT_EQUALS_EQUALS: MATH_OP_F(result, ast, !=, stack); break;
    case TOKEN_AND_AND: MATH_OP_I(result, ast, &&, stack); break;
    case TOKEN_MOD: MATH_OP_I(result, ast, %, stack); break;
    default: { printf("Not sure what to do with `%d`\n", ast->token_type); exit(1); }
  }

  return ast;
}
JSAST *js_eval_dot(JSAST *ast, map_T* stack) {
  JSAST* left = js_eval(ast->left, stack);
  JSAST* right = ast->right;
  if (left->type == JS_AST_UNDEFINED) {
    printf("Error: `%s` is undefined.\n", left->value_str);
    exit(1);
  }

  map_T* obj = left->keyvalue;
  char* key = right->value_str;
  JSAST* result = key ? (JSAST*)map_get_value(obj, key) : 0;

  if (result && key) {
    map_set(stack, key, result);
  }

  right = js_eval(ast->right, stack);



  return result ? result : init_js_ast_result(JS_AST_UNDEFINED);

}

// TODO: implement
JSAST *js_eval_unop(JSAST *ast, map_T* stack) { return init_js_ast_result(JS_AST_UNDEFINED); }

JSAST* js_eval_id(JSAST* ast, map_T* stack) {
  if (ast->value_str == 0) {
    printf("Error: ID has no str value.\n");
    exit(1);
  };
  JSAST* value = (JSAST*)map_get_value(stack, ast->value_str);
  map_bucket_T* b = map_get(stack, ast->value_str);
  if (value == 0 || b == 0) {
    printf("Undefined variable `%s`\n", ast->value_str);
    exit(1);
  }

  return value;
}
