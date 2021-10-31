#include <js/frontend/asm32/asm32.h>
#include <js/js_string.h>
#include <bootstrap.s.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


char* js_f_asm32_entry(JSAST* ast) {
  char* str = (char*)strdup(bootstrap_s);
  char* nextstr = js_f_asm32(ast);

  if (nextstr != 0) {
    js_str_append(&str, nextstr);
    free(nextstr);
  }
  return str;
}

char* js_f_asm32(JSAST* ast) {
  return 0;
}
