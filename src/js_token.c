#include <js/js_token.h>
#include <js/macros.h>
#include <stdio.h>
#include <string.h>

JSToken *init_js_token(JSTokenType type, char *value) {
  JSToken *tok = NEW(JSToken);
  tok->type = type;
  tok->value = strdup(value ? value : "");
  tok->c = 0;

  return tok;
}
void js_token_free(JSToken *token) {
  if (token == 0)
    return;
  if (token->value != 0) {
    free(token->value);
    token->value = 0;
  }

  free(token);
}

char *js_token_to_str(JSToken *token) {
  const char *typestr = JS_TOKEN_TYPE_STR[token->type];
  const char *template = "<token type={%s} value={%s}/>";
  char *str = js_calloc(strlen(typestr) + strlen(template) + 16, sizeof(char));
  sprintf(str, template, typestr, token->value);

  return str;
}

const char *js_token_type_to_str(JSTokenType type) {
  return JS_TOKEN_TYPE_STR[type];
}
