#include <js/js.h>
#include <stdint.h>
#include <stdio.h>

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

  if (argc == 1)
    return interactive();

  JSExecution execution = {1};
  js_execute_file(argv[1], &execution);

  js_execution_free(&execution);

  return 0;
}
