#ifndef JS_ENGINE_FRONTEND_H
#define JS_ENGINE_FRONTEND_H
#include <js/js.h>
#include <js/js_path.h>
#include <js/js_io.h>
#include <js/frontend/asm32/asm32.h>
#include <stdio.h>
#include <stdlib.h>

static int emit_asm32(char* input_file, JSExecution* execution) {
  char* basename = path_basename(input_file);
  if (execution->root == 0) {
    fprintf(stderr, "Nothing to emit.\n");
    return 1;
  }
  char* buffer = js_f_asm32_entry(execution->root);

  if (buffer == 0) return 0;

  char buff[PATH_MAX];
  sprintf(buff, "%s.s", basename);
  js_write_file(buff, buffer);
  printf("wrote to %s\n", buff);
  free(buffer);
  return 0;
}

static int emit(char* input_file, int argc, char* argv[], JSExecution* execution) {
  if (argc < 2) return 1;
  JSEmitType emit_type = js_string_to_emit_type(argv[0]);
  switch (emit_type) {
    case JS_EMIT_ASM32: return emit_asm32(input_file, execution); break;
    default: {
      printf("Unknown emit type %d\n", emit_type);
      return 1;
    }
  }
}
#endif
