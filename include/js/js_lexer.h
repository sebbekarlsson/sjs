#ifndef JS_ENGINE_LEXER_H
#define JS_ENGINE_LEXER_H
#include <js/js_token.h>
#include <stdint.h>
typedef struct JS_LEXER_STRUCT {
  char *src;
  uint64_t i;
  uint64_t len;
  char c;
  char cstr[2];
} JSLexer;

#define JS_LEXER_DONE(lexer) (lexer->c == '\0' || lexer->i >= lexer->len)

JSLexer *init_js_lexer(char *src);

void js_lexer_free(JSLexer *lexer);

void js_lexer_advance(JSLexer *lexer);
JSToken *js_lexer_get_next_token(JSLexer *lexer);
void js_lexer_skip_whitespace(JSLexer *lexer);
JSToken *js_lexer_parse_str(JSLexer *lexer);
JSToken *js_lexer_parse_num(JSLexer *lexer);
JSToken *js_lexer_parse_id(JSLexer *lexer);
JSToken *js_lexer_tok(JSLexer *lexer, JSTokenType type);
JSToken *js_lexer_tok_n(JSLexer *lexer, JSTokenType type, uint32_t n);

char js_lexer_peek(JSLexer *lexer, uint32_t i);

#endif
