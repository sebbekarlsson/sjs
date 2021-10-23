#include <js/js_eval.h>
#include <js/js_frame.h>
#include <js/js_io.h>
#include <js/js_lexer.h>
#include <js/js_parser.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  char *contents = get_file_contents(argv[1]);

  JSLexer *lexer = init_js_lexer(contents);
  JSParser *parser = init_js_parser(lexer);

  JSAST *root = js_parser_parse(parser);
  map_T *frame = setup_js_frame();

  JSAST *result = js_eval(root, frame);

  return 0;
}
