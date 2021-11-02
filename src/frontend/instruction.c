#include <js/frontend/instruction.h>
#include <js/macros.h>
#include <js/js_string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void js_asm_register_str(JSRegister* reg, char** buffer) {
  char* buff = *buffer;
  if (reg->position) {
    sprintf(buff, "%d(%s)", reg->position, reg->name ? reg->name : "");
  } else {
    sprintf(buff, "%s", reg->name ? reg->name : "");
  }
  *buffer = buff;
}

void js_asm_pushl(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  sprintf(buff, "pushl $%d", instr->value);
  *buffer = buff;
}

void js_asm_pushl_reg(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp);
  sprintf(buff, "pushl %s", tmp);
  *buffer = buff;
  free(tmp);
}
void js_asm_popl(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp);
  sprintf(buff, "popl %s", tmp);
  *buffer = buff;
  free(tmp);
}
void js_asm_movl(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp);
  if (instr->value_str)  {
    sprintf(buff, "movl $0x%s, %s", instr->value_str, tmp);
  } else {
    sprintf(buff, "movl $%d, %s", instr->value, tmp);
  }
  *buffer=  buff;
  free(tmp);
}

void js_asm_movl_reg(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp1 = (char*)calloc(256, sizeof(char));
  char* tmp2 = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp1);
  js_asm_register_str(&instr->reg2, (char**)&tmp2);
  sprintf(buff, "movl %s, %s", tmp1, tmp2);
  *buffer=  buff;
  free(tmp1);
  free(tmp2);
}

void js_asm_movb(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp1 = (char*)calloc(256, sizeof(char));
  char* tmp2 = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp1);
  js_asm_register_str(&instr->reg2, (char**)&tmp2);
  sprintf(buff, "movb (%s), %s", tmp1, tmp2);
  *buffer=  buff;
  free(tmp1);
  free(tmp2);
}
void js_asm_subl(JSInstruction* instr, char** buffer) {
    char* buff = *buffer;
  char* tmp = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp);
  sprintf(buff, "subl $%d, %s", instr->value, tmp);
  *buffer=  buff;
  free(tmp);
}
void js_asm_addl(JSInstruction* instr, char** buffer) {
     char* buff = *buffer;
  char* tmp = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp);
  sprintf(buff, "addl $%d, %s", instr->value, tmp);
  *buffer=  buff;
  free(tmp);
}
void js_asm_call(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  sprintf(buff, "call %s", instr->value_str);
  *buffer=  buff;
}
void js_asm_int(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  sprintf(buff, "int $%d", instr->value);
  *buffer=  buff;
}
void js_asm_jmp(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  sprintf(buff, "jmp %s", instr->value_str);
  *buffer=  buff;
}
void js_asm_ret(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  sprintf(buff, "ret %s", instr->value_str);
  *buffer=  buff;
}
void js_asm_test(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp1 = (char*)calloc(256, sizeof(char));
  char* tmp2 = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp1);
  js_asm_register_str(&instr->reg2, (char**)&tmp2);
  sprintf(buff, "test %s, %s", tmp1, tmp2);
  *buffer=  buff;
  free(tmp1);
  free(tmp2);
}
void js_asm_leal(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp1 = (char*)calloc(256, sizeof(char));
  char* tmp2 = (char*)calloc(256, sizeof(char));
  char* tmp3 = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp1);
  js_asm_register_str(&instr->reg2, (char**)&tmp2);
  js_asm_register_str(&instr->reg3, (char**)&tmp3);
  sprintf(buff, "leal (%s, %s, %d), %s", tmp1, tmp2, instr->value, tmp3);
  *buffer=  buff;
  free(tmp1);
  free(tmp2);
  free(tmp3);
}
void js_asm_inc(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp1 = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp1);
  sprintf(buff, "inc %s", tmp1);
  *buffer=  buff;
  free(tmp1);
}
void js_asm_je(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  sprintf(buff, "je %s", instr->value_str);
  *buffer=  buff;
}
void js_asm_cmpb(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  char* tmp1 = (char*)calloc(256, sizeof(char));
  js_asm_register_str(&instr->reg1, (char**)&tmp1);
  sprintf(buff, "cmpb $%d, %s", instr->value, tmp1);
  *buffer=  buff;
  free(tmp1);
}

void js_asm_function(JSInstruction* instr, char** buffer) {
  char* buff = *buffer;
  sprintf(buff,

"\n.globl %1$s\n"
".type %1$s, @function\n"
          "%1$s:\n"
"pushl %%ebp\n"
"movl %%esp, %%ebp\n\n"
          , instr->value_str);
  *buffer=  buff;
}

void js_asm_instruction_str(JSInstruction* instr, char** buffer) {
  const char* instr_name = 0;

  /*switch (instr->type) {
    case INST_PUSHL: instr_name = "pushl"; break;
    case INST_POPL: instr_name = "popl"; break;
    case INST_SUBL: instr_name = "subl"; break;
    case INST_ADDL: instr_name = "addl"; break;
    case INST_CALL: instr_name = "call"; break;
    case INST_INT: instr_name = "int"; break;
    case INST_MOVL: instr_name = "movl"; break;
    case INST_JMP: instr_name = "jmp"; break;
    case INST_RET: instr_name = "ret"; break;
    case INST_TEST: instr_name = "test"; break;
    case INST_LEAL: instr_name = "leal"; break;
    case INST_INC: instr_name = "inc"; break;
    case INST_JE: instr_name = "je"; break;
    case INST_CMPB: instr_name = "cmpb"; break;
    default: { instr_name = 0; } break;
    }*/

  switch (instr->type) {
    case INST_PUSHL: return js_asm_pushl(instr, buffer); break;
    case INST_PUSHL_REG: return js_asm_pushl_reg(instr, buffer); break;
    case INST_POPL: return js_asm_popl(instr, buffer); break;
    case INST_SUBL: return js_asm_subl(instr, buffer); break;
    case INST_ADDL: return js_asm_addl(instr, buffer); break;
    case INST_CALL: return js_asm_call(instr, buffer); break;
    case INST_INT: return js_asm_int(instr, buffer); break;
    case INST_MOVL: return js_asm_movl(instr, buffer); break;
    case INST_MOVL_REG: return js_asm_movl_reg(instr, buffer); break;
    case INST_MOVB: return js_asm_movb(instr, buffer); break;
    case INST_JMP: return js_asm_jmp(instr, buffer); break;
    case INST_RET: return js_asm_ret(instr, buffer); break;
    case INST_TEST: return js_asm_test(instr, buffer); break;
    case INST_LEAL: return js_asm_leal(instr, buffer); break;
    case INST_INC: return js_asm_inc(instr, buffer); break;
    case INST_JE: return js_asm_je(instr, buffer); break;
    case INST_CMPB: return js_asm_cmpb(instr, buffer); break;
    case INST_FUNCTION: return js_asm_function(instr, buffer); break;
    default: { printf("ASM: Not sure what to do with %d\n", instr->type); } break;
  }
}

JSRegister* init_js_register(JSRegister r) {
  JSRegister* rr = NEW(JSRegister);
  rr->name = strdup(r.name);
  rr->position = r.position;
  return rr;
}

JSInstructionMachine* init_js_instruction_machine() {
  JSInstructionMachine* im = NEW(JSInstructionMachine);
  im->instructions = init_list(sizeof(JSInstruction*));
  return im;
}
JSInstruction* init_js_instruction(JSInstruction instruction) {
  JSInstruction* inst = NEW(JSInstruction);
  inst->reg1 = instruction.reg1;
  inst->reg2 = instruction.reg2;
  inst->reg3 = instruction.reg3;
  inst->type = instruction.type;
  inst->value_str = instruction.value_str ? strdup(instruction.value_str) : 0;
  inst->value = instruction.value;
  return inst;
}

void js_im_execute(JSInstructionMachine* im, JSInstruction instruction) {
  list_push(im->instructions, init_js_instruction(instruction));
}


char* js_im_dump(JSInstructionMachine* im) {
  char* str = 0;

  for (size_t i = 0; i < im->instructions->size; i++) {
    char* buff = (char*)calloc(256, sizeof(char));
    JSInstruction* inst = (JSInstruction*)im->instructions->items[i];
    if (inst == 0) continue;
    js_asm_instruction_str(inst, &buff);
    js_str_append(&str, buff);

    js_str_append(&str, "\n");

    free(buff);
  }

  return str;
}
