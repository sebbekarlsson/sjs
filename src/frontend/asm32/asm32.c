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
#include <return.s.h>
#include <program_return.s.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_LEN 128

#define PUSH(c, ast)                               \
  { list_push(stacklist, ast); c = list_indexof_ptr(stacklist, ast); }

#define EMIT(name, ast)                         \
  js_f_asm32_##name(ast, stack, stacklist, execution)

#define STACK_ALLOC(str, n)                          \
  { const char* template = "subl $%d, %%esp\n"; char buff[512]; sprintf(buff, template, (int)n); js_str_append(&str, buff); }

#define MOVL(str, value, location)              \
  { const char* template = "movl $0x%s, %d(%%esp)\n"; char buff[512]; sprintf(buff, template, value, (int)location); js_str_append(&str, buff); }

#define PUSHL_STACK_ADDR(str, location)              \
  { const char* template = "pushl %d(%%esp)\n"; char buff[512]; sprintf(buff, template, (int)location); js_str_append(&str, buff); }

#define PUSHL_STACK_TOP(str)                    \
  { js_str_append(&str, "pushl %esp"); }


#define LEAL(str, n, src, dest, sz)              \
  { const char* template = "movl $%d, %%edi\nleal (%s, %%edi, %d), %s\n"; char buff[1024]; sprintf(buff, template, (int)n, src, sz, dest); js_str_append(&str, buff); }

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
  return list_map_str(ast->children, js_f_asm32, stack, stacklist, execution);
}

char* js_f_asm32_function(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  JSAST* prev = js_eval_set_current_function(stack, ast);
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
  char* bodystr = ast->body ? js_f_asm32(ast->body, stack, stacklist, execution) : 0;

  char* str = (char*)calloc((function_begin_s_len + strlen(name) * 5) + 1, sizeof(char));
  sprintf(str, template, name);
  if (bodystr) {
    js_str_append(&str, bodystr);
  }

  js_str_append(&str, program_return_s);
  js_eval_set_current_function(stack, prev);

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

char* js_f_asm32_string(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  char* str = 0;
  int c = 0;
  PUSH(c, ast);
  char* value = js_ast_str_value(ast);
  list_T* chunks = str_to_hex_chunks(value);
  int bytes_needed = (chunks->size * 4) + 4; // 4 * 4
  STACK_ALLOC(str, bytes_needed);
  char* hexstr = str_to_hex(value);

  int start = -bytes_needed;

  for (size_t i = 0; i < chunks->size; i++) {
    char* hexpart = (char*)chunks->items[i];
    MOVL(str, hexpart, start); // put string at start of memory block
    start += 4;
  }


  MOVL(str, "0", start); // terminate string

  LEAL(str, -bytes_needed, "%esp", "%esp", 1);
  //PUSHL_STACK_ADDR(str, -bytes_needed); // push pointer to start of string
  PUSHL_STACK_TOP(str);


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

char* js_f_asm32_ret(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  char* str = 0;
  char* rightstr = ast->right != 0 ? js_f_asm32(ast->right, stack, stacklist, execution) : 0;

  if (rightstr) {
    js_str_append(&str, rightstr);
    free(rightstr);
  }

  JSAST* current_function = (JSAST*)map_get_value(stack, STACK_ADDR_FUNCTION);
  char* func_name = js_ast_str_value(current_function);

  unsigned int should_return = 1;

  if (func_name != 0 && strcmp(func_name, "main") == 0) {
    should_return = 0;
    free(func_name);
  }

  if (should_return) {
    js_str_append(&str, return_s);
  }

  return str;
}

char* js_f_asm32(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution) {
  switch (ast->type) {
    case JS_AST_COMPOUND: return EMIT(compound, ast); break;
    case JS_AST_CALL: return EMIT(call, ast); break;
    case JS_AST_FUNCTION: return EMIT(function, ast); break;
    case JS_AST_STRING: return EMIT(string, ast); break;
    case JS_AST_NUMBER: return EMIT(number, ast); break;
    case JS_AST_DEFINITION: return EMIT(definition, ast); break;
    case JS_AST_RETURN: return EMIT(ret, ast); break;
  }
  return strdup("");
}
