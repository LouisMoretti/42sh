#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser/ast.h"

int builtin_echo(struct ast_simple_cmd *command);
int builtin_false(void);
int builtin_true(void);

#endif // ! BUILTIN_H
