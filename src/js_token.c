#include <js/js_token.h>
#include <js/macros.h>
#include <string.h>


JSToken* init_js_token(JSTokenType type, char* value) {
  JSToken* tok = NEW(JSToken);
  tok->type = type;
  tok->value = strdup(value ? value : "");
  tok->c = 0;

  return tok;
}
