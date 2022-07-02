#include <assert.h>
#include <js/js.h>
#include <js/js_eval.h>
#include <js/js_path.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define GC execution->gc

#define MARK(item)                                                             \
  {                                                                            \
    if (GC == 0) {                                                             \
      printf("ERROR (%s): Garbage collector is NULL.\n", __func__);            \
      exit(1);                                                                 \
    }                                                                          \
    js_gc_ast(GC, item);                                                       \
  }

char *js_dirname(map_T *stack) {
  if (!stack) return 0;
  JSAST *stringast = (JSAST *)map_get_value(stack, "__dirname");
  if (!stringast) return strdup("./");
  if (!stringast->value_str_ptr) return strdup("./");
  if (stringast->value_str) return stringast->value_str;
  if (!*stringast->value_str_ptr) return strdup("./");
  return strdup(*stringast->value_str_ptr);
}

void js_export_symbol(char *name, JSAST *symbol, map_T *stack,
                      JSExecution *execution) {
  JSAST *module = (JSAST *)map_get_value(stack, "module");
  JSAST *exports = (JSAST *)map_get_value(module->keyvalue, "exports");
  map_set(exports->keyvalue, name, symbol);
}

unsigned int js_is_dry(map_T *stack, JSExecution *execution) {
  if (execution == 0)
    return 0;
  return execution->dry;
}

unsigned int js_is_true(JSAST *ast) {
  return ((ast->is_true) || ast->value_int);
}

unsigned int js_is_empty(JSAST *ast) {
  return ((ast->value_int <= 0 && ast->value_num <= 0 && ast->value_str == 0) ||
          (ast->value_str != 0 && strlen(ast->value_str) == 0));
}

void stack_pop(map_T *stack, const char *key) {
  if (key == 0)
    return;
  JSAST *ast = (JSAST *)map_get_value(stack, (char*)key);
  if (ast == 0)
    return;
  map_unset(stack, (char*)key);
  // js_ast_maybe_free(ast);
}

JSAST *js_eval_string_concat(JSAST *left, JSAST *right) {
  JSAST *new_string = init_js_ast_result(JS_AST_STRING);
  // TODO: mark

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
    JSAST *left = js_eval(ast->left, stack, execution);                        \
    JSAST *right = js_eval(ast->right, stack, execution);                      \
    if (left->type == JS_AST_STRING || right->type == JS_AST_STRING)           \
      return js_eval_string_concat(left, right);                               \
    float x = left->value_num;                                                 \
    float y = right->value_num;                                                \
    JSAST *name = init_js_ast_result(JS_AST_NUMBER);                           \
    MARK(name);                                                                \
    name->value_num = x op y;                                                  \
    name->value_int = (int)name->value_num;                                    \
    return name;                                                               \
  }

#define MATH_OP_F_1(name, ast, op, y, stack)                                   \
  {                                                                            \
    JSAST *left = js_eval(ast, stack, execution);                              \
    float x = left->value_num;                                                 \
    JSAST *name = init_js_ast_result(JS_AST_NUMBER);                           \
    MARK(name);                                                                \
    name->value_num = x op y;                                                  \
    name->value_int = (int)name->value_num;                                    \
    left->value_num = name->value_num;                                         \
    left->value_int = name->value_int;                                         \
    return name;                                                               \
  }

#define MATH_OP_I_R1(name, ast, op, stack)                                     \
  {                                                                            \
    JSAST *left = js_eval(ast, stack, execution);                              \
    int x = left->value_int;                                                   \
    JSAST *name = init_js_ast_result(JS_AST_NUMBER);                           \
    MARK(name);                                                                \
    name->value_int = op x;                                                    \
    name->value_num = (float)ceil(name->value_int);                            \
    left->value_num = name->value_num;                                         \
    left->value_int = name->value_int;                                         \
    return name;                                                               \
  }

#define MATH_OP_I(name, ast, op, stack)                                        \
  {                                                                            \
    JSAST *left = js_eval(ast->left, stack, execution);                        \
    JSAST *right = js_eval(ast->right, stack, execution);                      \
    int x = left->value_int;                                                   \
    int y = right->value_int;                                                  \
    JSAST *name = init_js_ast_result(JS_AST_NUMBER);                           \
    MARK(name);                                                                \
    name->value_int = x op y;                                                  \
    name->value_num = (float)name->value_int;                                  \
    return name;                                                               \
  }

JSAST *js_eval(JSAST *ast, map_T *stack, JSExecution *execution) {
  JSAST *R = ast;
  switch (ast->type) {
  case JS_AST_IMPORT:
    R = js_eval_import(ast, stack, execution);
    break;
  case JS_AST_CONSTRUCT:
    R = js_eval_construct(ast, stack, execution);
    break;
  case JS_AST_THROW:
    R = js_eval_throw(ast, stack, execution);
    break;
  case JS_AST_TRY:
    R = js_eval_try(ast, stack, execution);
    break;
  case JS_AST_CATCH:
    R = js_eval_catch(ast, stack, execution);
    break;
  case JS_AST_COMPOUND:
    R = js_eval_compound(ast, stack, execution);
    break;
  case JS_AST_STATEMENT:
    R = js_eval_statement(ast, stack, execution);
    break;
  case JS_AST_RETURN:
    R = js_eval_statement(ast, stack, execution);
    break;
  case JS_AST_CALL:
    R = js_eval_call(ast, stack, execution);
    break;
  case JS_AST_STRING:
    R = js_eval_string(ast, stack, execution);
    break;
  case JS_AST_NUMBER:
    R = js_eval_number(ast, stack, execution);
    break;
  case JS_AST_ASSIGNMENT:
    R = js_eval_assignment(ast, stack, execution);
    break;
  case JS_AST_DEFINITION:
    R = js_eval_definition(ast, stack, execution);
    break;
  case JS_AST_FUNCTION:
    R = js_eval_function(ast, stack, execution);
    break;
  case JS_AST_BINOP:
    R = js_eval_binop(ast, stack, execution);
    break;
  case JS_AST_UNOP:
    R = js_eval_unop(ast, stack, execution);
    break;
  case JS_AST_ID:
    R = js_eval_id(ast, stack, execution);
    break;
  case JS_AST_IF:
    R = js_eval_if(ast, stack, execution);
    break;
  case JS_AST_WHILE:
    R = js_eval_while(ast, stack, execution);
  case JS_AST_FOR:
    R = js_eval_for(ast, stack, execution);
    break;
  case JS_AST_PROPERTY_ACCESS:
    R = js_eval_property_access(ast, stack, execution);
    break;
  default: { R = ast; }
  }

  if (R->is_result) {
  }

  return R;
}
JSAST *js_maybe_eval(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast == 0) {
    JSAST *newast = init_js_ast_result(JS_AST_UNDEFINED);
    MARK(newast);
    return newast;
  }
  return js_eval(ast, stack, execution);
}

JSAST *js_eval_compound(JSAST *ast, map_T *stack, JSExecution *execution) {
  for (uint32_t i = 0; i < ast->children->size; i++) {
    JSAST *child = (JSAST *)ast->children->items[i];
    if (child == 0)
      continue;

    js_eval(child, stack, execution);
  }

  return ast;
}
JSAST *js_eval_statement(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast->right) {
    JSAST *ret = js_eval(ast->right, stack, execution);

    if (ast->type == JS_AST_RETURN && ret != 0) {
      stack_pop(stack, STACK_ADDR_RETURN);
      map_set(stack, STACK_ADDR_RETURN, (JSAST *)ret);
    }
  }

  return ast;
}

JSAST *js_call_function(JSAST *self, JSAST *ast, JSFunction *fptr,
                        list_T *call_args, list_T *expected_args, map_T *stack,
                        JSExecution *execution) {

  stack_pop(stack, STACK_ADDR_RETURN);

  list_T *evaluated_args = init_list(sizeof(JSAST *));

  if (expected_args != 0) {
    for (uint32_t i = 0; i < expected_args->size; i++) {
      if (i >= call_args->size)
        break;

      JSAST *farg = (JSAST *)expected_args->items[i];
      JSAST *carg = (JSAST *)call_args->items[i];
      JSAST *evaluated = js_eval(carg, stack, execution);

      list_push(evaluated_args, evaluated);
      map_set(stack, farg->value_str, evaluated);
    }
  }

  JSAST *result = 0;

  if (fptr != 0) {
    result = (JSAST *)fptr(self, evaluated_args, stack, execution);
  } else if (ast && ast->body) {
    result = js_eval(ast->body, stack, execution);
  }

  list_free_shallow(evaluated_args);

  JSAST *ret = (JSAST *)map_get_value(stack, STACK_ADDR_RETURN);
  ret = ret ? ret : result ? result : init_js_ast_result(JS_AST_UNDEFINED);
  MARK(ret);

  return ret;
}

JSAST *js_eval_call(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast->left == 0) {
    printf("Error: undefined\n");
  }

  JSAST *left = js_eval(ast->left, stack, execution);

  JSAST *self = left->accessed ? left->accessed : left;

  JSAST *res = 0;
  if (map_get(stack, STACK_ADDR_CONSTRUCT_FLAG) != 0) {
    map_set(stack, STACK_ADDR_CONSTRUCT_CALLER, ast);
    return left;
  }

  if (left->fptr == 0 && left->type != JS_AST_FUNCTION) {
    JSAST *construct = js_ast_get_constructor(left);
    if (construct != 0) {
      return left;
    }
  }

  if (left->fptr == 0 && left->type != JS_AST_FUNCTION) {
    printf("Error: %s is not a function.\n", js_ast_to_string(ast));
    return ast;
  }

  if (left->fptr != 0) {
    res = js_call_function(self, 0, left->fptr, ast->args, left->args, stack,
                           execution);
  } else if (left->type == JS_AST_FUNCTION) {
    JSAST *prev = js_eval_set_current_function(stack, ast->left);
    res = js_call_function(self, left, 0, ast->args, left->args, stack,
                           execution);

    js_eval_set_current_function(stack, prev);
  }

  res = res ? res : init_js_ast_result(JS_AST_UNDEFINED);
  MARK(res);
  return res;
}
JSAST *js_eval_string(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast->value_str_ptr != 0) {
    JSAST *newast = init_js_ast_result(JS_AST_STRING);
    MARK(newast);
    js_ast_set_value_str(newast, *ast->value_str_ptr);
    return newast;
  }
  return ast;
}
JSAST *js_eval_number(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast == 0)
    return ast;
  if (ast->value_int_size_ptr != 0) {
    ast->value_int = (int)*ast->value_int_size_ptr;
    ast->value_num = (float)*ast->value_int_size_ptr;
  }

  return ast;
}
JSAST *js_eval_function(JSAST *ast, map_T *stack, JSExecution *execution) {
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
      ast->args->items[i] = js_eval(ast->args->items[i], stack, execution);
      }*/

  js_export_symbol(ast->value_str, ast, stack, execution);

  if (js_is_dry(stack, execution) == 0) {
    map_set(stack, ast->value_str, ast);
  }

  // if (ast->body != 0) {
  //  return js_eval(ast->body, stack, execution);
  //}

  return ast;
}

JSAST *js_eval_import(JSAST *ast, map_T *stack, JSExecution *execution) {
  char *filepath = ast->value_str;
  char *__dirname = js_dirname(stack);

  char *goodpath = path_resolve(filepath, __dirname);

  JSExecution _execution = {1, 1};
  js_execute_file((const char *)goodpath, &_execution);
  free(goodpath);
  free(__dirname);

  JSAST *module = js_get_module(_execution.frame);
  assert(module != 0);
  JSAST *exports = js_get_exports(module);
  assert(exports != 0);

  list_T *symbols = js_ast_get_values(exports);

  JSAST *left = ast->left ? js_eval(ast->left, stack, execution) : 0;

  list_T *keys = left ? js_ast_get_keys(left) : 0;

  for (size_t i = 0; i < symbols->size; i++) {
    JSAST *symbol = (JSAST *)symbols->items[i];
    if (symbol->value_str == 0)
      continue;

    unsigned int should_keep =
        keys == 0 ? 1 : list_contains_str(keys, symbol->value_str);

    if (should_keep) {
      JSAST *copy = js_ast_copy(symbol);
      js_gc_ast(execution->gc, copy);
      js_eval(copy, stack, execution);
    }
  }

  if (keys != 0) {
    list_free_shallow(keys);
  }

  if (symbols != 0) {
    list_free_shallow(symbols);
  }

  js_execution_free(&_execution);

  return ast;
}

JSAST *js_eval_if(JSAST *ast, map_T *stack, JSExecution *execution) {
  unsigned int is_expr_true = 0;

  if (ast->expr == 0) {
    is_expr_true = 1;
  } else {
    JSAST *expr = js_eval(ast->expr, stack, execution);
    is_expr_true = js_is_true(expr);
  }

  if (is_expr_true && ast->body) {
    return js_eval(ast->body, stack, execution);
  } else if (ast->right) {
    return js_eval_if(ast->right, stack, execution);
  }

  return ast;
}

JSAST *js_eval_while(JSAST *ast, map_T *stack, JSExecution *execution) {
  while (js_is_true(js_eval(ast->expr, stack, execution)) && ast->body != 0) {
    js_eval(ast->body, stack, execution);
  }

  return ast;
}

JSAST *js_eval_for(JSAST *ast, map_T *stack, JSExecution *execution) {
  JSAST *statement1 = ast->args->size > 0 ? (JSAST *)ast->args->items[0] : 0;
  JSAST *statement2 = ast->args->size > 1 ? (JSAST *)ast->args->items[1] : 0;
  JSAST *statement3 = ast->args->size > 2 ? (JSAST *)ast->args->items[2] : 0;

  for (js_maybe_eval(statement1, stack, execution);
       js_is_true(js_maybe_eval(statement2, stack, execution));
       js_maybe_eval(statement3, stack, execution)) {
    js_eval(ast->body, stack, execution);
  }

  return ast;
}

JSAST *js_eval_construct(JSAST *ast, map_T *stack, JSExecution *execution) {
  int flag = 1;
  map_set(stack, STACK_ADDR_CONSTRUCT_FLAG, &flag);
  JSAST *right = js_eval(ast->right, stack, execution);
  map_unset(stack, STACK_ADDR_CONSTRUCT_FLAG);
  JSAST *caller = (JSAST *)map_get_value(stack, STACK_ADDR_CONSTRUCT_CALLER);
  assert(caller != 0);
  assert(caller->type == JS_AST_CALL);
  assert(right != 0);
  assert(right->fptr != 0 || right->type == JS_AST_FUNCTION);
  JSAST *construct = js_ast_get_constructor(right);

  if (construct != 0) {
    right = construct;
  }

  JSAST *self = init_js_ast_result(JS_AST_OBJECT);
  MARK(self);
  map_set(stack, STACK_ADDR_THIS, self);
  js_call_function(self, right, right->fptr, caller->args, right->args, stack,
                   execution);

  if (construct == 0)
    map_set(self->keyvalue, "constructor", right);

  stack_pop(stack, STACK_ADDR_THIS);

  return self;
}

JSAST *js_eval_throw(JSAST *ast, map_T *stack, JSExecution *execution) {
  JSAST *right = js_eval(ast->right, stack, execution);
  return right;
}
JSAST *js_eval_try(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast->body == 0)
    return ast;

  JSAST *body = js_eval(ast->body, stack, execution);

  JSAST *except = (JSAST *)map_get_value(stack, STACK_ADDR_EXCEPTION);

  if (except != 0 && ast->right) {
    JSAST *catchblock = ast->right;
    if (catchblock->args && catchblock->args->size) {
      JSAST *first = (JSAST *)catchblock->args->items[0];
      assert(first && first->value_str != 0);
      char *name = first->value_str;
      map_set(stack, name, except);
      JSAST *evaluated = js_eval(catchblock, stack, execution);
      stack_pop(stack, name);
      return evaluated;
    }
    return catchblock;
  }

  return body;
}
JSAST *js_eval_catch(JSAST *ast, map_T *stack, JSExecution *execution) {
  return ast;
}

JSAST *js_eval_definition(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast->right && ast->right->type == JS_AST_FUNCTION) {
    ast->right->value_str =
        ast->left && ast->left->value_str ? strdup(ast->left->value_str) : 0;
  }

  JSAST *value = js_eval(ast->right, stack, execution);

  //JSAST *existing = (JSAST *)map_get_value(stack, ast->left->value_str);

  stack_pop(stack, ast->left->value_str);
  map_set(stack, ast->left->value_str, value);
  return value;
}
JSAST *js_eval_assignment(JSAST *ast, map_T *stack, JSExecution *execution) {
  JSAST *left = js_eval(ast->left, stack, execution);

  if (left == 0) {
    printf("Error: assignment left is 0.\n");
    exit(1);
  }

  if (left->type == JS_AST_ID)
    return ast; // never want to modify ID.

  JSAST *right = js_eval(ast->right, stack, execution);

  if (left && left->type == JS_AST_OBJECT && ast->left->type == JS_AST_BINOP) {
    JSAST *named = js_ast_most_right(ast->left);

    if (named != 0 && named->value_str != 0) {
      JSAST *obj = left;
      left = init_js_ast(JS_AST_UNDEFINED);
      map_set(obj->keyvalue, named->value_str, left);
    }
  }

  if (right->value_str != 0 && left->value_str != right->value_str) {
    if (left->value_str != 0) {
      free(left->value_str);
      left->value_str = 0;
    }
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

JSAST *js_eval_or(JSAST *ast, map_T *stack, JSExecution *execution) {
  JSAST *left = js_eval(ast->left, stack, execution);
  JSAST *right = js_eval(ast->right, stack, execution);

  if (!js_is_empty(left))
    return left;
  if (!js_is_empty(right))
    return right;
  return left;
}

JSAST *js_eval_binop(JSAST *ast, map_T *stack, JSExecution *execution) {
  switch (ast->token_type) {
  case TOKEN_DOT:
    return js_eval_dot(ast, stack, execution);
    break;
  case TOKEN_PIPE_PIPE:
    return js_eval_or(ast, stack, execution);
    break;
  case TOKEN_AND:
    MATH_OP_I(result, ast, &, stack);
    break;
  case TOKEN_PIPE:
    MATH_OP_I(result, ast, |, stack);
    break;
  case TOKEN_BITWISE_XOR:
    MATH_OP_I(result, ast, ^, stack);
    break;
  case TOKEN_BITWISE_ZERO_FILL_LEFT_SHIFT:
    MATH_OP_I(result, ast, <<, stack);
    break;
  case TOKEN_BITWISE_SIGNED_RIGHT_SHIFT:
    MATH_OP_I(result, ast, >>, stack);
    break;
  case TOKEN_BITWISE_ZERO_FILL_RIGHT_SHIFT:
    MATH_OP_I(result, ast, >>, stack);
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

static JSAST *get_property_index_ast(JSAST *ast, map_T *stack,
                                     JSExecution *execution) {
  if (ast->type == JS_AST_PROPERTY_ACCESS) {
    if (ast->args->size <= 0)
      return 0;
    return (JSAST *)ast->args->items[0];
  } else {
    return ast->right;
  }
}

JSAST *js_eval_dot(JSAST *ast, map_T *stack, JSExecution *execution) {
  JSAST *left = js_eval(ast->left, stack, execution);
  JSAST *right = ast->right;
  if (left->type == JS_AST_UNDEFINED) {
    printf("Error: `%s` is undefined.\n", left->value_str);
    exit(1);
  }

  JSAST *accessor = get_property_index_ast(ast, stack, execution);
  if (accessor == ast->right)
    accessor = 0;
  if (accessor != 0)
    accessor = js_eval(accessor, stack, execution);

  JSAST *result = 0;
  map_T *obj = left->keyvalue;
  char *key = right != 0 ? right->value_str : 0;

  if (ast->type == JS_AST_PROPERTY_ACCESS && accessor != 0) {
    if (accessor->type == JS_AST_NUMBER) {
      int index = accessor->value_int;
      result = index + 1 > left->children->size
                   ? init_js_ast_result(JS_AST_UNDEFINED)
                   : (JSAST *)left->children->items[index];

      MARK(result);
    } else if (accessor->type == JS_AST_STRING) {
      key = accessor->value_str;
      result = key ? obj ? (JSAST *)map_get_value(obj, key) : 0 : 0;
    }
  } else {
    result = key ? obj ? (JSAST *)map_get_value(obj, key) : 0 : 0;

    if (result == 0 && left->type == JS_AST_OBJECT) {
      return left;
    }

    JSAST *proto = js_ast_get_prototype(left);

    if (result == 0 && proto != 0) {
      obj = proto->keyvalue;
      result = key ? obj ? (JSAST *)map_get_value(obj, key) : 0 : 0;

      if (result != 0) {
        result = js_eval(result, stack, execution);
      }
    }
  }

  if (result && key) {
    result->accessed = left;
    map_set(stack, key, result);
  }

  if (ast->right != 0) {
    right = js_eval(ast->right, stack, execution);

    if (result == 0) {
      return right;
    }
  }

  result = result ? result : init_js_ast_result(JS_AST_UNDEFINED);
  MARK(result);
  return result;
}

JSAST *js_eval_property_access(JSAST *ast, map_T *stack,
                               JSExecution *execution) {
  JSAST *left = js_eval(ast->left, stack, execution);

  if (ast->args->size <= 0)
    return init_js_ast(
        JS_AST_UNDEFINED); // might not be what we wanna do though.

  if (left->type == JS_AST_OBJECT)
    return js_eval_dot(ast, stack, execution);

//  JSAST *right = ast->right;
  if (left->type == JS_AST_UNDEFINED) {
    printf("Error: `%s` is undefined.\n", left->value_str);
    exit(1);
  }

  list_T *children = js_ast_to_array(left);
  if (children == 0) {
    JSAST *udef = init_js_ast_result(JS_AST_UNDEFINED);
    MARK(udef);
    return udef;
  }

  JSAST *result = 0;
  JSAST *indexast = js_eval((JSAST *)ast->args->items[0], stack, execution);
  int index = indexast->value_int;
  result = index + 1 > children->size ? init_js_ast_result(JS_AST_UNDEFINED)
                                      : (JSAST *)children->items[index];
  MARK(result);

  if (ast->right) {
    /*right = */js_eval(ast->right, stack, execution);
  }

  result = result ? result : init_js_ast_result(JS_AST_UNDEFINED);
  MARK(result);
  return result;
}

JSAST *js_eval_unop_left(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast->left == 0) {
    printf("Error: Eval unop, left is 0\n");
  }
  JSAST *value = js_eval(ast->left, stack, execution);

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

JSAST *js_eval_unop_right(JSAST *ast, map_T *stack, JSExecution *execution) {
  JSAST *value = js_eval(ast->right, stack, execution);
  if (value != 0) {
    switch (ast->token_type) {
    case TOKEN_TILDE:
      MATH_OP_I_R1(result, value, ~, stack);
      break;
    default: {
      printf("Error %s\n", js_token_type_to_str(ast->token_type));
      exit(1);
    }
    }

    return value;
  }

  return ast->right;
}

JSAST *js_eval_unop(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast->left)
    return js_eval_unop_left(ast, stack, execution);
  if (ast->right)
    return js_eval_unop_right(ast, stack, execution);
  JSAST *newast = init_js_ast_result(JS_AST_UNDEFINED);
  MARK(newast);
  return newast;
}

JSAST *js_eval_id(JSAST *ast, map_T *stack, JSExecution *execution) {
  if (ast->value_str == 0) {
    printf("Error: ID has no str value.\n");
    exit(1);
  };
  JSAST *value = (JSAST *)map_get_value(stack, ast->value_str);
  map_bucket_T *b = map_get(stack, ast->value_str);
  if (value == 0 || b == 0) {
    return ast;
  }

  return js_eval(value, stack, execution);
}

JSAST *js_eval_set_current_function(map_T *stack, JSAST *f) {
  if (f == 0)
    return 0;
  JSAST *prev = (JSAST *)map_get_value(stack, STACK_ADDR_FUNCTION);
  map_unset(stack, STACK_ADDR_FUNCTION);
  map_set(stack, STACK_ADDR_FUNCTION, f);
  return prev;
}
