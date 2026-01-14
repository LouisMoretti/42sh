#ifndef PARSER_H
#define PARSER_H

#include "parser/ast.h"

struct ast *parse_input();
struct ast *parse_list();
struct ast *parse_and_or();
struct ast *parse_pipeline();
struct ast *parse_cmd();
struct ast *parse_simple_cmd();
struct ast *parse_shell_cmd();
struct ast *parse_rule_if();
struct ast *parse_compound_list();
struct ast *parse_else_clause();

#endif // ! PARSER_H
