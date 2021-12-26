#ifndef JS_ENGINE_H
#define JS_ENGINE_H
#include <js/stack_pointers.h>
#include <js/js_eval.h>
#include <js/js_frame.h>
#include <js/js_gc.h>
#include <js/js_list.h>
#include <js/js_lexer.h>
#include <js/js_parser.h>

typedef enum {
  JS_EMIT_NONE,
  JS_EMIT_ASM32,
  JS_EMIT_ASM64,
  JS_EMIT_C99,
  JS_EMIT_JS
} JSEmitType;

JSEmitType js_string_to_emit_type(const char *str);

typedef struct JS_EXECUTION_STRUCT {
  unsigned int should_execute;
  unsigned int dry;
  JSGC *gc;
  char *cwd;
  JSLexer *lexer;
  JSParser *parser;
  map_T *frame;
  JSAST *root;
  JSAST *result;
  char *contents;
  char *__filename;
  char *__dirname;
  list_T* lazy;
  list_T* lazy_str;
} JSExecution;

void js_execute_file(const char *filepath, JSExecution *execution);
void js_execute_str(char *str, JSExecution *execution);
void js_execution_free(JSExecution *execution);

JSAST *js_get_module(map_T *frame);
JSAST *js_get_exports(JSAST *modul);

#endif
