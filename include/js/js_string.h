#ifndef JS_ENGINE_STR_H
#define JS_ENGINE_STR_H
#include <string.h>
char *char_to_str(char c);
void js_str_append(char **str, char *value);
char *js_str_join(char *paths[], size_t n, const char *delim);
char *str_to_hex(const char *instr);
char *js_str_apply_escapes(char **str);
#endif
