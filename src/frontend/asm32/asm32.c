#include <js/js.h>
#include <js/frontend/asm32/asm32.h>
#include <js/js_string.h>
#include <js/js_list.h>
#include <js/js_eval.h>
#include <js/macros.h>
#include <bootstrap.s.h>
#include <call.s.h>
#include <reset.s.h>
#include <function_begin.s.h>
#include <int.s.h>
#include <assign.s.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define PUSH(c, ast)                               \
  { list_push(stacklist, ast); c = list_indexof_ptr(stacklist, ast); }

#define EMIT(name, ast)                         \
  js_f_asm32_##name(ast, stack, stacklist, execution)


#define MAX_NUM_LEN 128

static char* get_reset_str(int len) {
  const char* template = reset_s;
  char* str = (char*)calloc(reset_s_len + MAX_NUM_LEN, sizeof(char));
  sprintf(str, template, len);
  return str;
}

char* js_f_asm32_entry(const char* filename) {
  char* str = (char*)strdup(bootstrap_s);

  JSExecution execution = {0, 1};
  js_execute_file(filename, &execution);

  JSAST* ast = execution.root;

  list_T* stacklist = init_list(sizeof(JSAST*));

  char* nextstr = js_f_asm32(ast, execution.frame, stacklist, &execution);

  if (nextstr != 0) {
    js_str_append(&str, nextstr);
    free(nextstr);
  }
  js_execution_free(&execution);
  return str;
}




char* js_f_asm32_compound(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  return list_map_str(ast->children, (ListMapStrFunc)js_f_asm32, stack, stacklist, execution);
}

char* js_f_asm32_function(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  char* name = js_ast_str_value(ast);

  // rewrite "main" to "_start"
  if (strcmp(name, "main") == 0) {
    JSAST* copy = js_ast_copy(ast);
    js_ast_set_value_str(copy, "_start");
    free(name);
    name = js_ast_str_value(copy);
    ast = copy;
    map_set(stack, "main", copy);
  }

  execution->dry = 0;
  ast = js_eval(ast, stack, execution);
  execution->dry = 1;
  const char* template = function_begin_s;

  char* str = (char*)calloc((function_begin_s_len + strlen(name) * 5) + 1, sizeof(char));
  sprintf(str, template, name);

  return str;
}

char* js_f_asm32_call(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  JSAST* left = js_eval(ast->left, stack, execution);
  char* args_str = OR(list_map_str(ast->args, js_f_asm32, stack, stacklist, execution), strdup(""));
  const char* template = call_s;
  char* resetstr = get_reset_str(ast->args->size);
  char* func_name = js_ast_str_value(left);
  char* str = (char*)calloc(call_s_len + strlen(args_str) + 1 + strlen(resetstr) + 1  + strlen(func_name) + 1, sizeof(char));
  sprintf(str, template, args_str, func_name, resetstr);

  free(args_str);
  free(resetstr);
  return str;
}

char* js_f_asm32_number(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  const char* template = int_s;
  char* str = (char*)calloc(int_s_len + (MAX_NUM_LEN * 3), sizeof(char));
  int c = 0;
  PUSH(c, ast);
  sprintf(str, template, ast->value_int, ast->value_int, c);
  return str;
}

char* js_f_asm32_definition(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  JSAST* right = js_eval(ast->right, stack, execution);
  int c = stacklist->size;

  char* rightstr = js_f_asm32(right, stack, stacklist, execution);
  const char* template = assign_s;
  char* str = 0;
  char* str2 = (char*)calloc(assign_s_len + strlen(rightstr) + 1, sizeof(char));
  sprintf(str2, template, c);

  js_str_append(&str, rightstr);
  js_str_append(&str, str2);

  return str;
}

char* js_f_asm32(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  switch (ast->type) {
    case JS_AST_COMPOUND: return EMIT(compound, ast); break;
    case JS_AST_CALL: return EMIT(call, ast); break;
    case JS_AST_FUNCTION: return EMIT(function, ast); break;
    case JS_AST_NUMBER: return EMIT(number, ast); break;
    case JS_AST_DEFINITION: return EMIT(definition, ast); break;
    default: return strdup(""); break;
  }
  return strdup("");
}
