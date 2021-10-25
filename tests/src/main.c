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

static JSAST *ex(const char *filename, const char *expected_stdout) {
  char *contents = get_file_contents(filename);

  JSLexer *lexer = init_js_lexer(contents);
  JSParser *parser = init_js_parser(lexer);

  JSAST *root = js_parser_parse(parser);
  map_T *frame = setup_js_frame();

  capture_stdout();
  JSAST *result = js_eval(root, frame);
  restore_stdout();
  ASSERT(strcmp(stdoutbuff, expected_stdout) == 0);

  return result;
}

void test_hello_world_js() {
  JSAST *root = ex("sourcefiles/hello-world.js", "hello world\n");

  ASSERT(root != 0);
  ASSERT(root->type == JS_AST_COMPOUND);
  ASSERT(root->children->size == 2);
  JSAST *first = (JSAST *)root->children->items[0];
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
}

int main(int argc, char *argv[]) {
  test_hello_world_js();

  return 0;
}
