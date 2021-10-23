#include <js/js_eval.h>
#include <stdint.h>
#include <stdio.h>

JSAST *js_eval(JSAST *ast, map_T* stack) {
  switch (ast->type) {
    case JS_AST_COMPOUND: return js_eval_compound(ast, stack); break;
    case JS_AST_STATEMENT: return js_eval_statement(ast, stack); break;
    case JS_AST_CALL: return js_eval_call(ast, stack); break;
    case JS_AST_STRING: return js_eval_string(ast, stack); break;
    case JS_AST_NUMBER: return js_eval_number(ast, stack); break;
    case JS_AST_ASSIGNMENT: return js_eval_assign(ast, stack); break;
    case JS_AST_FUNCTION: return js_eval_function(ast, stack); break;
    case JS_AST_BINOP: return js_eval_binop(ast, stack); break;
    case JS_AST_UNOP: return js_eval_unop(ast, stack); break;
    case JS_AST_ID: return js_eval_id(ast, stack); break;
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

  return result ? result : init_js_ast(JS_AST_UNDEFINED);
}
JSAST *js_eval_string(JSAST *ast, map_T* stack) { return ast; }
JSAST *js_eval_number(JSAST *ast, map_T* stack) { return ast; }
JSAST *js_eval_assign(JSAST *ast, map_T* stack) {
  if (ast->left == 0) return ast;
  if (ast->left->value_str == 0) return ast;
  JSAST* value = ast->right ? js_eval(ast->right, stack) : init_js_ast(JS_AST_UNDEFINED);
  map_set(stack, ast->left->value_str, value);

  return value;
}
JSAST *js_eval_function(JSAST *ast, map_T* stack) {
  for (uint32_t i = 0; i < ast->args->size; i++) {
    JSAST* arg = (JSAST*)ast->args->items[i];
    if (arg == 0) continue;
    if (arg->value_str == 0) continue;
    map_bucket_T* b = map_get(stack, arg->value_str);

    if (b == 0) {
      map_set(stack, arg->value_str, init_js_ast(JS_AST_UNDEFINED));
    }
  }
  for (uint32_t i = 0; i < ast->args->size; i++) {
    ast->args->items[i] = js_eval(ast->args->items[i], stack);
  }

  if (ast->body != 0) {
    return js_eval(ast->body, stack);
  }

  return init_js_ast(JS_AST_UNDEFINED);
}

#define MATH_OP(name, ast, op, stack)                     \
  JSAST* left = js_eval(ast->left, stack);                               \
    JSAST* right = js_eval(ast->right, stack);\
    float x = left->value_num;\
    float y = right->value_num;\
    JSAST* name = init_js_ast(JS_AST_NUMBER);\
    name->value_num = x op y;

JSAST* js_eval_math_add(JSAST* ast, map_T* stack) {
  MATH_OP(result, ast, +, stack);
  return result;
}
JSAST* js_eval_math_minus(JSAST* ast, map_T* stack) {
  MATH_OP(result, ast, -, stack);
  return result;
}
JSAST* js_eval_math_star(JSAST* ast, map_T* stack) {
  MATH_OP(result, ast, *, stack);
  return result;
}
JSAST* js_eval_math_div(JSAST* ast, map_T* stack) {
  MATH_OP(result, ast, /, stack);
  return result;
}

JSAST *js_eval_binop(JSAST *ast, map_T* stack) {
  switch (ast->token_type) {
    case TOKEN_DOT: return js_eval_dot(ast, stack); break;
    case TOKEN_PLUS: return js_eval_math_add(ast, stack); break;
    case TOKEN_MINUS: return js_eval_math_minus(ast, stack); break;
    case TOKEN_STAR: return js_eval_math_star(ast, stack); break;
    case TOKEN_DIV: return js_eval_math_div(ast, stack); break;
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



  return result ? result : init_js_ast(JS_AST_UNDEFINED);

}

// TODO: implement
JSAST *js_eval_unop(JSAST *ast, map_T* stack) { return init_js_ast(JS_AST_UNDEFINED); }

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
