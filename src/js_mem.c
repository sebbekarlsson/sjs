#include <js/js_mem.h>
#include <stdio.h>
#include <stdlib.h>

void *js_calloc(uint32_t len, uint32_t siz) {
  void *data = calloc(len, siz);
  if (data == 0) {
    printf("Unable to allocate %d * %d\n", len, siz);
    exit(1);
  }

  return data;
}
