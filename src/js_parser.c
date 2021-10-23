#include <js/js_parser.h>
#include <js/macros.h>
#include <stdio.h>

JSParser *init_js_parser(JSLexer *lexer) {
  JSParser *parser = NEW(JSParser);
  parser->lexer = lexer;
  parser->token = js_lexer_get_next_token(parser->lexer);

  return parser;
}

void js_parser_eat(JSParser* parser, JSTokenType type) {
  if (parser->token->type != type) {
    printf("Parser: Unexpected token `%s`\n", parser->token->value);
    exit(1);
  }

  parser->token = js_lexer_get_next_token(parser->lexer);
}

JSAST *js_parser_parse_num(JSParser *parser) {
  JSAST* ast = init_js_ast(JS_AST_NUMBER);
  ast->value_num = atof(parser->token->value);
  if (parser->token->type == TOKEN_INT) {
    js_parser_eat(parser, TOKEN_INT);
  } else {
    js_parser_eat(parser, TOKEN_FLOAT);
  }
  return ast;
}
JSAST *js_parser_parse_str(JSParser *parser) {

  JSAST* ast = init_js_ast(JS_AST_STRING);
  ast->value_str = strdup(parser->token->value);
  js_parser_eat(parser, TOKEN_STRING);
  return ast;
}
JSAST *js_parser_parse_id(JSParser *parser) {
  JSAST* ast = init_js_ast(JS_AST_ID);
  ast->value_str = strdup(parser->token->value);
  js_parser_eat(parser, TOKEN_ID);
  return ast;
}
JSAST *js_parser_parse_call(JSParser *parser) { return 0; }
JSAST *js_parser_parse_array(JSParser *parser) { return 0; }
JSAST *js_parser_parse_function(JSParser *parser) { return 0; }

JSAST *js_parser_parse_factor(JSParser *parser) {
  switch (parser->token->type) {
    case TOKEN_STRING: return js_parser_parse_str(parser); break;
    case TOKEN_INT: return js_parser_parse_num(parser); break;
    case TOKEN_FLOAT: return js_parser_parse_num(parser); break;
    case TOKEN_ID: return js_parser_parse_id(parser); break;
    default: {
     return 0;
    }
  }

  return 0;
}

JSAST *js_parser_parse_term(JSParser *parser) {
  JSAST* left = js_parser_parse_factor(parser);

  while (left && (parser->token->type == TOKEN_PLUS || parser->token->type == TOKEN_MINUS)) {
    JSAST* binop = init_js_ast(JS_AST_BINOP);
    binop->left = left;
    binop->token_type = parser->token->type;
    js_parser_eat(parser, parser->token->type);
    binop->right = js_parser_parse_expr(parser);
    left = binop;
  }

  return left;

}
JSAST *js_parser_parse_expr(JSParser *parser) { return 0; }
JSAST *js_parser_parse_statement(JSParser *parser) { return 0; }
JSAST *js_parser_parse_compound(JSParser *parser) { return 0; }
JSAST *js_parser_parse(JSParser *parser) {
  return js_parser_parse_compound(parser);
}
