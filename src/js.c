#include <js/js.h>
#include <js/js_frame.h>
#include <js/js_io.h>
#include <js/js_lexer.h>
#include <js/js_parser.h>
#include <js/js_path.h>

void js_execute_file(const char *filepath, JSExecution *execution) {
  char *contents = get_file_contents(filepath);
  JSExecution ex = *execution;
  ex.cwd = get_working_directory();
  ex.__filename = strdup(filepath);
  ex.__dirname = path_dirname(filepath);

  *execution = ex;
  return js_execute_str(contents, execution);
}
void js_execute_str(char *str, JSExecution *execution) {
  JSExecution ex = *execution;
  JSLexer *lexer = init_js_lexer(str);
  JSParser *parser = init_js_parser(lexer);
  JSAST *root = js_parser_parse(parser);
  map_T *frame = setup_js_frame(execution);
  JSAST *result = ex.should_execute ? js_eval(root, frame) : 0;

  ex.lexer = lexer;
  ex.parser = parser;
  ex.root = root;
  ex.frame = frame;
  ex.result = result;

  *execution = ex;
}
void js_execution_free(JSExecution *execution) {
  if (execution == 0)
    return;
  if (execution->lexer != 0) {
    js_lexer_free(execution->lexer);
    execution->lexer = 0;
  }

  if (execution->parser != 0) {
    js_parser_free(execution->parser);
    execution->parser = 0;
  }

  if (execution->frame != 0) {
    js_frame_free(execution->frame);
    execution->frame = 0;
  }

  if (execution->result != 0) {
    js_ast_free(execution->result);
    execution->result = 0;
  }

  // TODO: implement js_ast_copy and have js_eval always return a copy.
  // Need to do this for this to work.
  // if (execution->root != 0) {
  //   js_ast_free(execution->root);
  //   execution->root = 0;
  // }

  if (execution->contents != 0) {
    free(execution->contents);
    execution->contents = 0;
  }
}

JSAST *js_get_module(map_T *frame) {
  JSAST *module = (JSAST *)map_get_value(frame, "module");
  return module;
}
JSAST *js_get_exports(JSAST *modul) {
  JSAST *exports = (JSAST *)map_get_value(modul->keyvalue, "exports");
  return exports;
}
