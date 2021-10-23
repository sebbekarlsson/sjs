#include <js/js_string.h>
#include <js/js_mem.h>
#include <stdlib.h>


char* char_to_str(char c) {
  char* s = js_calloc(2, sizeof(char));
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
