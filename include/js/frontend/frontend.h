#ifndef JS_ENGINE_FRONTEND_H
#define JS_ENGINE_FRONTEND_H
#include <js/js.h>
#include <js/js_path.h>
#include <js/args.h>
#include <js/js_io.h>
#include <js/frontend/asm32/asm32.h>
#include <js/frontend/js/js.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

static int emit_asm32(char* input_file, char* output_arg) {
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

static int emit_js(char* input_file, char* output_arg) {
  char* basename = path_basename(input_file);
  char* buffer = js_f_js_entry(input_file);

  if (buffer == 0) return 0;

  char* filename = output_arg;

  if (!filename) {
     char buff[PATH_MAX*2];
     sprintf(buff, "%s.out.js", basename);
     filename = buff;
  }


  js_write_file(filename, buffer);
  printf("wrote to %s\n", filename);


  free(buffer);

  return 0;
}

static int emit(char* input_file, int argc, char* argv[]) {
  if (argc < 2) return 1;

  char* output_arg = get_arg(argc, argv, "-o");
  char* emit_arg = get_arg(argc, argv, "-e");
  JSEmitType emit_type = js_string_to_emit_type(emit_arg);
  switch (emit_type) {
    case JS_EMIT_ASM32: return emit_asm32(input_file, output_arg); break;
    case JS_EMIT_JS: return emit_js(input_file, output_arg); break;
    default: {
      printf("Unknown emit type %s:%d\n", emit_arg, emit_type);
      return 1;
    }
  }
}
#endif
