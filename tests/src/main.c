
#include <js/js.h>
#include <assert.h>
#include <js/js_eval.h>
#include <js/js_frame.h>
#include <js/js_io.h>
#include <js/js_lexer.h>
#include <js/js_parser.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
int stdoutSave;
char stdoutbuff[BUFFER_SIZE];

void capture_stdout() {
  memset(stdoutbuff, 0, BUFFER_SIZE * sizeof(char));
  fflush(stdout);                            // clean everything first
  stdoutSave = dup(STDOUT_FILENO);           // save the stdout state
  freopen("NUL", "a", stdout);               // redirect stdout to null pointer
  setvbuf(stdout, stdoutbuff, _IOFBF, 1024); // set buffer to stdout
}

void restore_stdout() {
  freopen("NUL", "a", stdout);     // redirect stdout to null again
  dup2(stdoutSave, STDOUT_FILENO); // restore the previous state of stdout
  setvbuf(stdout, NULL, _IONBF,
          BUFFER_SIZE); // disable buffer to print to screen instantly
}

void printHelloWorld() {
  printf("hello\n");
  printf("world");
}

#define ASSERT(expr)                                                           \
  {                                                                            \
    printf("* %s\n", __func__);                                                \
    printf("ASSERT(%s)\n", #expr);                                             \
    if (!(expr)) {                                                             \
      printf("FAIL\n");                                                        \
    } else {                                                                   \
      printf("OK\n");                                                          \
    }                                                                          \
    assert(expr);                                                              \
  }

#define EX(name, filename, expected_stdout, should_eval)  \
  JSExecution execution = (JSExecution){ should_eval };\
  capture_stdout();\
  js_execute_file(filename, &execution);\
  restore_stdout();                                \
  printf("STDOUT(%s)\n", stdoutbuff);\
  ASSERT(strcmp(stdoutbuff, expected_stdout) == 0);

void test_hello_world_js() {
  EX(root, "sourcefiles/hello-world.js", "hello world\n", 1);
  JSAST* root = execution.result;

  ASSERT(root != 0);
  JSIterator iterator = js_ast_iterate(root);
  JSAST* first = iterator.current;
  ASSERT(first != 0);
  ASSERT(first->type == JS_AST_BINOP);
  ASSERT(first->left != 0);
  ASSERT(first->right != 0);
  ASSERT(first->left->type == JS_AST_ID);
  ASSERT(first->right->type == JS_AST_CALL);
  JSAST *call = first->right;
  ASSERT(call->args->size == 1);
  JSAST *arg1 = (JSAST *)call->args->items[0];
  ASSERT(arg1 != 0);
  ASSERT(arg1->type == JS_AST_STRING);
  ASSERT(arg1->value_str != 0);
  ASSERT(strcmp(arg1->value_str, "hello world") == 0);

  js_ast_free(root);
}

void test_foreach_js() {
  EX(root, "sourcefiles/foreach.js", "david\njohn\nsarah\nhannah\n", 1);
  JSAST* root = execution.result;
  JSIterator iterator = js_ast_iterate(root);
  JSAST* it = iterator.current;
  ASSERT(root != 0);
  ASSERT(it != 0);
  ASSERT(it->type == JS_AST_DEFINITION);

  js_iterator_next(&iterator);
  it = iterator.current;

  ASSERT(it != 0);
  ASSERT(it->type == JS_AST_BINOP);
  ASSERT(it->left != 0);
  ASSERT(it->left->type == JS_AST_ID);
  ASSERT(it->right != 0);
  ASSERT(it->right->type == JS_AST_CALL);
  ASSERT(it->right->args->size == 1);
  ASSERT(((JSAST*)it->right->args->items[0])->type == JS_AST_FUNCTION);

  js_ast_free(root);
}

void test_func_js() {
  EX(root, "sourcefiles/func.js", "26.50\n", 1);
  JSAST* root = execution.result;
  JSIterator iterator = js_ast_iterate(root);
  JSAST* it = iterator.current;

  ASSERT(it != 0);

  js_ast_free(root);
}

void test_map_js() {
  EX(root, "sourcefiles/map.js", "8.00\n", 1);
  JSAST* root = execution.result;
  JSIterator iterator = js_ast_iterate(root);
  JSAST* it = iterator.current;

  ASSERT(it != 0);

  js_ast_free(root);
}

void test_string_length_js() {
  EX(root, "sourcefiles/string_length.js", "11.00\n", 1);
  JSAST* root = execution.result;
  JSIterator iterator = js_ast_iterate(root);
  JSAST* it = iterator.current;

  ASSERT(it != 0);

  js_ast_free(root);
}

void test_if_js() {
  EX(root, "sourcefiles/if.js", "yes\ngood\n", 1);
  JSAST* root = execution.result;
  JSIterator iterator = js_ast_iterate(root);
  JSAST* it = iterator.current;

  ASSERT(it != 0);

  js_ast_free(root);
}

void test_projects_calculator_index_js() {
  EX(root, "sourcefiles/projects/calculator/index.js", "10.00\n5.00\n", 1);
  JSAST* root = execution.result;
  ASSERT(root != 0);
}

int main(int argc, char *argv[]) {
  test_hello_world_js();
  test_func_js();
  test_foreach_js();
  test_map_js();
  test_string_length_js();
  test_if_js();
  test_projects_calculator_index_js();

  return 0;
}
