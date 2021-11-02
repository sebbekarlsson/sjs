#ifndef JS_ENGINE_FRONTEND_ASM32_H
#define JS_ENGINE_FRONTEND_ASM32_H
#include <js/js_AST.h>
#include <js/frontend/instruction.h>
char* js_f_asm32_entry(const char* filename);

struct JS_EXECUTION_STRUCT;

typedef JSInstructionMachine IM;

char* js_f_asm32(JSAST* ast, map_T* stack, list_T* stacklist, struct JS_EXECUTION_STRUCT* execution, IM* im);
#endif
