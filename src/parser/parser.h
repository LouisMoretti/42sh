#ifndef PARSER_H
#define PARSER_H

#include "parser/ast.h"

struct ast *parse_input();
struct ast *parse_simple_cmd();

#endif // ! PARSER_H
