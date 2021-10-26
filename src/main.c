#include <js/js.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  JSExecution execution = {1};
  js_execute_file(argv[1], &execution);

  js_execution_free(&execution);

  return 0;
}
