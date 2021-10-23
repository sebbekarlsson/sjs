#include <ctype.h>
#include <js/js_lexer.h>
#include <js/js_string.h>
#include <js/macros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

JSLexer *init_js_lexer(char *src) {
  JSLexer *jslexer = NEW(JSLexer);
  jslexer->src = strdup(src);
  jslexer->i = 0;
  jslexer->c = jslexer->src[jslexer->i];
  jslexer->len = strlen(jslexer->src);
  jslexer->cstr[0] = jslexer->c;
  jslexer->cstr[1] = '\0';

  return jslexer;
}

void js_lexer_advance(JSLexer *lexer) {
  if (JS_LEXER_DONE(lexer) == 0) {
    lexer->i++;
    lexer->c = lexer->src[lexer->i];

    lexer->cstr[0] = lexer->c;
    lexer->cstr[1] = '\0';
  }
}

JSToken *js_lexer_get_next_token(JSLexer *lexer) {
  while (JS_LEXER_DONE(lexer) == 0) {
    js_lexer_skip_whitespace(lexer);

    if (isalpha(lexer->c) || lexer->c == '_')
      return js_lexer_parse_id(lexer);

    if (isdigit(lexer->c)) return js_lexer_parse_num(lexer);

    if (lexer->c == '\'' || lexer->c == '"') return js_lexer_parse_str(lexer);

    switch (lexer->c) {
      case '{': return js_lexer_tok(lexer, TOKEN_LBRACE);
      case '}': return js_lexer_tok(lexer, TOKEN_RBRACE);
      case '(': return js_lexer_tok(lexer, TOKEN_LPAREN);
      case ')': return js_lexer_tok(lexer, TOKEN_RPAREN);
      case '[': return js_lexer_tok(lexer, TOKEN_LBRACKET);
      case ']': return js_lexer_tok(lexer, TOKEN_RBRACKET);
      case ';': return js_lexer_tok(lexer, TOKEN_SEMI);
      case '=': return js_lexer_tok(lexer, TOKEN_EQUALS);
      case '<': return js_lexer_tok(lexer, TOKEN_LT);
      case '>': return js_lexer_tok(lexer, TOKEN_GT);
      case '.': return js_lexer_tok(lexer, TOKEN_DOT);
      case ',': return js_lexer_tok(lexer, TOKEN_COMMA);
      case ':': return js_lexer_tok(lexer, TOKEN_COLON);
      case '!': return js_lexer_tok(lexer, TOKEN_NOT);
      case '&': return js_lexer_tok(lexer, TOKEN_AND);
      case '+': return js_lexer_tok(lexer, TOKEN_PLUS);
      case '-': return js_lexer_tok(lexer, TOKEN_MINUS);
      case '/': return js_lexer_tok(lexer, TOKEN_DIV);
      case '%': return js_lexer_tok(lexer, TOKEN_MOD);
      case '|': return js_lexer_tok(lexer, TOKEN_PIPE);
      case '*': return js_lexer_tok(lexer, TOKEN_STAR);
      case '?': return js_lexer_tok(lexer, TOKEN_QUESTION);
      case '\\': return js_lexer_tok(lexer, TOKEN_ESCAPE);
      case '#': return js_lexer_tok(lexer, TOKEN_HASH);
      case '^': return js_lexer_tok(lexer, TOKEN_SQUARED);
      case '~': return js_lexer_tok(lexer, TOKEN_TILDE);
      case '\0': break;
    }
  }

  return init_js_token(TOKEN_EOF, 0);
}

void js_lexer_skip_whitespace(JSLexer *lexer) {
  while (JS_LEXER_DONE(lexer) == 0 && (lexer->c == ' ' || lexer->c == '\n' || lexer->c == '\r' || lexer->c == 13 || lexer->c == '\t')) {
    js_lexer_advance(lexer);
  }
}

JSToken *js_lexer_parse_str(JSLexer *lexer) {
  char* str = 0;
  char start = lexer->c;

  js_lexer_advance(lexer);

  while (lexer->c != 0 && lexer->c != start) {
    if (lexer->c == '\\') {
      js_str_append(&str, "\\");
      js_lexer_advance(lexer);
      js_str_append(&str, lexer->cstr);
      js_lexer_advance(lexer);
      continue;
    }
    if (lexer->c == start) {
      break;
    } else {
      if (lexer->c == '"' && start == '\'') {
        js_str_append(&str, "\\");
      }
      js_str_append(&str, lexer->cstr);
      js_lexer_advance(lexer);
    }
  }

  if (lexer->c == start)
    js_lexer_advance(lexer);

  JSToken* token = init_js_token(TOKEN_STRING, str ? str : strdup(""));

  if (str) { free(str); }

  token->c = start;

  return token;
}

JSToken *js_lexer_parse_num(JSLexer *lexer) {
  char* s = 0;
  JSTokenType type = TOKEN_INT;

  while (isdigit(lexer->c)) {
    js_str_append(&s, lexer->cstr);
    js_lexer_advance(lexer);
  }

  if (lexer->c == '.') {
    type = TOKEN_FLOAT;
    js_str_append(&s, lexer->cstr);
    js_lexer_advance(lexer);
  }

  while (isdigit(lexer->c)) {
    js_str_append(&s, lexer->cstr);
    js_lexer_advance(lexer);
  }

  JSToken* tok = init_js_token(type, s);

  if (s) { free(s); }

  return tok;
}

JSToken *js_lexer_parse_id(JSLexer *lexer) {
    char* s = 0;

  while (isalpha(lexer->c) || lexer->c == '_') {
    js_str_append(&s, lexer->cstr);
    js_lexer_advance(lexer);
  }

  JSToken* tok = init_js_token(TOKEN_ID, s);

  if (s) { free(s); }

  return tok;
}

JSToken *js_lexer_tok(JSLexer *lexer, JSTokenType type) {
  char *value = char_to_str(lexer->c);
  JSToken *tok = init_js_token(type, value);
  free(value);
  js_lexer_advance(lexer);

  return tok;
}
