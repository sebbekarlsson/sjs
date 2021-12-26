#ifndef JS_ENGINE_STR_H
#define JS_ENGINE_STR_H
#include <string.h>
char *char_to_str(char c);
void js_str_append(char **str, char *value);
char *js_str_join(char *paths[], size_t n, const char *delim);

void js_str_trim_last(char **str);
#endif
