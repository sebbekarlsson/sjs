#include <ctype.h>
#include <js/js_io.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char *get_file_contents(const char *filepath) {
  FILE *fp = fopen(filepath, "r");

  if (fp == 0) {
    printf("Could not open file `%s`\n", filepath);
    return 0;
  }

  char *buffer = NULL;
  size_t len;
  ssize_t bytes_read = getdelim(&buffer, &len, '\0', fp);
  if (bytes_read != -1) {
    fclose(fp);
    return buffer;
  } else {
    fclose(fp);
    printf("Read %ld byes from `%s`\n", bytes_read, filepath);
    return 0;
  }
}

char *get_working_directory() {
  char *cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    return strdup(cwd);
  } else {
    perror("getcwd() error");
    return 0;
  }

  return 0;
}

unsigned int js_file_exists(const char *filepath) {
  return access(filepath, F_OK) == 0;
}
unsigned int js_is_directory(const char *filepath) {
  struct stat statbuf;
  if (stat(filepath, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

unsigned int js_is_regular_file(const char *filepath) {
  if (js_file_exists(filepath) == 0)
    return 0;
  if (js_is_directory(filepath))
    return 0;
  return 1;
}

void js_write_file(const char *filepath, char *buffer) {
  if (buffer == 0)
    buffer = (char *)"";
  FILE *fp = fopen(filepath, "w+");
  if (fp == 0) {
    fprintf(stderr, "Could not write to `%s`", filepath);
    return;
  }

  uint32_t len = strlen(buffer);

  fwrite(&buffer[0], MAX(0, len), sizeof(char), fp);

  fclose(fp);
}
