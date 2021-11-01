#include <js/js_io.h>
#include <js/js_list.h>
#include <js/js_mem.h>
#include <js/js_string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

char *char_to_str(char c) {
  char *s = js_calloc(2, sizeof(char));
  s[0] = c;
  s[1] = '\0';

  return s;
}

void js_str_append(char **s1, char *s2) {
  char *s = *s1;
  size_t len = s ? strlen(s) : 0;
  size_t newlen = len + (s2 ? strlen(s2) : 0);

  s = realloc(s, (newlen + 1) * sizeof(char));
  s[len] = 0;
  strcat(s, s2);

  *s1 = s;
}

char *js_str_join(char *paths[], size_t n, const char *delim) {
  char *str = 0;

  for (size_t i = 0; i < n; i++) {
    char *path = paths[i];
    if (path == 0)
      continue;
    js_str_append(&str, path);

    if (i < n - 1) {
      js_str_append(&str, (char *)delim);
    }
  }

  return str;
}

char *str_to_hex(const char *instr) {
  unsigned int len = strlen(instr);
  char *hexstr = calloc(1, sizeof(char));

  for (unsigned int i = 0; i < len + 1; i++) {
    char *newstr = calloc(4, sizeof(char));
    sprintf(newstr, "%x", instr[(len - i)]);
    hexstr =
        realloc(hexstr, (strlen(hexstr) + strlen(newstr) + 1) * sizeof(char));
    strcat(hexstr, newstr);
    free(newstr);
  }

  return hexstr;
}

char *js_str_apply_escapes(char **str) {
  if (str == 0)
    return 0;
  if (*str == 0)
    return 0;
  char *buff = *str;
  if (buff == 0)
    return 0;
  size_t len = strlen(buff);

  char *newstr = (char *)calloc(len + 1, sizeof(char));

  size_t i = 0;
  while (i < len) {
    char c = buff[i];
    char nc = buff[MIN(len - 1, i + 1)];
    char newc = c;
    if (c == '\\' && nc == 'n') {
      newc = '\n';
      i += 2;
    }

    char b[2];
    b[0] = newc;
    b[1] = '\0';
    strcat(newstr, b);

    i++;
  }

  free(buff);
  *str = newstr;
  return *str;
}
