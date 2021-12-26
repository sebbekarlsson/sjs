#include <js/args.h>
#include <js/frontend/asm32/asm32.h>
#include <js/frontend/frontend.h>
#include <js/js.h>
#include <js/js_path.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

static int interactive() {
  char buff[1028];

  JSExecution execution = {1};
  while (1) {
    memset(buff, 0, 1028 * sizeof(char));
    printf("(SJS)$ ");
    fgets(buff, 1028, stdin);
    js_execute_str(buff, &execution);
    js_lexer_free(execution.lexer);
    js_parser_free(execution.parser);
  }
  js_execution_free(&execution);
  return 0;
}

int main(int argc, char *argv[]) {

  // char* s = strdup("hello   world   \n\t   \t esadf1");

  //  js_str_trim_last(&s);

  // printf("%s\n", s);

  // return 0;

  if (argc == 1)
    return interactive();

  int pos = 0;
  char **emit_arg = find_arg(argc, argv, &pos, "-e");

  if (emit_arg != 0)
    return emit(argv[1], argc - pos, emit_arg);
  JSExecution execution = {emit_arg == 0 ? 1 : 0};
  js_execute_file(argv[1], &execution);

  js_execution_free(&execution);

  return 0;
}
