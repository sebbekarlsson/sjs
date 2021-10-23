#ifndef JS_ENGINE_TOKEN_H
#define JS_ENGINE_TOKEN_H
#include <TOKEN_TYPES.gpp.h>
typedef struct {
  JSTokenType type;
  char* value;
  char c;
} JSToken;

JSToken* init_js_token(JSTokenType type, char* value);
#endif
