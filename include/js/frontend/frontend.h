#ifndef JS_ENGINE_FRONTEND_H
#define JS_ENGINE_FRONTEND_H
#include <js/js.h>
#include <js/js_path.h>
#include <js/js_io.h>
#include <js/frontend/asm32/asm32.h>
#include <stdio.h>
#include <stdlib.h>

static int emit_asm32(char* input_file) {
  char* basename = path_basename(input_file);
  char* buffer = js_f_asm32_entry(input_file);

  if (buffer == 0) return 0;

  char buff[PATH_MAX*2];
  sprintf(buff, "%s.s", basename);
  js_write_file(buff, buffer);
  printf("wrote to %s\n", buff);


  char as_cmd[PATH_MAX*2];
  sprintf(as_cmd, "as --32 %s -o %s.o", buff, basename);
  printf("%s\n", as_cmd);
  js_sh(as_cmd);

  char ld_cmd[PATH_MAX*2];
  sprintf(ld_cmd, "ld %s.o -o %s.out -m elf_i386", basename, basename);
  printf("%s\n", ld_cmd);
  js_sh(ld_cmd);
  free(buffer);
  return 0;
}

static int emit(char* input_file, int argc, char* argv[]) {
  if (argc < 2) return 1;
  JSEmitType emit_type = js_string_to_emit_type(argv[0]);
  switch (emit_type) {
    case JS_EMIT_ASM32: return emit_asm32(input_file); break;
    default: {
      printf("Unknown emit type %d\n", emit_type);
      return 1;
    }
  }
}
#endif
