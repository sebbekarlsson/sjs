#include <js/js_io.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


char* get_file_contents(const char* filepath) {
  FILE* fp = fopen(filepath, "r");

  if (fp == 0) {
    printf("Could not open file `%s`\n", filepath);
    return 0;
  }

  char* buffer = NULL;
  size_t len;
  ssize_t bytes_read = getdelim( &buffer, &len, '\0', fp);
  if (bytes_read != -1) {
    return buffer;
  } else {
    printf("Read %ld byes from `%s`\n", bytes_read, filepath);
    return 0;
  }
}
