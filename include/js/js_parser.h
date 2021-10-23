#ifndef JS_ENGINE_PARSER_H
#define JS_ENGINE_PARSER_H
#include <js/js_lexer.h>
#include <js/js_AST.h>

typedef struct JS_PARSER_STRUCT {
  JSLexer* lexer;
  JSToken* token;
} JSParser;

JSParser* init_js_parser(JSLexer* lexer);

void js_parser_eat(JSParser* parser, JSTokenType type);


JSAST* js_parser_parse_num(JSParser* parser);
JSAST* js_parser_parse_str(JSParser* parser);
JSAST* js_parser_parse_id(JSParser* parser);
JSAST* js_parser_parse_call(JSParser* parser);
JSAST* js_parser_parse_array(JSParser* parser);
JSAST* js_parser_parse_function(JSParser* parser);

JSAST* js_parser_parse_factor(JSParser* parser);
JSAST* js_parser_parse_term(JSParser* parser);
JSAST* js_parser_parse_expr(JSParser* parser);
JSAST* js_parser_parse_statement(JSParser* parser);
JSAST* js_parser_parse_compound(JSParser* parser);
JSAST* js_parser_parse(JSParser* parser);


#endif
