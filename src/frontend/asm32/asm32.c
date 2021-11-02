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

#define EMIT(name, ast)                         \
  js_f_asm32_##name(ast, stack, stacklist, execution, im)


char* js_f_asm32_entry(const char* filename) {
  char* str = (char*)strdup(bootstrap_s);

  JSExecution execution = {0, 1};
  js_execute_file(filename, &execution);

  JSAST* ast = execution.root;

  list_T* stacklist = init_list(sizeof(JSAST*));
  IM* im = init_js_instruction_machine();

  char* nextstr = js_f_asm32(ast, execution.frame, stacklist, &execution, im);

  nextstr = js_im_dump(im);

  if (nextstr != 0) {
    js_str_append(&str, nextstr);
    free(nextstr);
  }
  js_execution_free(&execution);
  return str;
}




char* js_f_asm32_compound(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution, IM* im) {
  return list_map_str(ast->children, js_f_asm32, stack, stacklist, execution, im);
}

char* js_f_asm32_function(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution, IM* im) {
  char* str = 0;
  JSAST* prev = js_eval_set_current_function(stack, ast);

  unsigned int is_main = 0;
  char* name = js_ast_str_value(ast);

  // rewrite "main" to "_start"
  if (strcmp(name, "main") == 0) {
    JSAST* copy = js_ast_copy(ast);
    js_ast_set_value_str(copy, "_start");
    free(name);
    name = js_ast_str_value(copy);
    ast = copy;
    map_set(stack, "main", copy);
    is_main = 1;
  }

  js_im_execute(im, (JSInstruction){ INST_FUNCTION, 0, name });

  execution->dry = 0;
  ast = js_eval(ast, stack, execution);
  execution->dry = 1;
  js_eval_set_current_function(stack, prev);
  char* bodystr = ast->body ? js_f_asm32(ast->body, stack, stacklist, execution, im) : 0;

  if (is_main) {
    js_im_execute(im, (JSInstruction){ INST_MOVL, 1, 0, (JSRegister){ 0, R_32_G1 } });
    js_im_execute(im, (JSInstruction){ INST_MOVL_REG, 0, 0, (JSRegister){ 0, R_32_G1 }, (JSRegister){ 0, R_32_G2 } });
    js_im_execute(im, (JSInstruction){ INST_INT, 0x80, 0 });
  }

  return str;
}

char* js_f_asm32_call(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution, IM* im) {
  JSAST* left = js_eval(ast->left, stack, execution);
  char* value = js_ast_str_value(left);

      list_T *evaluated_args = init_list(sizeof(JSAST *));

  list_T* call_args = ast->args;
  for (uint32_t i = 0; i < call_args->size; i++) {
    //JSAST *farg = (JSAST *)expected_args->items[i];
    JSAST *carg = (JSAST *)call_args->items[i];
    js_eval(carg, stack, execution);

    list_push(stacklist, carg);
    carg->stack_index = (int)stacklist->size;
    list_push(evaluated_args, carg);
    //map_set(stack, farg->value_str, evaluated);
  }

  char* args_str = list_map_str(evaluated_args, js_f_asm32, stack, stacklist, execution, im);

  js_im_execute(im, (JSInstruction){ INST_CALL, 0, value });
  char* str = 0;
  return str;
}

char* js_f_asm32_number(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution, IM* im) {
  char* str = 0;
  ast = js_eval(ast, stack, execution);
  int v = ast->value_int ? ast->value_int : (int)ast->value_num;
  list_push(stacklist, ast);
  int c = (int)stacklist->size;
  ast->stack_index = c;
  js_im_execute(im, (JSInstruction){ INST_SUBL, 4, 0, (JSRegister){ 0, R_32_SP } });
  js_im_execute(im, (JSInstruction){ INST_PUSHL, v, 0 });
  js_im_execute(im, (JSInstruction){ INST_MOVB, 0, 0, (JSRegister){ 0, R_32_SP }, (JSRegister){ 0, R_32_DR_6 } });
  js_im_execute(im, (JSInstruction){ INST_MOVL, v, 0, (JSRegister){ -(c*4), R_32_BP } });
  return str;
}

char* js_f_asm32_string(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution, IM* im) {
  char* str = 0;

  int c = (int)stacklist->size;

  char* value = js_ast_str_value(ast);


  char valuebuff[512];
  sprintf(valuebuff, "# %s\n", value);
  js_str_append(&str, valuebuff);

  list_T* chunks = str_to_hex_chunks(value);
  int bytes_needed = (chunks->size * 4) + 4; // 4 * 4
  js_im_execute(im, (JSInstruction){ INST_SUBL, bytes_needed, 0, (JSRegister){ 0, R_32_SP } });
  char* hexstr = str_to_hex(value);

  int start = -((c*4) + bytes_needed);

  for (size_t i = 0; i < chunks->size; i++) {
    char* hexpart = (char*)chunks->items[i];
     js_im_execute(im, (JSInstruction){ INST_MOVL, 0, hexpart, (JSRegister){ start, R_32_BP } });
    start += 4;
    list_push(stacklist, ast);
  }

  ast->stack_index = c+((bytes_needed+4)/4);
  list_push(stacklist, ast);
  js_im_execute(im, (JSInstruction){ INST_MOVL, 0, 0, (JSRegister){ start, R_32_BP } });

  js_im_execute(im, (JSInstruction){ INST_PUSHL_REG, 0, 0, (JSRegister){ 0, R_32_SP } });

  return str;
}

char* js_f_asm32_definition(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution, IM* im) {
  char* str = 0;

  js_eval(ast, stack, execution);
  JSAST* right = js_eval(ast->right, stack, execution);
  js_f_asm32(right,  stack, stacklist, execution, im);
  list_push(stacklist, ast);
  int c = stacklist->size;


  js_im_execute(im, (JSInstruction){ INST_POPL, 0, 0, (JSRegister){ -(c*4), R_32_BP } });
  js_im_execute(im, (JSInstruction){ INST_SUBL, 4, 0, (JSRegister){ 0, R_32_SP } });

  return str;
}

char* js_f_asm32_ret(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution, IM* im) {
  char* rightstr = ast->right != 0 ? js_f_asm32(ast->right, stack, stacklist, execution, im) : 0;

  JSAST* current_function = (JSAST*)map_get_value(stack, STACK_ADDR_FUNCTION);
  char* func_name = js_ast_str_value(current_function);

  unsigned int should_return = 1;

  if (func_name != 0 && strcmp(func_name, "main") == 0) {
    should_return = 0;
    free(func_name);
  }

  if (should_return) {
    js_im_execute(im, (JSInstruction){ INST_RET, 0, 0 });
  }
  return 0;
}

char* js_f_asm32_id(JSAST* ast, map_T* stack, list_T* stacklist, JSExecution* execution, IM* im) {
  JSAST* id = js_eval(ast, stack, execution);
  js_im_execute(im, (JSInstruction){ INST_PUSHL_REG, 0, 0, (JSRegister){ -(id->stack_index*4), R_32_BP } });
  char* str = 0;
  return str;
}

char* js_f_asm32(JSAST* ast, map_T* stack, list_T* stacklist, struct JS_EXECUTION_STRUCT* execution, IM* im) {
  switch (ast->type) {
    case JS_AST_COMPOUND: return EMIT(compound, ast); break;
    case JS_AST_CALL: return EMIT(call, ast); break;
    case JS_AST_FUNCTION: return EMIT(function, ast); break;
    case JS_AST_ID: return EMIT(id, ast); break;
    case JS_AST_STRING: return EMIT(string, ast); break;
    case JS_AST_NUMBER: return EMIT(number, ast); break;
    case JS_AST_DEFINITION: return EMIT(definition, ast); break;
    case JS_AST_RETURN: return EMIT(ret, ast); break;
  }
  return strdup("");
}
