#include <js/js_lexer.h>
#include <js/js_io.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  char* contents = get_file_contents(argv[1]);

  JSLexer* lexer = init_js_lexer(contents);

  JSToken* tok = js_lexer_get_next_token(lexer);

  printf("looping...\n");
  while (tok && tok->type != TOKEN_EOF) {
    printf("%s\n", tok->value);
    tok = js_lexer_get_next_token(lexer);
  }

  return 0;
}
