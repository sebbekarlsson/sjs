#include <js/js_parser.h>
#include <js/macros.h>
#include <stdio.h>
#include <string.h>

static unsigned int should_parse_statement(JSParser *parser) {
  JSToken *token = parser->token;
  if (token == 0)
    return 0;

  return token->type == TOKEN_IF || token->type == TOKEN_RETURN ||
         token->type == TOKEN_BREAK || token->type == TOKEN_ELSE ||
         token->type == TOKEN_WHILE || token->type == TOKEN_FOR ||
         token->type == TOKEN_FUNCTION || token->type == TOKEN_CONST ||
         token->type == TOKEN_VAR || token->type == TOKEN_LET;
}

static unsigned int should_parse_definition(JSParser *parser) {
  JSToken *token = parser->token;
  if (token == 0)
    return 0;

  return token->type == TOKEN_WHILE || token->type == TOKEN_FUNCTION ||
         token->type == TOKEN_CONST || token->type == TOKEN_VAR ||
         token->type == TOKEN_LET;
}

JSParser *init_js_parser(JSLexer *lexer) {
  JSParser *parser = NEW(JSParser);
  parser->lexer = lexer;
  parser->token = js_lexer_get_next_token(parser->lexer);

  return parser;
}

void js_parser_eat(JSParser *parser, JSTokenType type) {
  if (parser->token->type != type) {
    printf("Parser: Unexpected token `%s`\n", parser->token->value);
    exit(1);
  }

  parser->token = js_lexer_get_next_token(parser->lexer);
}

JSAST *js_parser_parse_num(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_NUMBER);
  ast->value_num = atof(parser->token->value);
  ast->value_int = (int)ast->value_num;
  if (parser->token->type == TOKEN_INT) {
    js_parser_eat(parser, TOKEN_INT);
  } else {
    js_parser_eat(parser, TOKEN_FLOAT);
  }
  return ast;
}
JSAST *js_parser_parse_str(JSParser *parser) {

  JSAST *ast = init_js_ast(JS_AST_STRING);
  ast->value_str = strdup(parser->token->value);
  js_parser_eat(parser, TOKEN_STRING);
  return ast;
}
JSAST *js_parser_parse_id(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_ID);
  ast->value_str = strdup(parser->token->value);
  js_parser_eat(parser, TOKEN_ID);
  return ast;
}
JSAST *js_parser_parse_array(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_ARRAY);
  js_parser_eat(parser, TOKEN_LBRACKET);

  if (parser->token->type != TOKEN_RBRACKET) {
    js_parser_parse_multiple(parser, ast->children, TOKEN_COMMA);
  }

  js_parser_eat(parser, TOKEN_RBRACKET);

  return ast;
}

JSAST *js_parser_parse_function(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_FUNCTION);
  js_parser_eat(parser, TOKEN_FUNCTION);
  if (parser->token->type == TOKEN_ID) {
    ast->value_str = strdup(parser->token->value);
    js_parser_eat(parser, TOKEN_ID);
  }

  js_parser_eat(parser, TOKEN_LPAREN);
  if (parser->token->type != TOKEN_RPAREN) {
    js_parser_parse_multiple(parser, ast->args, TOKEN_COMMA);
  }
  js_parser_eat(parser, TOKEN_RPAREN);

  js_parser_eat(parser, TOKEN_LBRACE);
  if (parser->token->type != TOKEN_RBRACE) {
    ast->body = js_parser_parse_body(parser);
  }
  js_parser_eat(parser, TOKEN_RBRACE);

  return ast;
}

void js_parser_parse_object_row(JSParser *parser, JSASTTuple *tuple) {
  JSASTTuple tup = *tuple;
  JSAST *keyast = js_parser_parse_factor(parser);
  JSAST *valueast = 0;
  if (parser->token->type == TOKEN_COLON) {
    js_parser_eat(parser, TOKEN_COLON);
    valueast = js_parser_parse_expr(parser);
  }

  tup.x = keyast;
  tup.y = valueast;

  *tuple = tup;
}

JSAST *js_parser_parse_object(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_OBJECT);
  js_parser_eat(parser, TOKEN_LBRACE);
  if (parser->token->type != TOKEN_RBRACE) {
    JSASTTuple tuple = (JSASTTuple){0, 0};
    js_parser_parse_object_row(parser, &tuple);
    char *tstr = js_ast_str_value(tuple.x);
    if (tstr != 0) {
      map_set(ast->keyvalue, tstr, tuple.y);
      free(tstr);
    }
    while (parser->token->type == TOKEN_COMMA) {
      js_parser_eat(parser, TOKEN_COMMA);
      tuple = (JSASTTuple){0, 0};
      js_parser_parse_object_row(parser, &tuple);
      char *tstr = js_ast_str_value(tuple.x);
      if (tstr != 0) {
        map_set(ast->keyvalue, tstr, tuple.y);
        free(tstr);
      }
    }
  }
  js_parser_eat(parser, TOKEN_RBRACE);

  return ast;
}

JSAST *js_parser_parse_if(JSParser *parser) {

  JSAST *ast = init_js_ast(JS_AST_IF);

  if (parser->token->type == TOKEN_ELSE) {
    ast->type = JS_AST_ELSE;
    js_parser_eat(parser, TOKEN_ELSE);
  }

  if (parser->token->type == TOKEN_IF) {
    js_parser_eat(parser, TOKEN_IF);
  }

  if (parser->token->type == TOKEN_LPAREN) {
    js_parser_eat(parser, TOKEN_LPAREN);

    if (parser->token->type != TOKEN_RPAREN) {
      ast->expr = js_parser_parse_expr(parser);
    }

    js_parser_eat(parser, TOKEN_RPAREN);
  }

  if (parser->token->type == TOKEN_LBRACE) {
    js_parser_eat(parser, TOKEN_LBRACE);

    if (parser->token->type != TOKEN_RBRACE) {
      ast->body = js_parser_parse_body(parser);
    }
    js_parser_eat(parser, TOKEN_RBRACE);
  }

  if (parser->token->type == TOKEN_ELSE) {
    ast->right = js_parser_parse_if(parser);
  }

  return ast;
}
JSAST *js_parser_parse_while(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_WHILE);
  js_parser_eat(parser, TOKEN_WHILE);
  js_parser_eat(parser, TOKEN_LPAREN);
  if (parser->token->type != TOKEN_RPAREN) {
    ast->expr = js_parser_parse_expr(parser);
  }
  js_parser_eat(parser, TOKEN_RPAREN);

  if (parser->token->type == TOKEN_LBRACE) {
    js_parser_eat(parser, TOKEN_LBRACE);

    if (parser->token->type != TOKEN_RBRACE) {
      ast->body = js_parser_parse_body(parser);
    }
    js_parser_eat(parser, TOKEN_RBRACE);
  } else {
    ast->body = js_parser_parse_expr(parser);
  }

  return ast;
}
JSAST *js_parser_parse_for(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_FOR);
  js_parser_eat(parser, TOKEN_FOR);
  js_parser_eat(parser, TOKEN_LPAREN);
  if (parser->token->type != TOKEN_RPAREN) {
    js_parser_parse_multiple(parser, ast->args, TOKEN_SEMI);
  }
  js_parser_eat(parser, TOKEN_RPAREN);

  if (parser->token->type == TOKEN_LBRACE) {
    js_parser_eat(parser, TOKEN_LBRACE);

    if (parser->token->type != TOKEN_RBRACE) {
      ast->body = js_parser_parse_body(parser);
    }
    js_parser_eat(parser, TOKEN_RBRACE);
  } else {
    ast->body = js_parser_parse_expr(parser);
  }
  return ast;
}

JSAST *js_parser_parse_factor(JSParser *parser) {
  if (should_parse_definition(parser)) {
    return js_parser_parse_definition(parser);
  }
  if (parser->token->type == TOKEN_LBRACE) {
    return js_parser_parse_object(parser);
  }
  if (parser->token->type == TOKEN_LBRACKET) {
    return js_parser_parse_array(parser);
  }
  if (parser->token->type == TOKEN_LPAREN) {
    js_parser_eat(parser, TOKEN_LPAREN);
    JSAST *ast = 0;
    list_T *args = init_list(sizeof(JSAST *));
    if (parser->token->type != TOKEN_RPAREN) {
      js_parser_parse_multiple(parser, args, TOKEN_COMMA);
    }
    js_parser_eat(parser, TOKEN_RPAREN);

    if (ast && (parser->token->type == TOKEN_DIV ||
                parser->token->type == TOKEN_STAR ||
                parser->token->type == TOKEN_MINUS ||
                parser->token->type == TOKEN_PLUS)) {
      JSAST *binop = init_js_ast(JS_AST_BINOP);
      binop->left = ast;
      binop->token_type = parser->token->type;
      js_parser_eat(parser, parser->token->type);
      binop->right = js_parser_parse_expr(parser);
      ast = binop;
    }

    if (parser->token->type == TOKEN_ARROW_RIGHT) {
      js_parser_eat(parser, TOKEN_ARROW_RIGHT);
      ast = init_js_ast(JS_AST_FUNCTION);
      list_free_shallow(ast->args);
      ast->args = args;
      if (parser->token->type == TOKEN_LBRACE) {
        js_parser_eat(parser, TOKEN_LBRACE);
        if (parser->token->type != TOKEN_RBRACE) {
          ast->body = js_parser_parse_body(parser);
        }
        js_parser_eat(parser, TOKEN_RBRACE);
      } else {
        ast->body = js_parser_parse_expr(parser);
      }
    }

    return ast;
  }
  switch (parser->token->type) {
  case TOKEN_STRING:
    return js_parser_parse_str(parser);
    break;
  case TOKEN_INT:
    return js_parser_parse_num(parser);
    break;
  case TOKEN_FLOAT:
    return js_parser_parse_num(parser);
    break;
  case TOKEN_ID:
    return js_parser_parse_id(parser);
    break;
  default: {
    if (parser->token->type != TOKEN_EOF) {
      printf("Parser.factor: Unexpected `%s`\n", parser->token->value);
      exit(1);
    }
  }
  }

  return 0;
}

JSAST *js_parser_parse_term(JSParser *parser) {
  JSAST *left = js_parser_parse_factor(parser);

  while (left && (parser->token->type == TOKEN_MINUS_EQUALS ||
                  parser->token->type == TOKEN_PLUS_EQUALS)) {
    JSAST *assign = init_js_ast(JS_AST_ASSIGNMENT);
    assign->left = left;
    assign->token_type = parser->token->type;
    js_parser_eat(parser, parser->token->type);
    assign->right = js_parser_parse_expr(parser);
    left = assign;
  }

  while (left && (parser->token->type == TOKEN_STAR ||
                  parser->token->type == TOKEN_DIV)) {
    JSAST *binop = init_js_ast(JS_AST_BINOP);
    binop->left = left;
    binop->token_type = parser->token->type;
    js_parser_eat(parser, parser->token->type);
    binop->right = js_parser_parse_expr(parser);
    left = binop;
  }

  return left;
}
JSAST *js_parser_parse_expr(JSParser *parser) {
  JSAST *left = js_parser_parse_term(parser);

  while (left && (parser->token->type == TOKEN_DOT)) {
    JSAST *binop = init_js_ast(JS_AST_BINOP);
    binop->left = left;
    binop->token_type = parser->token->type;
    js_parser_eat(parser, parser->token->type);
    binop->right = js_parser_parse_expr(parser);
    left = binop;
  }

  while (left && (parser->token->type == TOKEN_PLUS ||
                  parser->token->type == TOKEN_MINUS)) {
    JSAST *binop = init_js_ast(JS_AST_BINOP);
    binop->left = left;
    binop->token_type = parser->token->type;
    js_parser_eat(parser, parser->token->type);
    binop->right = js_parser_parse_expr(parser);
    left = binop;
  }

  while (left && (parser->token->type == TOKEN_LPAREN)) {
    JSAST *call = init_js_ast(JS_AST_CALL);
    js_ast_set_value_str(call, left->value_str);
    call->left = left;
    js_parser_eat(parser, TOKEN_LPAREN);
    if (parser->token->type != TOKEN_RPAREN) {
      js_parser_parse_multiple(parser, call->args, TOKEN_COMMA);
    }
    js_parser_eat(parser, TOKEN_RPAREN);

    if (parser->token->type == TOKEN_LPAREN) {
      call->right = js_parser_parse_expr(parser);
    }

    left = call;
  }

  // array access
  while (left && (parser->token->type == TOKEN_LBRACKET)) {
    JSAST *access = init_js_ast(JS_AST_PROPERTY_ACCESS);
    js_ast_set_value_str(access, left->value_str);
    access->left = left;
    js_parser_eat(parser, TOKEN_LBRACKET);
    if (parser->token->type != TOKEN_RBRACKET) {
      js_parser_parse_multiple(parser, access->args, TOKEN_COMMA);
    }
    js_parser_eat(parser, TOKEN_RBRACKET);

    if (parser->token->type == TOKEN_LBRACKET) {
      access->right = js_parser_parse_expr(parser);
    }

    left = access;
  }

  while (left && (parser->token->type == TOKEN_EQUALS)) {
    JSAST *assign = init_js_ast(JS_AST_ASSIGNMENT);
    js_ast_set_value_str(assign, left->value_str);
    assign->left = left;
    assign->token_type = TOKEN_EQUALS;
    js_parser_eat(parser, TOKEN_EQUALS);
    assign->right = js_parser_parse_expr(parser);
    left = assign;
  }

  while (left &&
         (parser->token->type == TOKEN_GT || parser->token->type == TOKEN_LT ||
          parser->token->type == TOKEN_EQUALS_EQUALS ||
          parser->token->type == TOKEN_EQUALS_EQUALS_EQUALS ||
          parser->token->type == TOKEN_NOT_EQUALS ||
          parser->token->type == TOKEN_NOT_EQUALS_EQUALS)) {
    JSAST *binop = init_js_ast(JS_AST_BINOP);
    binop->left = left;
    binop->token_type = parser->token->type;
    js_parser_eat(parser, parser->token->type);
    binop->right = js_parser_parse_term(parser);
    left = binop;
  }

  while (left && (parser->token->type == TOKEN_AND_AND)) {
    JSAST *binop = init_js_ast(JS_AST_BINOP);
    binop->left = left;
    binop->token_type = parser->token->type;
    js_parser_eat(parser, parser->token->type);
    binop->right = js_parser_parse_expr(parser);
    left = binop;
  }

  return left;
}
JSAST *js_parser_parse_definition(JSParser *parser) {
  if (parser->token->type == TOKEN_CONST) {
    js_parser_eat(parser, TOKEN_CONST);
  } else if (parser->token->type == TOKEN_VAR) {
    js_parser_eat(parser, TOKEN_VAR);
  } else if (parser->token->type == TOKEN_FUNCTION) {
    return js_parser_parse_function(parser);
  } else {
    js_parser_eat(parser, TOKEN_LET);
  }

  JSAST *ast = js_parser_parse_assignment(parser);
  ast->type = JS_AST_DEFINITION;
  return ast;
}

JSAST *js_parser_parse_assignment(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_ASSIGNMENT);
  ast->left = js_parser_parse_id(parser);

  if (parser->token->type == TOKEN_EQUALS) {
    js_parser_eat(parser, TOKEN_EQUALS);
    ast->right = js_parser_parse_expr(parser);
  }
  return ast;
}

JSAST *js_parser_parse_any_statement(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_STATEMENT);
  ast->value_str = strdup(parser->token->value);

  if (parser->token->type == TOKEN_RETURN)
    ast->type = JS_AST_RETURN;

  js_parser_eat(parser, parser->token->type);
  ast->right = js_parser_parse_expr(parser);
  return ast;
}
JSAST *js_parser_parse_statement(JSParser *parser) {
  switch (parser->token->type) {
  case TOKEN_IF:
  case TOKEN_ELSE:
    return js_parser_parse_if(parser);
    break;
  case TOKEN_WHILE:
    return js_parser_parse_while(parser);
    break;
  case TOKEN_FOR:
    return js_parser_parse_for(parser);
    break;
  case TOKEN_FUNCTION:
    return js_parser_parse_function(parser);
    break;
  case TOKEN_CONST:
    return js_parser_parse_definition(parser);
    break;
  case TOKEN_LET:
    return js_parser_parse_definition(parser);
    break;
  case TOKEN_VAR:
    return js_parser_parse_definition(parser);
    break;
  default:
    return js_parser_parse_any_statement(parser);
    break;
  }
}
JSAST *js_parser_parse_compound(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_COMPOUND);

  if (should_parse_statement(parser)) {
    list_push(ast->children, js_parser_parse_statement(parser));
  } else {
    list_push(ast->children, js_parser_parse_expr(parser));
  }

  while (parser->token->type != TOKEN_EOF) {
    if (parser->token->type == TOKEN_SEMI) {
      js_parser_eat(parser, TOKEN_SEMI);
    }

    if (should_parse_statement(parser)) {
      list_push(ast->children, js_parser_parse_statement(parser));
    } else {
      list_push(ast->children, js_parser_parse_expr(parser));
    }
  }

  return ast;
}

JSAST *js_parser_parse_body(JSParser *parser) {
  JSAST *ast = init_js_ast(JS_AST_COMPOUND);
  if (parser->token->type == TOKEN_RBRACE)
    return ast;

  if (should_parse_statement(parser)) {
    list_push(ast->children, js_parser_parse_statement(parser));
  } else {
    list_push(ast->children, js_parser_parse_expr(parser));
  }

  while (parser->token->type == TOKEN_SEMI) {
    js_parser_eat(parser, TOKEN_SEMI);

    if (parser->token->type == TOKEN_RBRACE)
      return ast;

    if (should_parse_statement(parser)) {
      list_push(ast->children, js_parser_parse_statement(parser));
    } else {
      list_push(ast->children, js_parser_parse_expr(parser));
    }
  }

  return ast;
}

JSAST *js_parser_parse(JSParser *parser) {
  return js_parser_parse_compound(parser);
}

void js_parser_parse_multiple(JSParser *parser, list_T *list,
                              JSTokenType delim) {
  JSAST *child = js_parser_parse_expr(parser);

  if (child != 0)
    list_push(list, child);

  while (parser->token->type == delim && parser->token->type != TOKEN_EOF &&
         parser->token->type != TOKEN_RPAREN) {
    js_parser_eat(parser, delim);
    child = js_parser_parse_expr(parser);

    if (child != 0)
      list_push(list, child);
  }
}
