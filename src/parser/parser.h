#ifndef PARSER_H
#define PARSER_H

#include "parser/ast.h"

struct ast_input *parse_input();
struct ast_simple_cmd *parse_simple_cmd();

#endif // ! PARSER_H
