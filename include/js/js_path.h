#ifndef JS_ENGINE_PATH_H
#define JS_ENGINE_PATH_H
#include <js/js_list.h>
#include <js/js_string.h>

list_T *path_split(char *path);
char *path_dirname(char *path);
char *path_join(list_T *paths);
char *path_resolve(char *filepath, char *dirname);
#endif
