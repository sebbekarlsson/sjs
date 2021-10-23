#include <ctype.h>
#include <js/js_lexer.h>
#include <js/js_string.h>
#include <js/macros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

static JSToken *modify_token(JSToken *token) {
  if (token == 0)
    return token;
  if (token->value == 0)
    return token;

  if (strcmp(token->value, "if") == 0)
    token->type = TOKEN_IF;
  else if (strcmp(token->value, "else") == 0)
    token->type = TOKEN_ELSE;
  else if (strcmp(token->value, "while") == 0)
    token->type = TOKEN_WHILE;
  else if (strcmp(token->value, "for") == 0)
    token->type = TOKEN_FOR;
  else if (strcmp(token->value, "return") == 0)
    token->type = TOKEN_RETURN;
  else if (strcmp(token->value, "new") == 0)
    token->type = TOKEN_NEW;
  else if (strcmp(token->value, "import") == 0)
    token->type = TOKEN_IMPORT;
  else if (strcmp(token->value, "export") == 0)
    token->type = TOKEN_EXPORT;
  else if (strcmp(token->value, "default") == 0)
    token->type = TOKEN_DEFAULT;
  else if (strcmp(token->value, "from") == 0)
    token->type = TOKEN_FROM;
  else if (strcmp(token->value, "as") == 0)
    token->type = TOKEN_AS;
  else if (strcmp(token->value, "const") == 0)
    token->type = TOKEN_CONST;
  else if (strcmp(token->value, "let") == 0)
    token->type = TOKEN_LET;
  else if (strcmp(token->value, "var") == 0)
    token->type = TOKEN_VAR;
  else if (strcmp(token->value, "function") == 0)
    token->type = TOKEN_FUNCTION;
  else if (strcmp(token->value, "interface") == 0)
    token->type = TOKEN_INTERFACE;
  else if (strcmp(token->value, "try") == 0)
    token->type = TOKEN_TRY;
  else if (strcmp(token->value, "catch") == 0)
    token->type = TOKEN_CATCH;
  else if (strcmp(token->value, "finally") == 0)
    token->type = TOKEN_FINALLY;
  else if (strcmp(token->value, "finally") == 0)
    token->type = TOKEN_CATCH;
  else if (strcmp(token->value, "throw") == 0)
    token->type = TOKEN_THROW;
  else if (strcmp(token->value, "typeof") == 0)
    token->type = TOKEN_TYPEOF;
  else if (strcmp(token->value, "in") == 0)
    token->type = TOKEN_IN;
  else if (strcmp(token->value, "of") == 0)
    token->type = TOKEN_OF;
  else if (strcmp(token->value, "do") == 0)
    token->type = TOKEN_DO;
  else if (strcmp(token->value, "switch") == 0)
    token->type = TOKEN_SWITCH;
  else if (strcmp(token->value, "case") == 0)
    token->type = TOKEN_CASE;
  else if (strcmp(token->value, "break") == 0)
    token->type = TOKEN_BREAK;
  else if (strcmp(token->value, "instanceof") == 0)
    token->type = TOKEN_INSTANCEOF;
  else if (strcmp(token->value, "void") == 0)
    token->type = TOKEN_VOID;
  else if (strcmp(token->value, "async") == 0)
    token->type = TOKEN_ASYNC;
  else if (strcmp(token->value, "await") == 0)
    token->type = TOKEN_AWAIT;
  else if (strcmp(token->value, "assert") == 0)
    token->type = TOKEN_ASSERT;
  else if (strcmp(token->value, "delete") == 0)
    token->type = TOKEN_DELETE;
  else if (strcmp(token->value, "class") == 0)
    token->type = TOKEN_CLASS;
  else if (strcmp(token->value, "extends") == 0)
    token->type = TOKEN_EXTENDS;
  else if (strcmp(token->value, "get") == 0)
    token->type = TOKEN_GET;
  else if (strcmp(token->value, "public") == 0)
    token->type = TOKEN_PUBLIC;
  else if (strcmp(token->value, "private") == 0)
    token->type = TOKEN_PRIVATE;

  return token;
}

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

    if (isdigit(lexer->c))
      return js_lexer_parse_num(lexer);

    if (lexer->c == '\'' || lexer->c == '"')
      return js_lexer_parse_str(lexer);

    if (lexer->c == '&' && js_lexer_peek(lexer, 1) == '&') {
      return js_lexer_tok_n(lexer, TOKEN_AND_AND, 2);
    }

    if (lexer->c == '=' && js_lexer_peek(lexer, 1) == '=') {
      return js_lexer_tok_n(lexer, TOKEN_EQUALS_EQUALS, 2);
    }

    if (lexer->c == '=' && js_lexer_peek(lexer, 1) == '>') {
      return js_lexer_tok_n(lexer, TOKEN_ARROW_RIGHT, 2);
    }

    switch (lexer->c) {
    case '{':
      return js_lexer_tok(lexer, TOKEN_LBRACE);
    case '}':
      return js_lexer_tok(lexer, TOKEN_RBRACE);
    case '(':
      return js_lexer_tok(lexer, TOKEN_LPAREN);
    case ')':
      return js_lexer_tok(lexer, TOKEN_RPAREN);
    case '[':
      return js_lexer_tok(lexer, TOKEN_LBRACKET);
    case ']':
      return js_lexer_tok(lexer, TOKEN_RBRACKET);
    case ';':
      return js_lexer_tok(lexer, TOKEN_SEMI);
    case '=':
      return js_lexer_tok(lexer, TOKEN_EQUALS);
    case '<':
      return js_lexer_tok(lexer, TOKEN_LT);
    case '>':
      return js_lexer_tok(lexer, TOKEN_GT);
    case '.':
      return js_lexer_tok(lexer, TOKEN_DOT);
    case ',':
      return js_lexer_tok(lexer, TOKEN_COMMA);
    case ':':
      return js_lexer_tok(lexer, TOKEN_COLON);
    case '!':
      return js_lexer_tok(lexer, TOKEN_NOT);
    case '&':
      return js_lexer_tok(lexer, TOKEN_AND);
    case '+':
      return js_lexer_tok(lexer, TOKEN_PLUS);
    case '-':
      return js_lexer_tok(lexer, TOKEN_MINUS);
    case '/':
      return js_lexer_tok(lexer, TOKEN_DIV);
    case '%':
      return js_lexer_tok(lexer, TOKEN_MOD);
    case '|':
      return js_lexer_tok(lexer, TOKEN_PIPE);
    case '*':
      return js_lexer_tok(lexer, TOKEN_STAR);
    case '?':
      return js_lexer_tok(lexer, TOKEN_QUESTION);
    case '\\':
      return js_lexer_tok(lexer, TOKEN_ESCAPE);
    case '#':
      return js_lexer_tok(lexer, TOKEN_HASH);
    case '^':
      return js_lexer_tok(lexer, TOKEN_SQUARED);
    case '~':
      return js_lexer_tok(lexer, TOKEN_TILDE);
    case '\0':
      break;
    }
  }

  return init_js_token(TOKEN_EOF, 0);
}

void js_lexer_skip_whitespace(JSLexer *lexer) {
  while (JS_LEXER_DONE(lexer) == 0 &&
         (lexer->c == ' ' || lexer->c == '\n' || lexer->c == '\r' ||
          lexer->c == 13 || lexer->c == '\t')) {
    js_lexer_advance(lexer);
  }
}

JSToken *js_lexer_parse_str(JSLexer *lexer) {
  char *str = 0;
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

  JSToken *token = init_js_token(TOKEN_STRING, str ? str : strdup(""));

  if (str) {
    free(str);
  }

  token->c = start;

  return token;
}

JSToken *js_lexer_parse_num(JSLexer *lexer) {
  char *s = 0;
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

  JSToken *tok = init_js_token(type, s);

  if (s) {
    free(s);
  }

  return tok;
}

JSToken *js_lexer_parse_id(JSLexer *lexer) {
  char *s = 0;

  while (isalpha(lexer->c) || lexer->c == '_') {
    js_str_append(&s, lexer->cstr);
    js_lexer_advance(lexer);
  }

  JSToken *tok = init_js_token(TOKEN_ID, s);

  if (s) {
    free(s);
  }

  return modify_token(tok);
}

JSToken *js_lexer_tok(JSLexer *lexer, JSTokenType type) {
  char *value = char_to_str(lexer->c);
  JSToken *tok = init_js_token(type, value);
  free(value);
  js_lexer_advance(lexer);

  return modify_token(tok);
}

JSToken *js_lexer_tok_n(JSLexer *lexer, JSTokenType type, uint32_t n) {
  char *value = 0;
  for (uint32_t i = 0; i < n; i++) {
    char *v = char_to_str(lexer->c);
    js_str_append(&value, v);
    free(v);
    js_lexer_advance(lexer);
  }

  JSToken *tok = init_js_token(type, value);

  return modify_token(tok);
}

char js_lexer_peek(JSLexer *lexer, uint32_t i) {
  return lexer->src[MIN(lexer->i + i, lexer->len - 1)];
}
