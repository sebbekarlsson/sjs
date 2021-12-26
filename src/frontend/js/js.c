#include <js/frontend/js/js.h>
#include <js/js.h>
#include <js/js_eval.h>
#include <js/js_mem.h>
#include <string.h>
#include <js/js_string.h>
#include <js/macros.h>
#include <call.js.h>
#include <definition.js.h>
#include <function.js.h>
#include <for.js.h>
#include <construct.js.h>
#include <class.js.h>
#include <while.js.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#define EMIT(name, ast)                         \
  js_f_js_##name(ast, stack, stacklist, execution)

#define EMIT_ANY(ast)                         \
  js_f_js(ast, stack, stacklist, execution)

char* js_f_js_entry(const char* filename) {
  char* str = (char*)strdup("");

  JSExecution execution = {0, 1};
  js_execute_file(filename, &execution);

  JSAST* ast = execution.root;

  list_T* stacklist = init_list(sizeof(JSAST*));

  char* nextstr = js_f_js(ast, execution.frame, stacklist, &execution);

  if (nextstr != 0) {
    js_str_append(&str, nextstr);
    free(nextstr);
  }
  js_execution_free(&execution);
  return str;
}

static char* join(list_T* children, char* delim, map_T* stack, list_T* stacklist, JSExecution* execution) {
  if (!children) return 0;
  if (!children->size) return 0;
      char* args_str = 0;
  for (uint32_t i = 0; i < children->size; i++) {
    JSAST* arg = (JSAST*)children->items[i];
    assert(arg != 0);
    char* s = EMIT_ANY(arg);
    assert(s != 0);
    if (!strlen(s)) { free(s); continue; }
    js_str_trim_last(&s);
    js_str_append(&args_str, s);
    free(s);

    if (i < children->size-1)
      js_str_append(&args_str, delim);
  }

  return OR(args_str, "");

}

char* js_f_js_compound(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
    char* args_str = 0;
  for (uint32_t i = 0; i < ast->children->size; i++) {
    JSAST* arg = (JSAST*)ast->children->items[i];
    assert(arg != 0);
    char* s = EMIT_ANY(arg);
    assert(s != 0);
    if (!strlen(s)) { free(s); continue; }
    js_str_trim_last(&s);
    js_str_append(&args_str, s);
    free(s);
    js_str_append(&args_str, ";");

  }

  return OR(args_str, strdup(""));
  //return list_map_str(ast->children, (ListMapStrFunc)js_f_js, stack, stacklist, execution);
}

char* js_f_js_call(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  const char* template = (const char*)call_js;

  char* args_str = 0;
  for (uint32_t i = 0; i < ast->args->size; i++) {
    JSAST* arg = (JSAST*)ast->args->items[i];
    assert(arg != 0);
    char* s = EMIT_ANY(arg);
    assert(s != 0);
    js_str_append(&args_str, s);
    free(s);
  }

  char* leftstr = ast->left ? EMIT_ANY(ast->left) : ast->value_str ? strdup(ast->value_str) : strdup("");

  args_str = OR(args_str, strdup(""));

  char* buff = js_calloc(call_js_len + strlen(args_str) + strlen(leftstr) + 16, sizeof(char));
  sprintf(buff, template, leftstr, args_str);

  free(leftstr);

  free(args_str);

  return buff;
}

char* js_f_js_function(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  js_eval(ast, stack, execution);
    const char* template = (const char*)function_js;

  char* args_str = 0;
  for (uint32_t i = 0; i < ast->args->size; i++) {
    JSAST* arg = (JSAST*)ast->args->items[i];
    assert(arg != 0);
    char* s = EMIT_ANY(arg);
    assert(s != 0);
    js_str_append(&args_str, s);
    free(s);
  }

  char* func_name = ast->value_str ? strdup(ast->value_str) : 0;

  if (!func_name) {
    char* namebuff = js_calloc(128, sizeof(char));
    sprintf(namebuff, "_%p", ast);
    func_name = namebuff;
  }

  args_str = OR(args_str, strdup(""));

  char* body_str = 0;

  if (ast->body) {
    body_str = EMIT_ANY(ast->body);
  } else {
    body_str = strdup("");
  }

  char* buff = js_calloc(function_js_len + strlen(args_str) + strlen(body_str) + strlen(func_name) + 16, sizeof(char));
  sprintf(buff, template, func_name, args_str, body_str);

  free(args_str);
  free(func_name);

  return buff;
}

char* js_f_js_class(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  js_eval(ast, stack, execution);
    const char* template = (const char*)class_js;

  char* args_str = 0;
  for (uint32_t i = 0; i < ast->args->size; i++) {
    JSAST* arg = (JSAST*)ast->args->items[i];
    assert(arg != 0);
    char* s = EMIT_ANY(arg);
    assert(s != 0);
    js_str_append(&args_str, s);
    free(s);
  }

  char* func_name = ast->value_str ? strdup(ast->value_str) : 0;

  if (!func_name) {
    char* namebuff = js_calloc(128, sizeof(char));
    sprintf(namebuff, "_%p", ast);
    func_name = namebuff;
  }

  char* body_str = 0;

  if (ast->body) {
    body_str = EMIT_ANY(ast->body);
  } else {
    body_str = strdup("");
  }

  char* tmp = args_str;
  if (tmp) {
    args_str = strdup("extends ");
    js_str_append(&args_str, tmp);
  } else {
    args_str = strdup("");
  }

  char* buff = js_calloc(class_js_len + strlen(args_str) + strlen(body_str) + strlen(func_name) + 16, sizeof(char));
  sprintf(buff, template, func_name, args_str, body_str);

  free(args_str);
  free(func_name);

  return buff;
}

char* js_f_js_number(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  char* buff = js_calloc(128, sizeof(char));
  sprintf(buff, "%1.6f", (double)OR(ast->value_double, OR(ast->value_num, ast->value_int)));
  return buff;
}

char* js_f_js_int(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  char* buff = js_calloc(128, sizeof(char));
  sprintf(buff, "%d", (int)OR(ast->value_int, OR(ast->value_num, ast->value_double)));
  return buff;
}

char* js_f_js_id(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  return strdup(OR(ast->value_str, ""));
}

char* js_f_js_return(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  char* str = strdup(OR(ast->value_str, ""));
  char* right = ast->right ? EMIT_ANY(ast->right) : 0;

  if (right) {
    js_str_append(&str, " ");
    js_str_append(&str, right);
    free(right);
  }

  return str;
}

char* js_f_js_construct(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution)
{
  js_eval(ast, stack, execution);
  const char* template = (const char*)construct_js;
  char* rightstr = ast->right ? EMIT_ANY(ast->right) : strdup("");
  char* buff = js_calloc(construct_js_len + strlen(rightstr) + 16, sizeof(char));
  sprintf(buff, template, rightstr);
  return buff;
}



char* js_f_js_definition(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution)
{
  js_eval(ast, stack, execution);
  const char* template = (const char*)definition_js;
  assert(ast->left != 0);
  char* leftstr = EMIT_ANY(ast->left);
  char* rightstr = ast->right ? EMIT_ANY(ast->right) : strdup("");
  char* buff = js_calloc(definition_js_len + strlen(leftstr) + strlen(rightstr) + 16, sizeof(char));
  sprintf(buff, template, leftstr, rightstr);
  return buff;
}

char* js_f_js_string(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  char* v = strdup(OR(ast->value_str, ""));
  char* str = 0;
  js_str_append(&str, "\"");
  js_str_append(&str, v);
  js_str_append(&str, "\"");

  free(v);
  return str;
}

char* js_f_js_binop(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  char* leftstr = ast->left ? EMIT_ANY(ast->left) : strdup("");
  char* rightstr = ast->right ? EMIT_ANY(ast->right) : strdup("");
  char* tokstr = 0;
  switch (ast->token_type) {
    case TOKEN_DOT: tokstr = strdup("."); break;
    case TOKEN_PLUS: tokstr = strdup("+"); break;
    case TOKEN_INCREMENT: tokstr = strdup("++"); break;
    case TOKEN_DECREMENT: tokstr = strdup("--"); break;
    case TOKEN_PLUS_EQUALS: tokstr = strdup("+="); break;
    case TOKEN_MINUS: tokstr = strdup("-"); break;
    case TOKEN_MINUS_EQUALS: tokstr = strdup("-="); break;
    case TOKEN_DIV: tokstr = strdup("/"); break;
    case TOKEN_STAR: tokstr = strdup("*"); break;
    case TOKEN_NOT_EQUALS: tokstr = strdup("!="); break;
    case TOKEN_NOT_EQUALS_EQUALS: tokstr = strdup("!=="); break;
    case TOKEN_EQUALS: tokstr = strdup("="); break;
    case TOKEN_EQUALS_EQUALS: tokstr = strdup("=="); break;
    case TOKEN_EQUALS_EQUALS_EQUALS: tokstr = strdup("==="); break;
    case TOKEN_LT: tokstr = strdup("<"); break;
    case TOKEN_GT: tokstr = strdup(">"); break;
    case TOKEN_LT_EQUALS: tokstr = strdup("<="); break;
    case TOKEN_GT_EQUALS: tokstr = strdup(">="); break;
    default: tokstr = strdup("__not_implemented__"); break;
  }

  char* str = 0;
  js_str_append(&str, leftstr);
  js_str_append(&str, tokstr);
  js_str_append(&str, rightstr);

  free(leftstr);
  free(tokstr);
  free(rightstr);

  return str;
}


char* js_f_js_import(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  JSAST* evaluated = js_eval(ast, stack, execution);
  JSAST *left = ast->left ? ast->left : 0;
  assert(left != 0);
  list_T *keys = left ? js_ast_get_keys(left) : 0;
  assert(keys != 0);

  char* str = 0;

  for (uint32_t i = 0; i < keys->size; i++) {
    char* k = keys->items[i];
    assert(k != 0);
    char buff[PATH_MAX];
    sprintf(buff, "import_%s", k);
    if (map_get_value(stack, buff)) {
      char tmpbuff[PATH_MAX];
      sprintf(tmpbuff, "\n/* double import %s */\n", k);
      js_str_append(&str, tmpbuff);
      continue;
    }

    JSAST* symbol = (JSAST*)map_get_value(stack, k);

    if (!symbol) {
      fprintf(stderr, "unable to import `%s`\n", k);
      continue;
    }

    char* symbol_str = EMIT_ANY(symbol);
    assert(symbol_str != 0);

    js_str_append(&str, symbol_str);
    free(symbol_str);

    map_set(stack, buff, symbol);
  }

  return OR(str, strdup(""));
}

char* js_f_js_for(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  const char* template = (const char*)for_js;
  char* args_str = OR(join(ast->args, ";", stack, stacklist, execution), strdup(""));
  char* body_str = OR(EMIT_ANY(OR(ast->body, ast->expr)), strdup(""));

  char* buff = js_calloc(for_js_len + strlen(args_str) + strlen(body_str) + 16, sizeof(char));

  sprintf(buff, template, args_str, body_str);

  return buff;
}

char* js_f_js_while(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  const char* template = (const char*)while_js;
  char* expr_str = ast->expr ? EMIT_ANY(ast->expr) : strdup("");
  char* body_str = ast->body ? OR(EMIT_ANY(ast->body), strdup("")) : strdup("");

  char* buff = js_calloc(while_js_len + strlen(expr_str) + strlen(body_str) + 16, sizeof(char));

  sprintf(buff, template, expr_str, body_str);

  return buff;
}

char* js_f_js(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
    assert(ast != 0);
    switch (ast->type) {
    case JS_AST_COMPOUND: return EMIT(compound, ast); break;
    case JS_AST_CALL: return EMIT(call, ast); break;
        case JS_AST_FUNCTION: case JS_AST_ARROW_FUNCTION: return EMIT(function, ast); break;
    case JS_AST_NUMBER: case JS_AST_FLOAT: return EMIT(number, ast); break;
    case JS_AST_INT: return EMIT(int, ast); break;
    case JS_AST_DEFINITION: return EMIT(definition, ast); break;
    case JS_AST_CLASS: return EMIT(class, ast); break;
    case JS_AST_ID: return EMIT(id, ast); break;
    case JS_AST_RETURN: return EMIT(return, ast);
    case JS_AST_BINOP: return EMIT(binop, ast);
    case JS_AST_STRING: return EMIT(string, ast);
    case JS_AST_IMPORT: return EMIT(import, ast);
    case JS_AST_FOR: return EMIT(for, ast);
    case JS_AST_WHILE: return EMIT(while, ast);
    case JS_AST_UNOP: return EMIT(binop, ast);
    case JS_AST_CONSTRUCT: return EMIT(construct, ast);
    default: return strdup(""); break;
  }
  return strdup("");
}
