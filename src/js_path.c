#include <js/js_io.h>
#include <js/js_path.h>
#include <js/js_string.h>
#include <stdio.h>
#include <string.h>

static void free_str(void *str) { free((char *)str); }

list_T *path_split(char *path) {
  list_T *newlist = init_list(sizeof(char **));
  if (path == 0)
    return newlist;

  char *path_copy = strdup(path);

  if (path_copy != 0) {
    char *tok = strtok(path_copy, "/");

    while (tok != 0) {
      list_push(newlist, strdup(tok));
      tok = strtok(0, "/");
    }

    free(path_copy);
  }

  return newlist;
}
char *path_dirname(char *path) {
  list_T *parts = path_split(path);
  char *last = list_pop(parts);
  if (last)
    free(last);

  char *fullpath = list_join(parts, "/");

  list_free_full(parts, free_str);

  return fullpath;
}

char *path_join(list_T *paths) {
  if (paths == 0)
    return 0;
  return list_join(paths, "/");
}

char *path_resolve(char *filepath, char *dirname) {
  if (js_file_exists(filepath))
    return strdup(filepath);
  char *path1 = js_str_join((char *[2]){dirname, filepath}, 2, "/");

  if (js_file_exists(path1))
    return path1;

  const char *extensions[] = {".js"};
  unsigned int nr_extensions = 1;

  char *path_to_use = path1;

  for (unsigned int i = 0; i < nr_extensions; i++) {
    char *dup = strdup(path1);
    const char *ext = extensions[i];
    js_str_append(&dup, (char *)ext);

    if (js_file_exists(dup)) {
      if (path_to_use != 0) {
        free(path_to_use);
        path_to_use = 0;
      }
      path_to_use = dup;
      break;
    }

    free(dup);
  }

  return path_to_use;
}
