#ifndef JS_ENGINE_TOKEN_H
#define JS_ENGINE_TOKEN_H
#include <TOKEN_TYPES.gpp.h>
typedef struct {
  JSTokenType type;
  char *value;
  char c;
} JSToken;

JSToken *init_js_token(JSTokenType type, char *value);

char *js_token_to_str(JSToken *token);

const char *js_token_type_to_str(JSTokenType type);
#endif
