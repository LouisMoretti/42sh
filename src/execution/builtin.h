#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser/ast.h"

int builtin_echo(struct ast_simple_cmd *command);
int builtin_false(struct ast_simple_cmd *ast_simple_cmd);
int builtin_true(struct ast_simple_cmd *ast_simple_cmd);

#endif // ! BUILTIN_H
