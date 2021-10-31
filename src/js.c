#include <assert.h>
#include <js/js.h>
#include <js/js_frame.h>
#include <js/js_io.h>
#include <js/js_lexer.h>
#include <js/js_parser.h>
#include <js/js_path.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void js_execute_file(const char *filepath, JSExecution *execution) {
  char *contents = get_file_contents(filepath);
  if (contents == 0)
    return;
  JSExecution ex = *execution;
  ex.cwd = get_working_directory();
  ex.__filename = strdup(filepath);
  ex.__dirname = path_dirname(filepath);
  ex.contents = contents;

  if (ex.gc == 0) {
    ex.gc = init_js_gc();
  }

  *execution = ex;
  return js_execute_str(contents, execution);
}
void js_execute_str(char *str, JSExecution *execution) {
  JSExecution ex = *execution;

  if (ex.gc == 0) {
    ex.gc = init_js_gc();
    *execution = ex;
  }

  JSLexer *lexer = init_js_lexer(str);
  JSParser *parser = init_js_parser(lexer, execution);
  JSAST *root = js_parser_parse(parser);
  js_gc_ast(execution->gc, root);

  map_T *frame = ex.frame;
  if (frame == 0) {
    frame = setup_js_frame(execution);
    ex.frame = frame;
  }
  ex.lexer = lexer;
  ex.root = root;
  ex.parser = parser;
  JSAST *result = ex.should_execute ? js_eval(root, frame, execution) : 0;
  js_gc_ast(execution->gc, result);

  ex.result = result;

  *execution = ex;
}
void js_execution_free(JSExecution *execution) {
  if (execution == 0)
    return;
  if (execution->gc == 0) {
    printf("ERROR (%s): Garbage collector is NULL.\n", __func__);
    exit(1);
  }
  if (execution->lexer != 0) {
    js_lexer_free(execution->lexer);
    execution->lexer = 0;
  }

  if (execution->parser != 0) {
    js_parser_free(execution->parser);
    execution->parser = 0;
  }

  if (execution->frame != 0) {
    js_frame_free(execution->frame, execution);
    execution->frame = 0;
  }

  if (execution->root != 0) {
    js_gc_ast(execution->gc, execution->root);
  }

  if (execution->result != 0) {
    if (execution->result->marked == 0) {
      js_ast_free(execution->result);
      execution->result = 0;
    }
  }

  if (execution->contents != 0) {
    free(execution->contents);
    execution->contents = 0;
  }

  if (execution->__dirname != 0) {
    free(execution->__dirname);
    execution->__dirname = 0;
  }

  if (execution->__filename != 0) {
    free(execution->__filename);
    execution->__filename = 0;
  }

  if (execution->cwd != 0) {
    free(execution->cwd);
    execution->cwd = 0;
  }

  if (execution->gc != 0) {
    js_gc_free(execution->gc);
  }
}

JSAST *js_get_module(map_T *frame) {
  assert(frame != 0);
  JSAST *module = (JSAST *)map_get_value(frame, "module");
  return module;
}
JSAST *js_get_exports(JSAST *modul) {
  assert(modul != 0);
  JSAST *exports = (JSAST *)map_get_value(modul->keyvalue, "exports");
  return exports;
}

JSEmitType js_string_to_emit_type(const char *str) {
  if (strcmp(str, "asm32") == 0)
    return JS_EMIT_ASM32;
  if (strcmp(str, "asm64") == 0)
    return JS_EMIT_ASM64;
  if (strcmp(str, "C99") == 0)
    return JS_EMIT_C99;
  if (strcmp(str, "js") == 0)
    return JS_EMIT_JS;
  if (strcmp(str, "none") == 0)
    return JS_EMIT_NONE;
  fprintf(stderr, "Unknown emit type %s\n", str);
  return JS_EMIT_NONE;
}
