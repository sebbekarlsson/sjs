#ifndef JS_ENGINE_INSTRUCTION_H
#define JS_ENGINE_INSTRUCTION_H
#include <js/js_list.h>
typedef enum {
INST_PUSHL,
INST_PUSHL_REG,
INST_POPL,
INST_SUBL,
INST_ADDL,
INST_CALL,
INST_INT,
INST_MOVL,
INST_MOVL_REG,
INST_MOVB,
INST_JMP,
INST_RET,
INST_DIV,
INST_TEST,
INST_LEAL,
INST_INC,
INST_JE,
INST_CMPB,
INST_FUNCTION
} InstructionType;

#define R_32_BP "%ebp" // base pointer
#define R_32_SP "%esp" // stack pointer
#define R_32_IP "%edi" // index pointer
#define R_32_IP "%edi" // index pointer

#define R_32_G1 "%eax" // general register
#define R_32_G2 "%ebx" // general register

// data registers
#define R_32_DR_1 "%ah"
#define R_32_DR_2 "%al"
#define R_32_DR_3 "%bh"
#define R_32_DR_4 "%bl"
#define R_32_DR_5 "%ch"
#define R_32_DR_6 "%cl"
#define R_32_DR_7 "%dh"
#define R_32_DR_8 "%dl"

typedef struct JS_REGISTER_STRUCT {
  int position;
  char* name;
} JSRegister;


void js_asm_register_str(JSRegister* reg, char** buffer);

typedef struct JS_INSTRUCTION_STRUCT {
  InstructionType type;
  int value;
  const char* value_str;
  JSRegister reg1;
  JSRegister reg2;
  JSRegister reg3;
} JSInstruction;


typedef struct JS_INSTRUCTION_MACHINE_STRUCT {
  list_T* instructions;
} JSInstructionMachine;

JSInstructionMachine* init_js_instruction_machine();
JSInstruction* init_js_instruction(JSInstruction instruction);

JSRegister* init_js_register(JSRegister r);

void js_im_execute(JSInstructionMachine* im, JSInstruction instruction);

char* js_im_dump(JSInstructionMachine* im);

void js_asm_instruction_str(JSInstruction* instr, char** buffer);

#endif
