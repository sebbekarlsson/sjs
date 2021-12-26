#ifndef JS_ENGINE_PARSER_H
#define JS_ENGINE_PARSER_H
#include <js/js_AST.h>
#include <js/js_lexer.h>

typedef struct JS_PARSER_STRUCT {
  JSLexer *lexer;
  JSToken *token;
  struct JS_EXECUTION_STRUCT *execution;
} JSParser;

JSParser *init_js_parser(JSLexer *lexer, struct JS_EXECUTION_STRUCT *execution);

void js_parser_free(JSParser *parser);

void js_parser_eat(JSParser *parser, JSTokenType type);

JSAST *js_parser_parse_num(JSParser *parser);
JSAST *js_parser_parse_str(JSParser *parser);
JSAST *js_parser_parse_id(JSParser *parser);
JSAST *js_parser_parse_call(JSParser *parser);
JSAST *js_parser_parse_array(JSParser *parser);
JSAST *js_parser_parse_function(JSParser *parser);
JSAST *js_parser_parse_class(JSParser *parser);
JSAST *js_parser_parse_object(JSParser *parser);
void js_parser_parse_object_row(JSParser *parser, JSASTTuple *tuple);

JSAST *js_parser_parse_import(JSParser *parser);
JSAST *js_parser_parse_if(JSParser *parser);
JSAST *js_parser_parse_try(JSParser *parser);
JSAST *js_parser_parse_while(JSParser *parser);
JSAST *js_parser_parse_for(JSParser *parser);
JSAST *js_parser_parse_construct(JSParser *parser);
JSAST *js_parser_parse_throw(JSParser *parser);
JSAST *js_parser_parse_definition(JSParser *parser);
JSAST *js_parser_parse_assignment(JSParser *parser);

JSAST *js_parser_parse_factor(JSParser *parser);
JSAST *js_parser_parse_term(JSParser *parser);
JSAST *js_parser_parse_expr(JSParser *parser);
JSAST *js_parser_parse_statement(JSParser *parser);
JSAST *js_parser_parse_any_statement(JSParser *parser);
JSAST *js_parser_parse_compound(JSParser *parser);
JSAST *js_parser_parse_body(JSParser *parser);
JSAST *js_parser_parse(JSParser *parser);

void js_parser_parse_multiple(JSParser *parser, list_T *list,
                              JSTokenType delim);

#endif
