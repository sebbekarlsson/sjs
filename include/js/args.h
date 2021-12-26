#ifndef JS_ENGINE_ARGS_H
#define JS_ENGINE_ARGS_H
#include <string.h>
#include <sys/param.h>
static char **find_arg(int argc, char *argv[], int *pos, const char *name) {
  for (int i = 0; i < argc; i++) {
    char *arg = argv[i];
    if (strcmp(arg, name) == 0) {
      *pos = i;
      return &argv[MIN(i + 1, MAX(0, argc - 1))];
    }
  }
  return 0;
}

static char *get_arg(int argc, char *argv[], const char *name) {
    for (int i = 0; i < argc; i++) {
    char *arg = argv[i];
    if (strcmp(arg, name) == 0) {
      return argv[i+1];
    }
    }

    return 0;
}
#endif
